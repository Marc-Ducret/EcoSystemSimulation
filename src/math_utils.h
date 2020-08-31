//
// Created by marc on 31/08/2020.
//

#ifndef ECOSYSTEM_MATH_UTILS_H
#define ECOSYSTEM_MATH_UTILS_H

typedef uint8_t u_char;
typedef uint16_t u_short;
typedef uint32_t u_int;
typedef uint64_t u_long;
typedef int8_t s_char;
typedef int16_t s_short;
typedef int32_t s_int;
typedef int64_t s_long;

template<typename T>
T sign(T val) {
    return (T(0) < val) - (val < T(0));
}

struct char2 {
    s_char x, y;

    char2 sign() const {
        return char2{
                .x = ::sign(x),
                .y = ::sign(y)
        };
    }

    char2 x2() const {
        return char2{
                .x = x,
                .y = 0
        };
    }

    char2 y2() const {
        return char2{
                .x = 0,
                .y = y
        };
    }
};

struct int2 {
    s_int x, y;

    int2 sign() const {
        return int2{
                .x = ::sign(x),
                .y = ::sign(y)
        };
    }

    int2 operator+(int2 that) const {
        return int2{
                .x = x + that.x,
                .y = y + that.y
        };
    }

    int2 operator+(char2 that) const {
        return int2{
                .x = x + that.x,
                .y = y + that.y
        };
    }
};

#endif //ECOSYSTEM_MATH_UTILS_H
