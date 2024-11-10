#include "Controllers/TimerController.h"

static bool INSTANTIATED = false;

TimerController::TimerController(dpp::cluster& bot)
    : m_Bot(bot)
{
    if (INSTANTIATED)
        throw std::runtime_error("TimerController is a singleton and cannot be instantiated more than once.");
    
    INSTANTIATED = true;
}

TimerController::~TimerController()
{
    INSTANTIATED = false;
}

void TimerController::init()
{
    loadTimers();
}

bool TimerController::handleSlashCommand(const dpp::slashcommand_t& event)
{
    bool handled = true;
    using namespace std::string_literals;

    if (event.command.get_command_name() == "set_timer")
    {
        dpp::snowflake channel;
        std::visit(
            [&channel, &event](auto&& arg) {
                using T = std::decay_t<decltype(arg)>;
                if constexpr (std::is_same_v<T, dpp::snowflake>)
                    channel = arg;
                if constexpr (std::is_same_v<T, std::monostate>)
                    channel = event.command.channel_id;
            },
            event.get_parameter("channel")
        );

        std::string startStr = "", endStr = std::get<std::string>(event.get_parameter("end"));
        TimerDTO::TimePoint_Type startTime, endTime;
        
        std::visit(
            [&startStr](auto&& arg) {

                using T = std::decay_t<decltype(arg)>;
                if constexpr (std::is_same_v<T, std::string>)
                    startStr = arg;
            },
            event.get_parameter("start")
        );

        try
        {
            if (!startStr.empty())
                startTime = TimerController::ParseTime(startStr);
            else
            {
                startTime = std::chrono::system_clock::now();
                startStr = TimerController::GetFormattedTime(startTime);
            }

            endTime = TimerController::ParseTime(endStr);
        }
        catch (const std::exception& e)
        {
            event.reply(dpp::message("Error: "s + e.what()).set_flags(dpp::m_ephemeral));
            return handled;
        }

        std::string name = std::get<std::string>(event.get_parameter("name"));
        int64_t interval = std::get<int64_t>(event.get_parameter("interval"));
        std::string message = std::get<std::string>(event.get_parameter("message"));

        TimerDTO t;
        t.setName(name);
        t.setChannel(channel);
        t.setStart(startTime);
        t.setEnd(endTime);
        t.setInterval(interval);
        t.setMessage(message);

        try
        {
            startTimer(t);
        }
        catch (const std::exception& e)
        {
            event.reply(dpp::message("Error: "s + e.what()).set_flags(dpp::m_ephemeral));
            return handled;
        }

        m_Bot.log(dpp::ll_info, "Timer started with message \"" + message + "\".");

        std::string msg = "Timer started with message \"" + message + "\".\n";
        msg += " Start: " + startStr + '\n';
        msg += " End: " + endStr + '\n';
        msg += " Interval: " + std::to_string(interval) + " seconds.";

        event.reply(dpp::message(msg).set_flags(dpp::m_ephemeral));
    }
    else if (event.command.get_command_name() == "list_timers")
    {
        std::string msg = "Running timers:\n";
        for (const auto& [name, timer] : getTimers())
        {
            msg += "- " + name + "\n"
                "\tStart: " + TimerController::GetFormattedTime(timer.getStart()) + "\n"
                "\tEnd: " + TimerController::GetFormattedTime(timer.getEnd()) + "\n"
                "\tInterval: " + std::to_string(timer.getInterval()) + " seconds\n"
                "\tMessage: " + timer.getMessage() + "\n";
        }

        event.reply(dpp::message(msg).set_flags(dpp::m_ephemeral));
    }
    else if (event.command.get_command_name() == "stop_timer")
    {
        std::string name = std::get<std::string>(event.get_parameter("name"));

        try
        {
            stopTimer(name);
        }
        catch (const std::exception& e)
        {
            event.reply(dpp::message("Error: "s + e.what()).set_flags(dpp::m_ephemeral));
            return handled;
        }

        event.reply(dpp::message("Timer with name \"" + name + "\" stopped.").set_flags(dpp::m_ephemeral));
    }
    else
        handled = false;

    return handled;
}

void TimerController::startTimer(const TimerDTO& timer)
{
    if (IsDatePassed(timer.getEnd()))
        throw PastDateException("End date is in the past: " + GetFormattedTime(timer.getEnd()));

    m_TimerDAO.add(timer.getName(), timer);

    startTimer_NoRegister(timer.getName());
}

void TimerController::stopTimer(const std::string& id)
{
    m_Bot.log(dpp::ll_info, "Stopping timer with id: " + id);

    try
    {
        m_TimerDAO.deleteByID(id);
    }
    catch (const std::exception& e)
    {
        m_Bot.log(dpp::ll_warning, "Could not delete timer with id: " + id + ". Error: " + e.what());
        throw;
    }

    m_Bot.stop_timer(m_RunningDppTimers.at(id));

    m_RunningDppTimers.erase(id);
    m_Bot.log(dpp::ll_info, "Timer with id: " + id + " stopped.");
}

void TimerController::loadTimers()
{
    m_TimerDAO.loadTimers();
    
    // Remove timers that have already ended
    for (const auto& [id, timer] : m_TimerDAO.getDataMap())
    {
        if (IsDatePassed(timer.getEnd()))
            m_TimerDAO.deleteByID(id);
    }

    // Start timers
    for (const auto& [id, _] : m_TimerDAO.getDataMap())
    {
        startTimer_NoRegister(id);
    }
}

void TimerController::startTimer_NoRegister(const std::string& timerId)
{
    Timer timer(m_TimerDAO.findOne(timerId));

    int64_t secondsToNextInterval;

    try
    {
        secondsToNextInterval = timer.getSecondsToNextInterval();
    }
    catch (const std::runtime_error& e)
    {
        m_Bot.log(dpp::ll_error, e.what());
        return;
    }
    
    m_RunningDppTimers[timerId] = m_Bot.start_timer([this, timerId](const dpp::timer& dppTimer) {
        
        Timer timer(m_TimerDAO.findOne(timerId));

        if (timer.isOver())
        {
            stopTimer(std::string(timerId));
            return;
        }
        else
        {
            sendMessage(timerId);

            m_RunningDppTimers[timerId] = m_Bot.start_timer([this, timerId](const dpp::timer& dppTimer) {

                Timer timer(m_TimerDAO.findOne(timerId));

                if (!timer.isOver())
                    sendMessage(timerId);
                else
                {
                    m_Bot.log(dpp::ll_info, "Timer is over. Timer id: " + timerId);
                    stopTimer(std::string(timerId));
                }

            }, timer.getData().getInterval());
            
            m_Bot.stop_timer(dppTimer);
        }
    }, secondsToNextInterval);
}

void TimerController::sendMessage(const std::string& timerId)
{
    Timer timer(m_TimerDAO.findOne(timerId));

    auto msg = timer.getParsedMessage();
    dpp::embed embed;
        embed.set_description(msg);
    m_Bot.message_create(dpp::message(timer.getData().getChannel(), embed));
    m_Bot.log(dpp::ll_info, "Timer message: " + msg);
}

bool TimerController::IsDatePassed(const TimePoint_Type& time)
{
    return std::chrono::system_clock::now() > time;
}

std::string TimerController::GetFormattedTime(const TimePoint_Type& time)
{
    using namespace std::chrono;
    
    auto time_t = system_clock::to_time_t(time);
    auto tm = std::localtime(&time_t);

    std::stringstream ss;
    ss << std::put_time(tm, "%d/%m/%Y %H:%M:%S");

    return ss.str();
}

TimerController::TimePoint_Type TimerController::ParseTime(const std::string& time)
{
    std::stringstream ss(time);

    int day = -1, month = -1, year = -1, hour = -1, minute = -1, second = -1;
    char sep1, sep2, sep4, sep5;

    ss >> day >> sep1 >> month >> sep2 >> year >> hour >> sep4 >> minute >> sep5 >> second;

    if (ss.fail() || sep1 != '/' || sep2 != '/' || sep4 != ':' || sep5 != ':')
        throw ParsingException("Invalid time format: " + time);

    std::tm tm = {};
    tm.tm_mday = day;
    tm.tm_mon = month - 1;
    tm.tm_year = year - 1900;
    tm.tm_hour = hour;
    tm.tm_min = minute;
    tm.tm_sec = second;

    auto tp = std::chrono::system_clock::from_time_t(std::mktime(&tm));

    if (tp == std::chrono::system_clock::time_point{})
        throw ParsingException("Could not parse time: " + time);

    return tp;
}

/* Timer nested class */

TimerController::Timer::Timer(const TimerDTO& timer)
    : m_TimerDTO(timer)
{}

bool TimerController::Timer::isOver() const
{
    return IsDatePassed(m_TimerDTO.getEnd());
}

int64_t TimerController::Timer::getSecondsToNextInterval() const
{
    using namespace std::chrono;

    auto now = system_clock::now();

    if (now > m_TimerDTO.getEnd())
        throw PastDateException("Timer is already over");
    
    if (now < m_TimerDTO.getStart())
        return duration_cast<seconds>(m_TimerDTO.getStart() - now).count();
    else
    {
        auto sinceStart = duration_cast<seconds>(now - m_TimerDTO.getStart()).count();
        
        return m_TimerDTO.getInterval() - (sinceStart % m_TimerDTO.getInterval());
    }
}

std::string TimerController::Timer::getParsedMessage() const
{
    std::string parsedMessage = m_TimerDTO.getMessage();

    auto now = std::chrono::system_clock::now();
    auto remaining = m_TimerDTO.getEnd() - now;
    auto secondsLeft = std::chrono::duration_cast<std::chrono::seconds>(remaining).count();
    
    std::unordered_map<std::string, std::string> replacements = {
        {"{name}", m_TimerDTO.getName()},
        {"{interval}", std::to_string(m_TimerDTO.getInterval())},
        {"{start}", GetFormattedTime(m_TimerDTO.getStart())},
        {"{end}", GetFormattedTime(m_TimerDTO.getEnd())},
        {"{rem:days}", std::to_string(secondsLeft / 60 / 60 / 24)},
        {"{rem:hours}", std::to_string(secondsLeft / 60 / 60)},
        {"{rem:minuts}", std::to_string(secondsLeft / 60)},
        {"{rem:seconds}", std::to_string(secondsLeft)},
    };

    for (const auto& [placeholder, replacement] : replacements) {
        size_t pos = 0;
        while ((pos = parsedMessage.find(placeholder, pos)) != std::string::npos) {
            parsedMessage.replace(pos, placeholder.length(), replacement);
            pos += replacement.length();
        }
    }

    return parsedMessage;
}