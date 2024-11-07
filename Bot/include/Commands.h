#pragma once

#include <string_view>
#include <array>

struct CommandDescription
{
    constexpr CommandDescription(std::string_view name, std::string_view description)
        : name(name), description(description) {}

    std::string_view name;
    std::string_view description;
};

inline std::vector<CommandDescription> COMMANDS = {
    CommandDescription{"ping", "Ping the bot"},
    CommandDescription{"ping", "Ping the bot"},
};