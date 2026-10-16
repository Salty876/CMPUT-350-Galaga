#include "DrawContext.h"
#include "MathUtil.h"
#include <SFML/Graphics.hpp>

namespace CMPUT350 {

void SetContextOffset(Point2D p) {

}

void SetContextRotation(float rotation) {

}

Point2D Transform(Point2D p) {

}

Point2D ReverseTransform(Point2D p) {

}

Rect Transform(Rect r) {

}

DrawContext::DrawContext(std::shared_ptr<sf::RenderWindow> window, std::shared_ptr<sf::Font> font)
    : mWindow(window), mFont(font) {}

void DrawContext::DrawCenteredText(const std::string &text, int pixelSize, Point2D p, RGBColor c) {
    sf::Text sfText(*mFont, text, pixelSize);

    // Center(x) = (left_bound(x) + bound_size(x)) / 2
    // Center(y) = (left_bound(y) + bound_size(y)) / 2
    sfText.setOrigin(sf::Vector2f(
        sfText.getLocalBounds().position.x + sfText.getLocalBounds().size.x / 2.0f, 
        sfText.getLocalBounds().position.y + sfText.getLocalBounds().size.y / 2.0f));

    sfText.setPosition(sf::Vector2f(p.x, p.y));
    sfText.setFillColor(sf::Color(c.r, c.g, c.b));
    mWindow->draw(sfText);
}

void DrawContext::DrawText(const std::string &text, int pixelSize, Point2D p, RGBColor c) {
    sf::Text sfText(*mFont, text, pixelSize);
    sfText.setPosition(sf::Vector2f(p.x, p.y));
    sfText.setFillColor(sf::Color(c.r, c.g, c.b));
    mWindow->draw(sfText);
}

void DrawContext::DrawCircle(Point2D p, float radius, RGBColor c) {
    sf::CircleShape sfCircle(radius);

    // Default position is top-left
    // Center(x) = x_value - radius
    // Center(y) = y_value - radius
    sfCircle.setPosition(sf::Vector2f(p.x - radius, p.y - radius));
    sfCircle.setFillColor(sf::Color(c.r, c.g, c.b));
    mWindow->draw(sfCircle);
}

void DrawContext::DrawRect(Rect r, RGBColor c) {
    sf::RectangleShape sfRectangle(sf::Vector2f(r.width, r.height));
    sfRectangle.setPosition(sf::Vector2f(r.topLeft.x, r.topLeft.y));
    sfRectangle.setFillColor(sf::Color(c.r, c.g, c.b));
    mWindow->draw(sfRectangle);
}

void DrawContext::FrameRect(Rect r, float width, RGBColor c) {
    sf::RectangleShape sfRectangle(sf::Vector2f(r.width, r.height));
    sfRectangle.setOutlineColor(sf::Color(c.r, c.g, c.b));
    sfRectangle.setOutlineThickness(-1 * width); // Prevent rectangle frame from exceeding it's bounds
    sfRectangle.setPosition(sf::Vector2f(r.topLeft.x, r.topLeft.y));
    sfRectangle.setFillColor(sf::Color::Transparent);
    mWindow->draw(sfRectangle);
}

/**
 * @brief Draws a line between two points with a specified width and color.
 *
 * @param from The starting point of the line (Point2D).
 * @param to The ending point of the line (Point2D).
 * @param width The width of the line in pixels.
 * @param c The color of the line, specified as an RGBColor object.
 *
 * This function calculates the distance and angle between the two points
 * and uses a polygone shape to represent the line. The line is drawn
 * relative to the world offset and rendered onto the associated window.
 */
void DrawContext::DrawLine(Point2D from, Point2D to, float width, RGBColor c) {
    if (to == from) return;

    // Using derivation covered in class:
    // Direction = to - from
    Point2D direction = to - from;

    // Perpendicular to Direction
    Point2D end(-direction.y, direction.x);

    end.Normalize(); // Create unit vector parallel to the perpendicular line
    end = end * (width / 2.f); // Width covers both perpendiculars from the center

    Point2D points[4] = {from + end, to + end, to - end, from - end};
    
    sf::ConvexShape sfShape(4); // Generate 4-sided SFML polygon
    for (int i = 0; i < 4; ++i) sfShape.setPoint(i, sf::Vector2f(points[i].x, points[i].y));

    sfShape.setFillColor(sf::Color(c.r, c.g, c.b));
    mWindow->draw(sfShape);
}

int DrawContext::GetWindowWidth() { return mWindow->getSize().x; }

int DrawContext::GetWindowHeight() { return mWindow->getSize().y; }

}  // namespace CMPUT350
