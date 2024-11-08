#pragma once

#include <stdexcept>

#include "DAO/DAOConcepts.h"

class DAOBadID : public std::runtime_error
{
public:
    DAOBadID(const std::string& message)
        : std::runtime_error(message)
    {}

    DAOBadID()
        : std::runtime_error("Bad ID.")
    {}

    template <typename ID>
        requires ConvertibleToString<ID>
    DAOBadID(const ID& id)
        : std::runtime_error("Bad ID: " + std::to_string(id))
    {}
};

class DAOIDNotFound : public std::runtime_error
{
public:
    DAOIDNotFound(const std::string& message)
        : std::runtime_error(message)
    {}

    DAOIDNotFound()
        : std::runtime_error("Element with the given ID not found.")
    {}

    template <typename ID>
        requires ConvertibleToString<ID>
    DAOIDNotFound(const ID& id)
        : std::runtime_error("Element with the ID " + std::to_string(id) + " not found.")
    {}
};

class DAOIDAlreadyExists : public std::runtime_error
{
public:
    DAOIDAlreadyExists(const std::string& message)
        : std::runtime_error(message)
    {}

    DAOIDAlreadyExists()
        : std::runtime_error("Element with the given ID already exists.")
    {}

    template <typename ID>
        requires ConvertibleToString<ID>
    DAOIDAlreadyExists(const ID& id)
        : std::runtime_error("Element with the ID " + std::to_string(id) + " already exists.")
    {}
};

class DAOInputStreamException : public std::runtime_error
{
public:
    DAOInputStreamException(const std::string& message)
        : std::runtime_error(message)
    {}

    DAOInputStreamException()
        : std::runtime_error("Error reading from input stream.")
    {}

    template <typename ID>
        requires ConvertibleToString<ID>
    DAOInputStreamException(const ID& id)
        : std::runtime_error("Error reading from input stream for ID " + std::to_string(id))
    {}
};

class DAOOutputStreamException : public std::runtime_error
{
public:
    DAOOutputStreamException(const std::string& message)
        : std::runtime_error(message)
    {}

    DAOOutputStreamException()
        : std::runtime_error("Error writing to output stream.")
    {}

    template <typename ID>
        requires ConvertibleToString<ID>
    DAOOutputStreamException(const ID& id)
        : std::runtime_error("Error writing to output stream for ID " + std::to_string(id))
    {}
};

class DAOParsingException : public std::runtime_error
{
public:
    DAOParsingException(const std::string& message)
        : std::runtime_error(message)
    {}

    DAOParsingException()
        : std::runtime_error("Error parsing data.")
    {}

    template <typename ID>
        requires ConvertibleToString<ID>
    DAOParsingException(const ID& id)
        : std::runtime_error("Error parsing data for ID " + std::to_string(id))
    {}
};