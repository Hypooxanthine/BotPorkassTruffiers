#pragma once

#include <chrono>
#include <optional>
#include <functional>
#include <dpp/dpp.h>

class Timer
{
public:
    using TimePoint_Type = std::chrono::time_point<std::chrono::system_clock>;
    Timer() = default;
    Timer(dpp::cluster& bot, dpp::snowflake channel, int64_t intervalSeconds, const std::string& message, const TimePoint_Type& start, const TimePoint_Type& end);

    void saveToFile(const std::string& filename);
    void loadFromFile(const std::string& filename);

    void onEnd(const std::function<void()>& callback);

    void start();
    void stop();
    
    static bool IsDatePassed(const TimePoint_Type& time);
    static std::string GetFormattedTime(const TimePoint_Type& time);
    static std::optional<TimePoint_Type> ParseTime(const std::string& time);

private:
    bool isOver() const;
    int64_t getSecondsToNextInterval() const;
    std::string getParsedMessage() const;

private:
    dpp::timer m_Timer;
    dpp::cluster& m_Bot;
    dpp::snowflake m_Channel = 0;
    int64_t m_IntervalSeconds = -1;
    std::string m_Message;
    TimePoint_Type m_Start, m_End;

    std::function<void()> m_OnEnd;
};