#include <vector>
#include "Bezier.h"
#include "MathUtil.h"

namespace CMPUT350 {

    

    Bezier::Bezier(const std::vector<Point2D> &pts) : pts(pts) {}

    Point2D Bezier::GetPoint(float t) const {
        float mem1x = (1.0f - t) * (1.0f - t) * (1.0f - t) * Bezier::pts[0].x;
        float mem2x = 3 * (1.f - t) * (1.f - t) * t * Bezier::pts[1].x;
        float mem3x = 3 * (1.f - t) * t * t * Bezier::pts[2].x;
        float mem4x = t * t * t * Bezier::pts[3].x;
        float Bx = mem1x + mem2x + mem3x + mem4x;

        float mem1y = (1.0f - t) * (1.0f - t) * (1.0f - t) * Bezier::pts[0].y;
        float mem2y = 3.f * (1.f - t) * (1.f - t) * t * Bezier::pts[1].y;
        float mem3y = 3.f * (1.f - t) * t * t * Bezier::pts[2].y;
        float mem4y = t * t * t * Bezier::pts[3].y;
        float By = mem1y + mem2y + mem3y + mem4y;

        return Point2D(Bx, By);
    }

    Point2D Bezier::GetSlope(float t) const {
        float mem1x = 3.f * (1.f - t) * (1.f - t) * (Bezier::pts[1].x - Bezier::pts[0].x);
        float mem2x = 6.f * (1.f - t) * t * (Bezier::pts[2].x - Bezier::pts[1].x);
        float mem3x = 3.f * t * t * (Bezier::pts[3].x - Bezier::pts[2].x);

        float dBx = mem1x + mem2x + mem3x;

        float mem1y = 3.f * (1.f - t) * (1.f - t) * (Bezier::pts[1].y - Bezier::pts[0].y);
        float mem2y = 6.f * (1.f - t) * t * (Bezier::pts[2].y - Bezier::pts[1].y);
        float mem3y = 3.f * t * t * (Bezier::pts[3].y - Bezier::pts[2].y);

        float dBy = mem1y + mem2y + mem3y;
        
        return Point2D(dBx, dBy);
    }




    static Point2D GetPoint(const std::vector<Point2D> &pts, float t) {
        float mem1x = (1.0f - t) * (1.0f - t) * (1.0f - t) * pts[0].x;
        float mem2x = 3 * (1.f - t) * (1.f - t) * t * pts[1].x;
        float mem3x = 3 * (1.f - t) * t * t * pts[2].x;
        float mem4x = t * t * t * pts[3].x;
        float Bx = mem1x + mem2x + mem3x + mem4x;

        float mem1y = (1.0f - t) * (1.0f - t) * (1.0f - t) * pts[0].y;
        float mem2y = 3.f * (1.f - t) * (1.f - t) * t * pts[1].y;
        float mem3y = 3.f * (1.f - t) * t * t * pts[2].y;
        float mem4y = t * t * t * pts[3].y;
        float By = mem1y + mem2y + mem3y + mem4y;

        return Point2D(Bx, By);
    }

    static Point2D GetSlope(const std::vector<Point2D> &pts, float t) {
        float mem1x = 3.f * (1.f - t) * (1.f - t) * (pts[1].x - pts[0].x);
        float mem2x = 6.f * (1.f - t) * t * (pts[2].x - pts[1].x);
        float mem3x = 3.f * t * t * (pts[3].x - pts[2].x);

        float dBx = mem1x + mem2x + mem3x;

        float mem1y = 3.f * (1.f - t) * (1.f - t) * (pts[1].y - pts[0].y);
        float mem2y = 6.f * (1.f - t) * t * (pts[2].y - pts[1].y);
        float mem3y = 3.f * t * t * (pts[3].y - pts[2].y);

        float dBy = mem1y + mem2y + mem3y;
        
        return Point2D(dBx, dBy);
    }

}
