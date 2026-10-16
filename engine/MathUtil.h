#ifndef MATHUTIL_H
#define MATHUTIL_H

#include <cmath>
#include <iostream>

namespace CMPUT350 {

struct Point2D {
    float x, y;
    Point2D(float x = 0, float y = 0) : x(x), y(y) {}
    double Distance(const Point2D &other) const {
        double deltaX = other.x - x;
        double deltaY = other.y - y;
        double distance = std::sqrt((deltaX * deltaX) + (deltaY * deltaY));
        return distance;
    }
    Point2D operator+(const Point2D &other) const {
        return Point2D(x + other.x, y + other.y);
    }
    Point2D operator+(const float &other) const {
        return Point2D(x + other, y + other);
    }
    Point2D operator-(const Point2D &other) const {
        return Point2D(x - other.x, y - other.y);;
    }
    Point2D operator-(const float &other) const {
        return Point2D(x - other, y - other);
    }
    Point2D operator*(const float &scalar) const {
        return Point2D(x * scalar, y * scalar);
    }
    Point2D &operator+=(const float &scalar) {
        x += scalar;
        y += scalar;
        return *this;
    }
    Point2D &operator+=(const Point2D &other) {
        x += other.x;
        y += other.y;
        return *this;
    }
    Point2D &operator-=(const Point2D &other) {
        x -= other.x;
        y -= other.y;
        return *this;
    }
    bool operator==(const Point2D &other) const {
        return (x == other.x && y == other.y);
    }
    Point2D &operator*=(const float &scalar) {
        x *= scalar;
        y *= scalar;
        return *this;
    }
    Point2D &operator/=(const int &scalar) {
        x /= scalar;
        y /= scalar;
        return *this;
    }
    float operator*(const Point2D &other) const {
        return (x * other.x) + (y * other.y);
    }
    float Dot(Point2D b) const {
        return (x * b.x) + (y * b.y);
    }
    static float Dot(Point2D a, Point2D b) {
        return (a.x * b.x) + (a.y * b.y);
    }
    static float Cross(Point2D a, Point2D b) {
        return (a.x * b.y) - (a.y * b.x);
    }
    void Normalize() {
        float magnitude = std::sqrt((x * x) + (y * y));
        x = x / magnitude;
        y = y / magnitude;
    }
};

static std::ostream &operator<<(std::ostream &os, const Point2D &p) {
    os << "(" << p.x << ", " << p.y << ")";
    return os;
}

static Point2D operator*(float number, const Point2D &rhs) {
    return Point2D(rhs.x * number, rhs.y * number);
}

struct Line {
    Point2D p1, p2;

    Line(Point2D p1 = {0, 0}, Point2D p2 = {0, 0}) : p1(p1), p2(p2) {}
    Line(float x1, float y1, float x2, float y2) : p1(x1, y1), p2(x2, y2) {}
    float Length() const {
        return p1.Distance(p2);
    }
    Point2D ClosestPoint(const Point2D &p) const {
        // (This may be wrong but here im assuming we want the point closest to p (any pooint in line))
        
        Point2D ab = p2 - p1;
        Point2D ap = p - p1;

        float t = (ab.Dot(ap)) / (ab.Dot(ab));
        
        //clamp t 
        
        if (t < 0.0f) {
          t = 0.0f;
        }

        else if (t > 1.0f) {
          t = 1.0f;
        }

        Point2D val = p1 + ab * t;

        
        return val;
    }
    bool Crosses(Line other, Point2D &crossingPoint) const {
        Point2D P = p2 - p1;
        Point2D Q = other.p2 - other.p1;
        
        float cp = Point2D::Cross(P, Q);
    
        if (cp == 0.0f){
          // Vectors are parralel so cant cross
          return false;
        }

        float t = (Point2D::Cross((other.p1 - p1), Q) / cp);

        float u = (Point2D::Cross((other.p1 - p1), P) / cp);

        if ((t < 0.0f) || (t > 1.0f) || (u < 0.0f) || (u < 1.0f)) {
          // Infinite lines intersect but not the lines we have
          return false;
        }

        Point2D intersect = p1 + (p2 - p1) * t;

        return (intersect == crossingPoint);
    }
};

static std::ostream &operator<<(std::ostream &os, const Line &l) {  
    os << "Line from " << l.p1 << " to " << l.p2;
    return os;
}

struct Circle {
    Point2D center;
    float radius;
    Circle(Point2D c = {0, 0}, float r = 0) : center(c), radius(r) {}
    Circle(float x, float y, float r) : center(x, y), radius(r) {}
};

static std::ostream &operator<<(std::ostream os, const Circle &c) {
    os << "Center: " << c.center << " and Radius: " << c.radius;
    return os;
}

struct Rect {
    Point2D topLeft;
    float width, height;

    Rect(float left, float top, float width, float height)
        : topLeft(Point2D(left, top)), width(width), height(height) {}

    Rect(Point2D tl = {0, 0}, int w = 0, int h = 0) : topLeft(tl), width(w), height(h) {}

    // Creates bounding box around p1 and p2 with positive width/height
    Rect(Point2D p1, Point2D p2)
        : topLeft(std::min(p1.x, p2.x), std::min(p1.y, p2.y)),
          width(fabs(p1.x - p2.x)),
          height(fabs(p1.y - p2.y)) {}

    Rect(Point2D center, float radius)
        : topLeft(center.x - radius, center.y - radius), width(2 * radius), height(2 * radius) {}

    Rect &operator|=(const Rect &other) {
        // check width
        Point2D currBotRight(topLeft.x + width, topLeft.y + height);
        Point2D otherBotRitgh(other.topLeft.x + other.width, other.topLeft.y + other.height);
        topLeft.x = std::min(topLeft.x, other.topLeft.x);
        topLeft.x = std::min(topLeft.y, other.topLeft.y);

        Point2D maxBotRight;

        maxBotRight.x = std::max(currBotRight.x, otherBotRitgh.x);
        maxBotRight.y = std::max(currBotRight.y, otherBotRitgh.y);

        width = maxBotRight.x - topLeft.x;
        height = maxBotRight.y = topLeft.y;

        return *this;
    }

    Rect &operator|=(const Point2D &other) {  
        Point2D currBotRight(topLeft.x + width, topLeft.y + height);
        topLeft.x = std::min(topLeft.x, other.x);
        topLeft.x = std::min(topLeft.y, other.y);

        Point2D maxBotRight;

        maxBotRight.x = std::max(currBotRight.x, other.x);
        maxBotRight.y = std::max(currBotRight.y, other.y);

        width = maxBotRight.x - topLeft.x;
        height = maxBotRight.y = topLeft.y;

        return *this;
    }
    Rect &operator|=(const Line &other) { 
        *this |= other.p1;
        *this |= other.p2;

        return *this;
    }
    Rect &operator&=(const Rect &other) {
        Point2D currBotRight(topLeft.x + width, topLeft.y + height);
        Point2D otherBotRitgh(other.topLeft.x + other.width, other.topLeft.y + other.height);

        float newTopLeftX = std::max(topLeft.x, other.topLeft.x);
        float newTopLeftY = std::max(topLeft.y, other.topLeft.y);

        Point2D minBotRight;

        minBotRight.x = std::min(currBotRight.x, otherBotRitgh.x);
        minBotRight.y = std::min(currBotRight.y, otherBotRitgh.y);

        // overlap check
        if (minBotRight.x > newTopLeftX && minBotRight.y > newTopLeftY){
            topLeft.x = newTopLeftX;
            topLeft.y = newTopLeftY;
            width = minBotRight.x - topLeft.x;
            height = minBotRight.y - topLeft.y;
        } else {
            topLeft.x = 0;
            topLeft.y = 0;
            width = 0;
            height = 0;
        }



        return *this;
    }
    Rect &operator+=(const Point2D &other) {
        *this |= other;
        return *this;
    }
    Rect operator+(const Point2D &other) const {
        Rect retVal = *this;
        retVal += other;
        return retVal;
    }
    void Inset(int inset) {
        topLeft += static_cast<float>(inset);
        width -= (2.f * static_cast<float>(inset));
        height -= (2.f * static_cast<float>(inset));
    }
    bool IsInside(const Point2D &p) const {
        Point2D rightBot(topLeft.x + width, topLeft.y + height);
        if (topLeft.x < p.x && p.x < rightBot.x && topLeft.y < p.y && p.y < rightBot.y) {
            return true;
        }
        return false;
    }
};

static std::ostream &operator<<(std::ostream &os, const Rect &l) {
    os << "Top Left: " << l.topLeft << " and Width: " << l.width << " and Height: " << l.height; 
    return os;
}

}  // namespace CMPUT350

#endif  // MATHUTIL_H
