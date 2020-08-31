//
// Created by marc on 31/08/2020.
//

#ifndef ECOSYSTEM_TILE_H
#define ECOSYSTEM_TILE_H

#include "math_utils.h"

namespace tile {
    enum Kind : u_char {
        EMPTY = 0,
        WATER = 1,
        PLANT = 2,
        PREY = 3,
        PREDATOR = 4,
        VOID = 0xFF,
    };

    unsigned int Color(Kind kind) {
        switch (kind) {
            case EMPTY:
                return 0x204020;
            case WATER:
                return 0x2020A0;
            case PLANT:
                return 0x20A020;
            case PREY:
                return 0xA04080;
            case PREDATOR:
                return 0xA02020;
            case VOID:
                return 0x808080;
            default:
                return 0xFF00FF;
        }
    }

    struct tile {
        Kind type;
        u_short age;
        u_char think_offset;
        char2 target;
    };
}

#endif //ECOSYSTEM_TILE_H
