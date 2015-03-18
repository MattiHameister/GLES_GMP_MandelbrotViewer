#pragma once

#include <iostream>
#include <stdint.h>
#include <limits>

#define FRAMES_PER_SECOND 30

using iter_t = int32_t;//uint16_t;
#define INT_MAX_VALUE std::numeric_limits<iter_t>::max()

#define IS_INSIDE (INT_MAX_VALUE - 1)
#define IS_BLANK (INT_MAX_VALUE)


