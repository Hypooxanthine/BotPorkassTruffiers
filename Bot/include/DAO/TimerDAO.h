#pragma once

#include <vector>

#include "DAO/IDAO.h"
#include "DTO/TimerDTO.h"

class TimerDAO : public IDAO<std::string, TimerDTO>
{
public:
    TimerDAO() = default;
    
    /**
     * @brief Create a new element.
     * @param id The id of the element.
     * @param element The data of the element.
     * 
     * @throw DAOBadID if the id is invalid.
     * @throw DAOIDAlreadyExists if there is already an element with the given id.
     */
    void create(const ID_Type& id, const DTO_Type& element) const override;

    /**
     * @brief Update an existing element.
     * @param id The id of the element to update.
     * @param element The new data of the element.
     * 
     * @throw DAOBadID if the id is invalid.
     * @throw DAOIDNotFound if there is no element with the given id.
     */
    void update(const ID_Type& id, const DTO_Type& element) const override;

    /**
     * @brief Remove an existing element.
     * @param id The id of the element to remove.
     * 
     * @throw DAOBadID if the id is invalid.
     * @throw DAOIDNotFound if there is no element with the given id.
     */
    void remove(const ID_Type& id) const override;

    /**
     * @brief Get an element by id.
     * @return DTO_Type The element with the given id.
     * 
     * @throw DAOBadID if the id is invalid.
     * @throw DAOIDNotFound if there is no element with the given id.
     */
    DTO_Type get(const ID_Type& id) const override;

    /**
     * @brief Get all elements.
     * @return std::vector<DTO_Type> A vector with all elements.
     */
    std::vector<DTO_Type> getAll() const override;

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

};