#pragma once

#include <chrono>
#include <optional>
#include <functional>
#include <dpp/dpp.h>

class Timer
{
public:
    using TimePoint_Type = std::chrono::time_point<std::chrono::system_clock>;

    Timer(dpp::cluster& bot);
    Timer(dpp::cluster& bot, const std::string& timerName, dpp::snowflake channel, int64_t intervalSeconds, const std::string& message, const TimePoint_Type& start, const TimePoint_Type& end);

    void saveToFile(const std::string& filename);
    void loadFromFile(const std::string& filename);

    void onEnd(const std::function<void()>& callback);

    void start();
    void stop();

    inline const std::string& getName() const { return m_Name; }
    inline const dpp::snowflake& getChannel() const { return m_Channel; }
    inline int64_t getInterval() const { return m_IntervalSeconds; }
    inline const std::string& getMessage() const { return m_Message; }
    inline const TimePoint_Type& getStart() const { return m_Start; }
    inline const TimePoint_Type& getEnd() const { return m_End; }
    
    static bool IsDatePassed(const TimePoint_Type& time);
    static std::string GetFormattedTime(const TimePoint_Type& time);
    static std::optional<TimePoint_Type> ParseTime(const std::string& time);

private:
    bool isOver() const;
    int64_t getSecondsToNextInterval() const;
    std::string getParsedMessage() const;
    void sendMessage();

private:
    dpp::timer m_Timer;
    dpp::cluster& m_Bot;
    std::string m_Name;
    dpp::snowflake m_Channel = 0;
    int64_t m_IntervalSeconds = -1;
    std::string m_Message;
    TimePoint_Type m_Start, m_End;

    std::function<void()> m_OnEnd;
};