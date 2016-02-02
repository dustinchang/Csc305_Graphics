#pragma once
#include "object.h"

class Plane : public Object
{
public:
    Plane(Vector3 point_in, Vector3 norms_in, Vector3 color_in) {
        point = point_in;
        norms = norms_in;
        color = color_in;

    }

    virtual bool Intersect(Vector3 Origin, Vector3 Direction,
                           float * t_out, Vector3 * normal_out, Vector3 *col_out)
    {
        //Formula for acquire t of a plane
        float t = DotProduct(norms, Minus(point, Origin)) / DotProduct(norms, Direction);

        if(t > 0.0001) {
            *t_out = t;
            *normal_out = norms;
            *col_out = color;
            return true;
        } else {
            return false;
        }

    }//End of Intersect

    Vector3 point;
    Vector3 norms;
    Vector3 color;
};
