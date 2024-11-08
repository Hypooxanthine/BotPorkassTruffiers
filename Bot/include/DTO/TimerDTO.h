#pragma once

#include <chrono>
#include <string>

#include <dpp/dpp.h>

class TimerDTO
{
public:
    using TimePoint_Type = std::chrono::time_point<std::chrono::system_clock>;

public:
    TimerDTO() = default;

    TimerDTO(const std::string& name, dpp::snowflake channel, int64_t intervalSeconds, const std::string& message, const TimePoint_Type& start, const TimePoint_Type& end)
        : m_Name(name), m_Channel(channel), m_IntervalSeconds(intervalSeconds), m_Message(message), m_Start(start), m_End(end)
    {}

    TimerDTO(const TimerDTO&) = default;

    inline const std::string& getName() const { return m_Name; }
    inline const dpp::snowflake& getChannel() const { return m_Channel; }
    inline int64_t getInterval() const { return m_IntervalSeconds; }
    inline const std::string& getMessage() const { return m_Message; }
    inline const TimePoint_Type& getStart() const { return m_Start; }
    inline const TimePoint_Type& getEnd() const { return m_End; }

    inline void setName(const std::string& name) { m_Name = name; }
    inline void setChannel(dpp::snowflake channel) { m_Channel = channel; }
    inline void setInterval(int64_t intervalSeconds) { m_IntervalSeconds = intervalSeconds; }
    inline void setMessage(const std::string& message) { m_Message = message; }
    inline void setStart(const TimePoint_Type& start) { m_Start = start; }
    inline void setEnd(const TimePoint_Type& end) { m_End = end; }

    void saveToFile(const std::string& filename);
    void loadFromFile(const std::string& filename);

private:
    std::string m_Name;
    dpp::snowflake m_Channel = 0;
    int64_t m_IntervalSeconds = -1;
    std::string m_Message;
    TimePoint_Type m_Start, m_End;
};