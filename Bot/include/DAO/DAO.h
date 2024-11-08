#pragma once

#include <vector>

#include "DAO/DAOExceptions.h"
#include "DAO/DAOConcepts.h"

template <typename ID, typename DTO>
    requires ConvertibleToString<ID> && std::copyable<DTO>
class IDAO
{
public:
    using ID_Type = ID;
    using DTO_Type = DTO;

public:
    
    /**
     * @brief Add a new element.
     * @param id The id of the element.
     * @param element The data of the element.
     * 
     * @throw DAOBadID if the id is invalid.
     * @throw DAOIDAlreadyExists if there is already an element with the given id.
     * @throw DAOOutputStreamException if there is an error writing to the output stream.
     */
    virtual void add(const ID_Type& id, const DTO_Type& element) = 0;

    /**
     * @brief Update an existing element.
     * @param id The id of the element to update.
     * @param element The new data of the element.
     * 
     * @throw DAOBadID if the id is invalid.
     * @throw DAOIDNotFound if there is no element with the given id.
     * @throw DAOOutputStreamException if there is an error writing to the output stream.
     */
    virtual void update(const ID_Type& id, const DTO_Type& element) = 0;

    /**
     * @brief Delete an existing element.
     * @param id The id of the element to delete.
     * 
     * @throw DAOBadID if the id is invalid.
     * @throw DAOIDNotFound if there is no element with the given id.
     * @throw filesystem_error if there is an error deleting the file.
     */
    virtual void deleteByID(const ID_Type& id) = 0;

    /**
     * @brief Get an element by id.
     * @return const DTO_Type& The element with the given id.
     * 
     * @throw DAOBadID if the id is invalid.
     * @throw DAOIDNotFound if there is no element with the given id.
     */
    virtual const DTO_Type& findOne(const ID_Type& id) const = 0;

    /**
     * @brief Get all elements.
     * @return std::vector<DTO_Type> A vector with all elements.
     */
    virtual std::vector<DTO_Type> findAll() const = 0;

    /**
     * @brief Check if an element with the given id exists.
     * @return True if the element exists, false otherwise.
     * 
     * @throw DAOBadID if the id is invalid.
     */
    virtual bool idExists(const ID_Type& id) const = 0;

    /**
     * @brief Check if the given id is valid.
     * @return True if the id is valid, false otherwise.
     */
    virtual bool isIDValid(const ID_Type& id) const = 0;
};