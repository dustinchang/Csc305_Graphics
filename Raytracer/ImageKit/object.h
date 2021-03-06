#pragma once
#include "common.h"

class Object
{
public:
    virtual bool Intersect(Vector3 Origin, Vector3 Direction,
                           float * t, Vector3 * normal, Vector3 *col_out) = 0;
};
