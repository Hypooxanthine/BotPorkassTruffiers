#pragma once

#include <list>
#include <dpp/dpp.h>

#include "DAO/TimerDAO.h"
#include "DTO/TimerDTO.h"
#include "Controllers/ControllerExceptions.h"

class TimerController
{
public:
    using TimePoint_Type = std::chrono::time_point<std::chrono::system_clock>;
public:
    TimerController(dpp::cluster& bot);

    ~TimerController();

    TimerController(const TimerController&) = delete;

    TimerController& operator=(const TimerController&) = delete;

    TimerController(TimerController&&) = delete;

    TimerController& operator=(TimerController&&) = delete;

    /**
     * @brief Initialize the controller, loads the timers from persistent storage.
     * 
     */
    void init();

    bool handleSlashCommand(const dpp::slashcommand_t& event);

    /**
     * @brief 
     * 
     * @param timer 
     * 
     * @throw DAOBadID if the timer name is invalid.
     * @throw DAOIDAlreadyExists if there is already a timer with the given name.
     * @throw DAOOutputStreamException if there is an error writing to the output stream.
     * @throw PastDateException if the end date is in the past.
     */
    void startTimer(const TimerDTO& timer);

    /**
     * @brief Stop a timer.
     * 
     * @param id The id of the timer to stop.
     * 
     * @throw DAOBadID if the timer name is invalid.
     * @throw DAOIDNotFound if there is no timer with the given name.
     * @throw filesystem_error if there is an error deleting the file.
     */
    void stopTimer(const std::string& id);

    /**
     * @brief Get all timers.
     * 
     * @return const TimerDAO::Map_Type& The timers.
     */
    inline const auto& getTimers() const { return m_TimerDAO.getDataMap(); }
    
    static bool IsDatePassed(const TimePoint_Type& time);
    static std::string GetFormattedTime(const TimePoint_Type& time);

    /**
     * @brief Parse a time string in the format "dd/mm/yy hh:mm:ss".
     * 
     * @param time The time string.
     * @return TimePoint_Type The parsed time.
     * 
     * @throw ParsingException if the time string is invalid.
     */
    static TimePoint_Type ParseTime(const std::string& time);

private:

    /**
     * @brief Timer nested class responsible for handling the timer logic.
     * 
     */
    class Timer
    {
    public:
        Timer(const TimerDTO& timer);

        inline const TimerDTO& getData() const { return m_TimerDTO; }
    
        bool isOver() const;
        int64_t getSecondsToNextInterval() const;
        std::string getParsedMessage() const;

    private:
        const TimerDTO& m_TimerDTO;
    };


private:
    void loadTimers();
    void startTimer_NoRegister(const std::string& timerId);
    void sendMessage(const std::string& timerId);

private:
    dpp::cluster& m_Bot;
    TimerDAO m_TimerDAO;
    std::unordered_map<std::string, dpp::timer> m_RunningDppTimers;
};