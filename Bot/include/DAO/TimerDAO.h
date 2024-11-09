#pragma once

#include <vector>

#include "DAO/AbstractMapDAO.h"
#include "DTO/TimerDTO.h"

class TimerDAO : public AbstractMapDAO<std::string, TimerDTO>
{
public:
    TimerDAO() = default;
    
    /**
     * @brief Add a new element.
     * @param id The id of the element.
     * @param element The data of the element.
     * 
     * @throw DAOBadID if the id is invalid.
     * @throw DAOIDAlreadyExists if there is already an element with the given id.
     * @throw DAOOutputStreamException if there is an error writing to the output stream.
     */
    void add(const ID_Type& id, const DTO_Type& element) override;

    /**
     * @brief Update an existing element.
     * @param id The id of the element to update.
     * @param element The new data of the element.
     * 
     * @throw DAOBadID if the id is invalid.
     * @throw DAOIDNotFound if there is no element with the given id.
     * @throw DAOOutputStreamException if there is an error writing to the output stream.
     * @throw filesystem_error if there is an error deleting the file.
     */
    void update(const ID_Type& id, const DTO_Type& element) override;

    /**
     * @brief Delete an existing element.
     * @param id The id of the element to delete.
     * 
     * @throw DAOBadID if the id is invalid.
     * @throw DAOIDNotFound if there is no element with the given id.
     * @throw filesystem_error if there is an error deleting the file.
     */
    void deleteByID(const ID_Type& id) override;

    /**
     * @brief Get an element by id.
     * @return const DTO_Type& The element with the given id.
     * 
     * @throw DAOBadID if the id is invalid.
     * @throw DAOIDNotFound if there is no element with the given id.
     */
    const DTO_Type& findOne(const ID_Type& id) const override;

    /**
     * @brief Get all elements.
     * @return std::vector<const DTO_Type&> A vector with all elements.
     */
    std::vector<DTO_Type> findAll() const override;

    /**
     * @brief Check if an element with the given id exists.
     * @return True if the element exists, false otherwise.
     * 
     * @throw DAOBadID if the id is invalid.
     */
    bool idExists(const ID_Type& id) const override;

    /**
     * @brief Check if the given id is valid.
     * @return True if the id is valid, false otherwise.
     */
    bool isIDValid(const ID_Type& id) const override;

    /**
     * @brief Load all timers from the data directory.
     * 
     * @throw DAOInputStreamException if there is an error reading from the input stream.
     */
    void loadTimers();

private:

    /**
     * @brief Write a timer to an output stream.
     * @param os The output stream.
     * @param timer The timer to write.
     * 
     * @throw DAOOutputStreamException if there is an error writing to the output stream.
     */
    void writeTimer(std::ostream& os, const TimerDTO& timer) const;

    /**
     * @brief Read a timer from an input stream.
     * @param is The input stream.
     * @return TimerDTO The timer read from the input stream.
     * 
     * @throw DAOInputStreamException if there is an error reading from the input stream.
     * @throw DAOParseException if there is an error parsing the input.
     */
    TimerDTO readTimer(std::istream& is) const;
};