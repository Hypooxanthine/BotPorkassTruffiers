#pragma once

#include <stdexcept>

namespace std
{

inline std::string to_string(const std::string& str)
{
    return str;
}

} // namespace std

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
    DAOIDAlreadyExists(const ID& id)
        : std::runtime_error("Element with the ID " + std::to_string(id) + " already exists.")
    {}
};