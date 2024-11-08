#pragma once

#include <concepts>
#include <string>

namespace std
{

inline string to_string(const string& str)
{
    return str;
}

} // namespace std

template<typename T>
concept ConvertibleToString = requires(T t) {
    { std::to_string(t) } -> std::convertible_to<std::string>;
};

template<typename T>
concept Comparable = requires(T a, T b) {
    { a == b } -> std::convertible_to<bool>;
    { a < b } -> std::convertible_to<bool>;
};