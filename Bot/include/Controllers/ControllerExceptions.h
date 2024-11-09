#pragma once

#include <stdexcept>

class ParsingException : public std::runtime_error
{
public:
    ParsingException(const std::string& message)
        : std::runtime_error(message)
    {
    }
};

class PastDateException : public std::runtime_error
{
public:
    PastDateException(const std::string& message)
        : std::runtime_error(message)
    {
    }
};