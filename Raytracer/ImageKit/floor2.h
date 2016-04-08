#pragma once
#include "object.h"

class Floor2 : public Object
{
public:
    Floor2(Floor f1_in, Floor f2_in) {
      f1 = f1_in;
      f2 = f2_in;
      color = color_in;
      pt0 = point_in;
    }

    virtual bool Intersect(Vector3 Origin, Vector3 Direction,
                           float * t_out, Vector3 * normal_out, Vector3 *col_out)
    {
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

    }//End of virtual bool Intersect
    Floor f1;
    Floor f2;
    Vector3 color;
    Vector3 pt0;
};//End of class Floor2
