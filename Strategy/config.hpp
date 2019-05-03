#ifndef CONFIG_HPP
#define CONFIG_HPP

#include "point.hpp"
#include <cstdint>

struct config {
    static uint32_t row;
    static uint32_t column;
    static Point nopos;
};

#endif
