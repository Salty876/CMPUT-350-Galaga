#include <vector>
#include "bezier.h"
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
        float mem2y = 3 * (1.f - t) * (1.f - t) * t * Bezier::pts[1].y;
        float mem3y = 3 * (1.f - t) * t * t * Bezier::pts[2].y;
        float mem4y = t * t * t * Bezier::pts[3].y;
        float By = mem1y + mem2y + mem3y + mem4y;

        return Point2D(Bx, By);


    }
}