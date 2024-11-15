#include "Controllers/TimerController.h"

static bool INSTANTIATED = false;

TimerController::TimerController(dpp::cluster& bot)
    : Controller(bot)
{
    if (INSTANTIATED)
        throw std::runtime_error("TimerController is a singleton and cannot be instantiated more than once.");
    
    INSTANTIATED = true;
}

TimerController::~TimerController()
{
    INSTANTIATED = false;
}

void TimerController::onInit()
{
    loadTimers();
    
    m_Bot.log(dpp::ll_info, "PingController initialized");
}

void TimerController::onCreateCommands() const
{
    dpp::slashcommand timer("timer", "Timer commands", m_Bot.me.id);

    dpp::command_option timer_set(dpp::co_sub_command, "set", "Set a timer");
        timer_set.add_option(dpp::command_option(dpp::co_string, "name", "Timer name. Must be unique.", true));
        timer_set.add_option(dpp::command_option(dpp::co_integer, "interval", "Interval in seconds between each message.", true));
        timer_set.add_option(dpp::command_option(dpp::co_string, "message", "Message to send.", true));
        timer_set.add_option(dpp::command_option(dpp::co_string, "end", "End time of the timer in dd/mm/yy hh:mm:ss format.", true));
        timer_set.add_option(dpp::command_option(dpp::co_string, "title", "Title of the timer.", false));
        timer_set.add_option(dpp::command_option(dpp::co_string, "start", "Start time of the timer in dd/mm/yy hh:mm:ss format. Default: now.", false));
        timer_set.add_option(dpp::command_option(dpp::co_channel, "channel", "Channel to send the message to. Default: this channel.", false));
        timer_set.add_option(dpp::command_option(dpp::co_string, "image", "Image to send with the message.", false));

    dpp::command_option timer_list(dpp::co_sub_command, "list", "List running timers.");
    dpp::command_option timer_stop(dpp::co_sub_command, "stop", "Stop a running timer.");
        timer_stop.add_option(dpp::command_option(dpp::co_string, "name", "Name of the timer to stop.", true));

    dpp::command_option timer_trigger(dpp::co_sub_command, "trigger", "Trigger a timer.");
        timer_trigger.add_option(dpp::command_option(dpp::co_string, "name", "Name of the timer to trigger.", true));

    timer.add_option(timer_list);
    timer.add_option(timer_set);
    timer.add_option(timer_trigger);
    timer.add_option(timer_stop);

    m_Bot.global_command_create(timer);
}

bool TimerController::onSlashCommand(const dpp::slashcommand_t& event)
{
    if (event.command.get_command_name() != "timer")
        return false;
        
    auto commandName = event.command.get_command_interaction().options[0].name;

    using namespace std::string_literals;

    if (commandName == "set")
    {
        TimerDTO::TimePoint_Type startTime;
        std::string startStr = getParamOrCall(
            event,
            "start",
            []() { 
                return GetFormattedTime(std::chrono::system_clock::now());
            }
        );

        try
        {
            startTime = TimerController::ParseTime(startStr);
        }
        catch(...)
        {
            event.reply(dpp::message("Error: Could not parse start time: " + startStr).set_flags(dpp::m_ephemeral));
            return true;
        }

        TimerDTO::TimePoint_Type endTime;
        std::string endStr = getParam<std::string>(event, "end");

        try
        {
            endTime = TimerController::ParseTime(endStr);
        }
        catch (...)
        {
            event.reply(dpp::message("Error: Could not parse end time: " + endStr).set_flags(dpp::m_ephemeral));
            return true;
        }

        std::string name = getParam<std::string>(event, "name");
        int64_t interval = getParam<int64_t>(event, "interval");
        std::string message = getParam<std::string>(event, "message");
        dpp::snowflake channel = getParamOr(event, "channel", event.command.channel_id);
        std::string image = getParamOr(event, "image", ""s);
        std::string title = getParamOr(event, "title", ""s);

        TimerDTO t;
        t.setName(name);
        t.setChannel(channel);
        t.setStart(startTime);
        t.setEnd(endTime);
        t.setInterval(interval);
        t.setMessage(message);
        t.setImageURL(image);
        t.setTitle(title);

        try
        {
            startTimer(t);
        }
        catch (const std::exception& e)
        {
            event.reply(dpp::message("Error: "s + e.what()).set_flags(dpp::m_ephemeral));
            return true;
        }

        m_Bot.log(dpp::ll_info, "Timer started with message \"" + message + "\".");
        event.reply(dpp::message("Timer started:\n" + std::to_string(Timer(t))).set_flags(dpp::m_ephemeral));
    }
    else if (commandName == "list")
    {
        if (getTimers().empty())
            event.reply(dpp::message("No running timers.").set_flags(dpp::m_ephemeral));
        else
        {
            std::string msg = "Running timers:\n";
            for (const auto& [i, pair] : std::views::enumerate(getTimers()))
            {
                const auto& [_, timerDTO] = pair;
                msg += "Timer " + std::to_string(i) + "\n" + std::to_string(Timer(timerDTO)) + '\n';
            }

            event.reply(dpp::message(msg).set_flags(dpp::m_ephemeral));
        }
    }
    else if (commandName == "stop")
    {
        std::string name = getParam<std::string>(event, "name");

        try
        {
            stopTimer(name);
        }
        catch (...)
        {
            event.reply(dpp::message("Error: Could not delete timer with name: " + name + ".").set_flags(dpp::m_ephemeral));
            return true;
        }

        event.reply(dpp::message("Timer with name \"" + name + "\" stopped.").set_flags(dpp::m_ephemeral));
    }
    else if (commandName == "trigger")
    {
        std::string name = getParam<std::string>(event, "name");

        try
        {
            sendMessage(name);
        }
        catch (...)
        {
            event.reply(dpp::message("Error: Could not trigger timer with name: " + name + ".").set_flags(dpp::m_ephemeral));
            return true;
        }

        event.reply(dpp::message("Timer with name \"" + name + "\" triggered.").set_flags(dpp::m_ephemeral));
    }
    else
    {
        m_Bot.log(dpp::ll_warning, "Unknown timer command: " + commandName);
        return true;
    }

    return true;
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

    auto msg = timer.parseString(timer.getData().getMessage());
    dpp::embed embed;
    
    if (timer.getData().getTitle().empty())
        embed.set_description(msg);
    else
    {
        auto title = timer.parseString(timer.getData().getTitle());
        embed.add_field(title, msg);
    }

    if (!timer.getData().getImageURL().empty())
        embed.set_image(timer.getData().getImageURL());

    m_Bot.message_create(dpp::message(timer.getData().getChannel(), embed));
    m_Bot.log(dpp::ll_info, "Timer \"" + timerId + "\" triggered");
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

std::string TimerController::Timer::parseString(const std::string& str) const
{
    std::string parsedMessage = str;

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

std::ostream& operator<<(std::ostream& os, const TimerController::Timer& timer)
{
    const auto& dto = timer.getData();
    os  << "\tName: " << dto.getName() << '\n'
        << "\tStart: " << TimerController::GetFormattedTime(dto.getStart()) << '\n'
        << "\tEnd: " << TimerController::GetFormattedTime(dto.getEnd()) << '\n'
        << "\tInterval: " << dto.getInterval() << " seconds\n";

    if (!dto.getTitle().empty())
        os << "\tTitle: " << dto.getTitle() << '\n';

    os  << "\tMessage: " << dto.getMessage() << '\n'
        << "\tChannel: " << dto.getChannel() << '\n';
    
    if (!dto.getImageURL().empty())
        os << "\tImage: " << dto.getImageURL() << '\n';

    return os;
}

std::string std::to_string(const TimerController::Timer& timer)
{
    std::stringstream ss;
    ss << timer;
    return ss.str();
}