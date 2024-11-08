#include "DAO/TimerDAO.h"

#include <filesystem>

void TimerDAO::add(const ID_Type& id, const TimerDTO& timer)
{
    if (!isIDValid(id))
        throw DAOBadID(id);

    if (idExists(id))
        throw DAOIDAlreadyExists(id);
    
    auto file = std::ofstream("data/timers/" + id + ".txt");

    if (!file.is_open())
        throw DAOOutputStreamException();

    try {
        writeTimer(file, timer);
    } catch (...) {
        std::filesystem::remove("data/timers/" + id + ".txt");
        file.close();
        throw;
    }

    m_Elements[id] = timer;
}

void TimerDAO::update(const ID_Type& id, const TimerDTO& timer)
{
    deleteByID(id);
    add(id, timer);
}

void TimerDAO::deleteByID(const ID_Type& id)
{
    if (!isIDValid(id))
        throw DAOBadID(id);

    if (!idExists(id))
        throw DAOIDNotFound(id);

    std::filesystem::remove("data/timers/" + id + ".txt");
    m_Elements.erase(id);
}

const TimerDAO::DTO_Type& TimerDAO::findOne(const ID_Type& id) const
{
    if (!isIDValid(id))
        throw DAOBadID(id);

    if (!idExists(id))
        throw DAOIDNotFound(id);

    return m_Elements.at(id);
}

std::vector<TimerDAO::DTO_Type> TimerDAO::findAll() const
{
    std::vector<TimerDTO> timers;

    for (const auto& [id, timer] : m_Elements)
        timers.push_back(timer);

    return timers;
}

bool TimerDAO::idExists(const ID_Type& id) const
{
    return m_Elements.contains(id);
}

bool TimerDAO::isIDValid(const ID_Type& id) const
{
    return id != "";
}

void TimerDAO::writeTimer(std::ostream& os, const TimerDTO& timer) const
{
    using namespace std::chrono;

    os << timer.getName() << std::endl
        << timer.getChannel() << std::endl
        << timer.getInterval() << std::endl
        << timer.getMessage() << std::endl
        << duration_cast<seconds>(timer.getStart().time_since_epoch()).count() << std::endl
        << duration_cast<seconds>(timer.getEnd().time_since_epoch()).count() << std::endl;

    if (os.bad())
        throw DAOOutputStreamException();
}

TimerDTO TimerDAO::readTimer(std::istream& is) const
{
    using namespace std::chrono;

    std::string name;
    dpp::snowflake channel;
    std::string message;
    uint64_t interval;
    int64_t start;
    int64_t end;

    std::getline(is, name);
    is >> channel;
    is >> interval;
    std::getline(is, message);
    is >> start;
    is >> end;

    if (is.bad())
        throw DAOInputStreamException();

    return TimerDTO(name, channel, interval, message, TimerDTO::TimePoint_Type(seconds(start)), TimerDTO::TimePoint_Type(seconds(end)));
}