#pragma once
#include "object.h"

class Floor : public Object
{
public:
    Floor(Vector3 pt_a, Vector3 pt_b, Vector3 pt_c, Vector3 color_in, Vector3 point_in) {
      A = pt_a;
      B = pt_b;
      C = pt_c;
      color = color_in;
      pt0 = point_in;
    }

    virtual bool Intersect(Vector3 Origin, Vector3 Direction,
                           float * t_out, Vector3 * normal_out, Vector3 *col_out)
    {
        //Vector3 BO = Minus(pt0, Origin);
        Vector3 norm = CrossProduct(Minus(B, A), Minus(B, C));

        float t = -1;
        t = DotProduct(norm, Minus(B, Origin)) / DotProduct(norm, Direction);

        Vector3 pt = Add(Origin, MultiplyScalar(Direction, t));

        //Here
        if(t > 0) {
            float bapa = DotProduct(CrossProduct(Minus(B, A), Minus(pt, A)), norm);
            float cbpb = DotProduct(CrossProduct(Minus(C, B), Minus(pt, B)), norm);
            float acpc = DotProduct(CrossProduct(Minus(A, C), Minus(pt, C)), norm);

            if(sameSign(bapa, cbpb)) {
              if(sameSign(cbpb, acpc)) {
                //All sameSign
                *t_out = t;
                Vector3 IntersectionPoint = MultiplyScalar(Direction, t);
                IntersectionPoint = Add(IntersectionPoint, Origin);
                Vector3 SurfaceNormal = Minus(IntersectionPoint, pt0);
                (*normal_out) = Normalize(SurfaceNormal);
                *col_out = color;
                return true;
              } else {
                return false;
              }
            }
        }//End of if(t > 0)



        /* Original Code
        if (Direction.y > 0)
        {
            (*t) = Origin.y / Direction.y;
            normal->x = 0;
            normal->y = -1;
            normal->z = 0;
            return true;
        }
        return false;
        */
    }//End of virtual bool Intersect
    Vector3 A;
    Vector3 B;
    Vector3 C;
    Vector3 color;
    Vector3 pt0;
};//End of class Floor


/*
bool intersectPlane(const Vec3f &n, const Vec3f &p0,
                    const Vec3f &l0, const Vec3f &l, float &t)
{
    // assuming vectors are all normalized
    float denom = dotProduct(n, l);
    if (denom > 1e-6) {
        Vec3f p0l0 = p0 - l0;
        t = dotProduct(p0l0, n) / denom;
        return (t >= 0);
    }

    return false;
}
*/
