#pragma once
#include "object.h"

class Floor : public Object
{
public:
    //****WARNING: YOU CANNOT COPY THIS CODE FOR YOUR ASSIGNMENT 1****
    //This is a partial demo, and it is ***INCORRECT***
    //PLEASE IMPLEMENT THE 'TRUE' RAY-PLANE INTERSECTION
    //ON SLIDE PAGE 21 IN 'RAY-SHADE.PDF' IN SYLLABUS
    virtual bool Intersect(Vector3 Origin, Vector3 Direction,
                           float * t_out, Vector3 * normal_out, Vector3 *col_out)
    {
        //My Test
        Vector3 A(0, 512, 0);
        Vector3 B(100, 256, 800);
        Vector3 C(512, 512, 0);
        Vector3 pt0(125, 350, 450);

        Vector3 BO = Minus(pt0, Origin);
        //float v = DotProduct(BO, Direction);
        Vector3 norm = CrossProduct(Minus(B, A), Minus(B, C));

        float t = -1;
        //if(DotProduct(norm, Direction) != 0) {
            t = DotProduct(norm, Minus(B, Origin)) / DotProduct(norm, Direction);
        //}
        //*t_out = t;

        //return true;

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
