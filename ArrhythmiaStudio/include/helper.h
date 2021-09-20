#pragma once

#include <sstream>

#define H_STR(x) #x
#define STRINGIFY(x) H_STR(x)

namespace std
{
    template <typename T>
    std::string to_string_with_precision(const T a_value, const int n = 6)
    {
        std::ostringstream out;
        out.precision(n);
        out << std::fixed << a_value;
        return out.str();
    }
}