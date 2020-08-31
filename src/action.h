//
// Created by marc on 31/08/2020.
//

#ifndef ECOSYSTEM_ACTION_H
#define ECOSYSTEM_ACTION_H

#include "math_utils.h"

namespace action {
    typedef unsigned char action_kind;

    const action_kind MOVE = 0;

    struct action {
        action_kind kind;
        int2 from, to;
    };
}

#endif //ECOSYSTEM_ACTION_H
