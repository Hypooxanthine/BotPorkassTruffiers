#pragma once

#include <list>

#include "Controllers/Controller.h"

#include "DAO/TimerDAO.h"
#include "DTO/TimerDTO.h"
#include "Controllers/ControllerExceptions.h"

class TimerController final : public Controller
{
public:
    using TimePoint_Type = std::chrono::time_point<std::chrono::system_clock>;
public:
    TimerController(dpp::cluster& bot);

    ~TimerController();
    
    /**
     * @brief Check if a date is in the past.
     * 
     * @param time The time to check.
     * @return true if the date is in the past, false otherwise.
     */
    static bool IsDatePassed(const TimePoint_Type& time);
    
    /**
     * @brief Get a formatted time string in the format "dd/mm/yy hh:mm:ss".
     * 
     * @param time The time to format.
     * @return std::string The formatted time.
     */
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

    /**
     * @brief Parse an interval string in the format "0d 0h 0m 0s". Example: "1d 2h 3m 4s", "1d 2h", "1d", "2h 4s".
     * 
     * @param interval The interval string.
     * @return int64_t The parsed interval in seconds.
     * 
     * @throw ParsingException if the interval string is invalid.
     */
    static int64_t ParseInteval(const std::string& interval);
    
public:

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
        std::string parseString(const std::string& str) const;

        friend std::ostream& operator<<(std::ostream& os, const Timer& timer);

    private:
        const TimerDTO& m_TimerDTO;
    };

private:

    /**
     * @brief Initialize the controller, loads the timers from persistent storage.
     * 
     */
    void onInit() override;

    void onCreateCommands() const override;

    bool onSlashCommand(const dpp::slashcommand_t& event) override;

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
     * @brief Update a timer.
     * 
     * @param id The id of the timer to update.
     * @param timer The new timer data.
     * 
     * @throw DAOBadID if the timer name is invalid.
     * @throw DAOIDNotFound if there is no timer with the given name.
     * @throw DAOOutputStreamException if there is an error writing to the output stream.
     * @throw PastDateException if the end date is in the past.
     */    
    void updateTimer(const std::string& id, const TimerDTO& timer);

    /**
     * @brief Get all timers.
     * 
     * @return const TimerDAO::Map_Type& The timers.
     */
    inline const auto& getTimers() const { return m_TimerDAO.getDataMap(); }

    void loadTimers();
    void startTimer_NoRegister(const std::string& timerId);
    void sendMessage(const std::string& timerId, const dpp::snowflake& channel) const;
    void sendMessage(const std::string& timerId) const;

private:
    TimerDAO m_TimerDAO;
    std::unordered_map<std::string, dpp::timer> m_RunningDppTimers;
};

namespace std
{
    std::string to_string(const TimerController::Timer& timer);
}