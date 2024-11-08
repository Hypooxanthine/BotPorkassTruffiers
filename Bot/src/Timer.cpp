#include "Timer.h"

#include <fstream>
#include <iomanip>

Timer::Timer(dpp::cluster& bot)
    : m_Bot(bot)
{
}

Timer::Timer(dpp::cluster& bot, const std::string& timerName, dpp::snowflake channel, int64_t intervalSeconds, const std::string& message, const TimePoint_Type& start, const TimePoint_Type& end)
    : m_Bot(bot), m_Name(timerName), m_Channel(channel), m_IntervalSeconds(intervalSeconds), m_Message(message), m_Start(start), m_End(end)
{
}

void Timer::saveToFile(const std::string& filename)
{
    std::ofstream file(filename, std::ios::trunc);

    if (file.is_open())
    {
        using namespace std::chrono;

        file << m_Name << std::endl;
        file << m_Channel << std::endl;
        file << m_IntervalSeconds << std::endl;
        file << m_Message << std::endl;
        file << duration_cast<seconds>(m_Start.time_since_epoch()).count() << std::endl;
        file << duration_cast<seconds>(m_End.time_since_epoch()).count() << std::endl;
    }

    file.close();
}

void Timer::loadFromFile(const std::string& filename)
{
    std::ifstream file(filename);

    if (file.is_open())
    {
        using namespace std::chrono;

        file >> m_Name;
        file >> m_Channel;
        file >> m_IntervalSeconds;
        file >> m_Message;

        int64_t start, end;
        file >> start;
        file >> end;

        m_Start = TimePoint_Type(seconds(start));
        m_End = TimePoint_Type(seconds(end));
    }

    file.close();
}

void Timer::onEnd(const std::function<void()>& callback)
{
    m_OnEnd = callback;
}

void Timer::start()
{
    int64_t secondsToNextInterval;

    try
    {
        secondsToNextInterval = getSecondsToNextInterval();
    }
    catch (const std::runtime_error& e)
    {
        m_Bot.log(dpp::ll_error, e.what());
        return;
    }
        
    m_Timer = m_Bot.start_timer([this](const dpp::timer& timer) {
        sendMessage();

        if (isOver())
        {
            stop();
            return;
        }
        else
        {
            m_Timer = m_Bot.start_timer([this](const dpp::timer& timer) {
                if (isOver())
                {
                    stop();
                    return;
                }

                sendMessage();
            }, m_IntervalSeconds);
        }

        m_Bot.stop_timer(timer);
    }, secondsToNextInterval);
}

void Timer::stop()
{
    m_Bot.stop_timer(m_Timer);
    if (m_OnEnd)
        m_OnEnd();
}

bool Timer::isOver() const
{
    return IsDatePassed(m_End);
}

int64_t Timer::getSecondsToNextInterval() const
{
    using namespace std::chrono;

    auto now = system_clock::now();

    if (now > m_End)
        throw std::runtime_error("Timer is already over");
    
    if (now < m_Start)
        return duration_cast<seconds>(m_Start - now).count();
    else
    {
        auto sinceStart = duration_cast<seconds>(now - m_Start).count();
        
        return m_IntervalSeconds - (sinceStart % m_IntervalSeconds);
    }
}

std::string Timer::getParsedMessage() const
{
    std::string parsedMessage = m_Message;

    auto now = std::chrono::system_clock::now();
    auto remaining = m_End - now;
    auto secondsLeft = std::chrono::duration_cast<std::chrono::seconds>(remaining).count();
    
    std::unordered_map<std::string, std::string> replacements = {
        {"{name}", m_Name},
        {"{interval}", std::to_string(m_IntervalSeconds)},
        {"{start}", GetFormattedTime(m_Start)},
        {"{end}", GetFormattedTime(m_End)},
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

void Timer::sendMessage()
{
    auto msg = getParsedMessage();
    dpp::embed embed;
        embed.set_description(msg);
    m_Bot.message_create(dpp::message(m_Channel, embed));
    m_Bot.log(dpp::ll_info, "Timer message: " + msg);
}

bool Timer::IsDatePassed(const TimePoint_Type& time)
{
    return std::chrono::system_clock::now() > time;
}

std::string Timer::GetFormattedTime(const TimePoint_Type& time)
{
    using namespace std::chrono;
    
    auto time_t = system_clock::to_time_t(time);
    auto tm = std::localtime(&time_t);

    std::stringstream ss;
    ss << std::put_time(tm, "%d/%m/%Y %H:%M:%S");

    return ss.str();
}

std::optional<Timer::TimePoint_Type> Timer::ParseTime(const std::string& time)
{
    std::stringstream ss(time);

    int day = -1, month = -1, year = -1, hour = -1, minute = -1, second = -1;
    char sep1, sep2, sep4, sep5;

    ss >> day >> sep1 >> month >> sep2 >> year >> hour >> sep4 >> minute >> sep5 >> second;

    if (ss.fail() || sep1 != '/' || sep2 != '/' || sep4 != ':' || sep5 != ':')
        return std::nullopt;

    std::tm tm = {};
    tm.tm_mday = day;
    tm.tm_mon = month - 1;
    tm.tm_year = year - 1900;
    tm.tm_hour = hour;
    tm.tm_min = minute;
    tm.tm_sec = second;

    auto tp = std::chrono::system_clock::from_time_t(std::mktime(&tm));

    if (tp == std::chrono::system_clock::time_point{}) {
        return std::nullopt;
    }

    return tp;
}