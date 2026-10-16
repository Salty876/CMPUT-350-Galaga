#include "GameEngine.h"
#include "GameContext.h"
#include "DrawContext.h"
#include "GraphicsObject.h"
#include "CollisionObject.h"

#include <SFML/System/Vector2.hpp>
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>

/// @brief
namespace CMPUT350 {
#include "FontData.h"

GameEngine::GameEngine(unsigned int width, unsigned int height, const std::string& name) {
    mFont = std::make_shared<sf::Font>();
    if (!mFont->openFromMemory(&_font, _font_len))
    {
        fprintf(stderr, "WARNING: Font did not load.\n");
    }

    mWindow = std::make_shared<sf::RenderWindow>(sf::VideoMode(sf::Vector2u(width, height)), name);
    mWindow->setFramerateLimit(30);
    mWindow->setKeyRepeatEnabled(false);
}

GameEngine::~GameEngine() {
    if (mWindow->isOpen()) mWindow->close();
}

void GameEngine::AddGameObject(std::shared_ptr<GameObject> gameObject) {
    mNewObjects.push_back(gameObject);
}

void GameEngine::InstallKeyDownNotification(int key, const std::string &notification) {
    mKeyDownNotifications[key].push_back(notification);
}

void GameEngine::InstallKeyUpNotification(int key, const std::string &notification) {
    mKeyUpNotifications[key].push_back(notification);
}

/**
 * @method Run
 * @arguments None
 * @description Gives control to the game engine. Will not return until the game window is closed or
 * all objects have been destroyed.
 */
void GameEngine::Run() {
    DrawContext drawContext = DrawContext(mWindow, mFont);
    DrawContext guiContext = DrawContext(mWindow, mFont);
    GameContext gameContext;

    gameContext.mEngineView = this;
    gameContext.ScreenContext = &drawContext;

    gameContext.GUIContext = &guiContext;
    gameContext.mNotificationManager = &mNotificationManager;
    gameContext.currentFrame = 0;

    while (true) {
        if (mWindow->isOpen() == false) break;

        // 0. Remove any objects that are now dead
        for (int i = mObjects.size() - 1; i >= 0; --i) {
            if (!mObjects[i]->IsAlive()) {
                // Delete using swap-and-pop
                mObjects[i] = std::move(mObjects.back());
                mObjects.pop_back();
            }
        }

        // 1. Activate and initialize any objects added during the last frame
        while (mNewObjects.size() != 0) {
            auto obj = mNewObjects.back();
            mNewObjects.pop_back();
            gameContext.CurrObject = obj;

            obj->Initialize(&gameContext);
            mObjects.push_back(obj);
        }

        // 2. Process events
        ProcessEvents(&gameContext);

        // 3. Update game objects
        for (int i = mObjects.size() - 1; i >= 0; --i) {
            gameContext.CurrObject = mObjects[i];
            mObjects[i]->Update(&gameContext);
        }

        // 4. Process collision events
        ProcessCollisions(&gameContext);

        // 5. Late updates
        for (int i = mObjects.size() - 1; i >= 0; --i) {
            gameContext.CurrObject = mObjects[i];
            mObjects[i]->LateUpdate(&gameContext);
        }

        // Clear window
        mWindow->clear(sf::Color::Black);

        // Collect all drawable objects
        std::vector<GraphicsObject*> graphicsObjects;
        for (int i = mObjects.size() - 1; i >= 0; --i) {
            // Use std::dynamic_cast to test pointer type
            GraphicsObject* obj = dynamic_cast<GraphicsObject*>(mObjects[i].get());

            if (obj != nullptr) graphicsObjects.push_back(obj);
        }

        // 6. Render background
        for (int i = graphicsObjects.size() - 1; i >= 0; --i) {
            drawContext.SetContextOffset(graphicsObjects[i]->GetLocation());
            drawContext.SetContextRotation(graphicsObjects[i]->GetRotation());
            graphicsObjects[i]->RenderBackground(&gameContext);
        }

        // 7. Render foreground
        for (int i = graphicsObjects.size() - 1; i >= 0; --i) {
            drawContext.SetContextOffset(graphicsObjects[i]->GetLocation());
            drawContext.SetContextRotation(graphicsObjects[i]->GetRotation());
            graphicsObjects[i]->RenderForeground(&gameContext);
        }

        // Render GUI
        for (int i = 0; i < mObjects.size(); ++i) {
            gameContext.CurrObject = mObjects[i];
            mObjects[i]->RenderUI(&gameContext);
        }

        // Actually render to window
        mWindow->display();
        gameContext.currentFrame++; // Append Frame
    }

    return; // Window is closed; terminate program
}

void GameEngine::ProcessEvents(GameContext *context) {
    while (const std::optional event = mWindow->pollEvent())
    {   
        if (event->is<sf::Event::Closed>()) {
            mWindow->close();
        }
        else if (event->is<sf::Event::Resized>()) {} // No action needed
        // Key Pressed
        else if (const auto* keyPressed = event->getIf<sf::Event::KeyPressed>()) { 
            // (note: Keyboard.h has scancodes as unsigned ints, so we need to cast them.)
            for (const auto &m: mKeyDownNotifications[static_cast<int>(keyPressed->scancode)])
                context->mNotificationManager->Notify(m);
        }
        // Key Released
        else if (const auto* keyPressed = event->getIf<sf::Event::KeyReleased>()) {
            for (const auto &m: mKeyUpNotifications[static_cast<int>(keyPressed->scancode)])
                context->mNotificationManager->Notify(m);
        }
    }
}

// Some collision helper functions
namespace {
    bool Overlap(Rect a, const Rect &b) {
        return (a &= b).width > 0; // Resultant Rect from a &= b being non-zero implies collision
    }

    bool CircleRect(const Circle &c, const Rect &r) {
        float x = c.center.x - std::clamp(c.center.x, r.topLeft.x, r.topLeft.x + r.width);
        float y = c.center.y - std::clamp(c.center.y, r.topLeft.y, r.topLeft.y + r.height);

        return (x * x) + (y * y) < (c.radius * c.radius);
    }

    // Returns first point of Line-Rect collision
    bool LineRect(const Line &l, const Rect &r, Point2D &out) {
        if (r.IsInside(l.p1)) {
            out = l.p1;
            return true; // Line starts inside Rectangle
        }

        Point2D topLeft = r.topLeft;
        Point2D topRight(topLeft.x + r.width, topLeft.y);
        Point2D bottomRight(topLeft.x + r.width, topLeft.y + r.height);
        Point2D bottomLeft(topLeft.x, topLeft.y + r.height);

        Line edges[4] = {
            Line(topLeft, topRight), 
            Line(topRight, bottomRight),
            Line(bottomRight, bottomLeft), 
            Line(bottomLeft, topLeft)};

        bool collided = false;
        float shortestDistance = FLT_MAX;

        for (const Line &edge : edges) {
            Point2D crossing;

            if (!l.Crosses(edge, crossing)) continue;

            float distance = l.p1.Distance(crossing);

            if (distance < shortestDistance) { 
                // Keep the crossing closest to the start of the line
                shortestDistance = distance;
                out = crossing;
                collided = true;
            }
        }

        return collided;
    }

    // Returns first point of Line-Circle collision
    bool LineCircle(const Line &l, const Circle &c, Point2D &out) {
        if (l.p1.Distance(c.center) <= c.radius) {
            out = l.p1;
            return true; // Line starts inside Circle
        }

        Point2D lineVector = l.p2 - l.p1;
        float lengthSquared = lineVector.Dot(lineVector);

        if (lengthSquared < 1e-6f) return false; // Line is a single point outside the Circle

        // Projection is the closest point to the circle center on the infinite line
        float projection = (c.center - l.p1).Dot(lineVector) / lengthSquared;
        Point2D closestPoint = l.p1 + lineVector * projection;

        float centerToLine = closestPoint.Distance(c.center);

        if (centerToLine >= c.radius) return false;

        float hitOffset = std::sqrt((c.radius * c.radius) - (centerToLine * centerToLine));

        Point2D toStart = l.p1 - closestPoint; // a - p
        toStart.Normalize();
        Point2D hitPoint = closestPoint + toStart * hitOffset; // P

        float hitProjection = (hitPoint - l.p1).Dot(lineVector);

        if (hitProjection < 0.0f || hitProjection > lengthSquared) return false;

        out = hitPoint;
        return true;
    }

    bool TestShapes(const Shape &first, const Shape &second, Point2D &collisionPoint) {
        bool firstIsLine = first.t == ShapeType::kLine;
        bool secondIsLine = second.t == ShapeType::kLine;

        if (firstIsLine || secondIsLine) {
            const Shape &lineShape = firstIsLine ? first : second;
            const Shape &otherShape = firstIsLine ? second : first;

            if (otherShape.t == ShapeType::kLine) {
                return lineShape.shape.line.Crosses(otherShape.shape.line, collisionPoint);
            }
            if (otherShape.t == ShapeType::kCircle) {
                return LineCircle(lineShape.shape.line, otherShape.shape.circle, collisionPoint);
            }
            return LineRect(lineShape.shape.line, otherShape.shape.rect, collisionPoint);
        }

        if (first.t == ShapeType::kRect && second.t == ShapeType::kRect) {
            return Overlap(first.shape.rect, second.shape.rect);
        }

        if (first.t == ShapeType::kCircle && second.t == ShapeType::kCircle) {
            float radiiSum = first.shape.circle.radius + second.shape.circle.radius;
            return first.shape.circle.center.Distance(second.shape.circle.center) < radiiSum;
        }

        if (first.t == ShapeType::kCircle) return CircleRect(first.shape.circle, second.shape.rect);
        return CircleRect(second.shape.circle, first.shape.rect);
    }

    bool ShapesCollide(const std::vector<Shape> &shapesA, const std::vector<Shape> &shapesB,
                       Point2D &firstPoint, bool &hasPoint) {
        bool collided = false;
        float shortestDistance = FLT_MAX;
        hasPoint = false;

        for (const Shape &shapeA : shapesA) {
            for (const Shape &shapeB : shapesB) {
                Point2D point;

                if (!TestShapes(shapeA, shapeB, point)) continue;

                collided = true;

                if (shapeA.t != ShapeType::kLine && shapeB.t != ShapeType::kLine) continue;

                const Line &line = (shapeA.t == ShapeType::kLine) ? shapeA.shape.line : shapeB.shape.line;
                float distance = line.p1.Distance(point);

                if (distance < shortestDistance) {
                    shortestDistance = distance;
                    firstPoint = point;
                    hasPoint = true;
                }
            }
        }

        return collided;
    }
}


void GameEngine::ProcessCollisions(GameContext *context) {
    DrawContext *drawContext = context->ScreenContext;

    // Moves the draw context onto an object so that
    // Transform() and ReverseTransform() can convert 
    // between its local space and the screen
    auto setTransform = [&](CollisionObject &obj) {
        drawContext->SetContextOffset(obj.GetLocation());
        drawContext->SetContextRotation(obj.GetRotation());
    };

    // Returns an object's collision shapes in screen coordinates
    auto getWorldShapes = [&](CollisionObject &obj) {
        setTransform(obj);
        std::vector<Shape> worldShapes;

        for (const Shape &shape : obj.GetShapes()) {
            switch (shape.t) {
                case ShapeType::kLine:
                    worldShapes.emplace_back(Line(drawContext->Transform(shape.shape.line.p1), drawContext->Transform(shape.shape.line.p2)));
                    break;
                case ShapeType::kCircle:
                    worldShapes.emplace_back(Circle(drawContext->Transform(shape.shape.circle.center), shape.shape.circle.radius));
                    break;
                case ShapeType::kRect:
                    worldShapes.emplace_back(drawContext->Transform(shape.shape.rect));
                    break;
            }
        }

        return worldShapes;
    };

    std::vector<std::shared_ptr<CollisionObject>> collisionObjects;
    for (int i = mObjects.size() - 1; i >= 0; --i) {
        // Use std::dynamic_pointer_cast to test pointer type
        std::shared_ptr<CollisionObject> obj = std::dynamic_pointer_cast<CollisionObject>(mObjects[i]);

        if (obj != nullptr && obj->IsAlive()) collisionObjects.push_back(obj);
    }

    for (int i = 0; i < collisionObjects.size(); ++i) {
        // Note: the inner loop starts from i + 1 as those collisions
        // are already tested during the previous iterations.
        for (int j = i + 1; j < collisionObjects.size(); ++j) {
            // Bounds are in local space, so we need to move them to screen space first
            setTransform(*collisionObjects[i]);
            Rect iBounds = drawContext->Transform(collisionObjects[i]->GetBounds());
            setTransform(*collisionObjects[j]);
            Rect jBounds = drawContext->Transform(collisionObjects[j]->GetBounds());

            // AABB Collision Detection, then check the individual shapes
            bool condition = Overlap(iBounds, jBounds);
            Point2D collisionPoint;
            bool hasPoint = false;

            if (condition && ShapesCollide(getWorldShapes(*collisionObjects[i]), getWorldShapes(*collisionObjects[j]), collisionPoint, hasPoint)) {
                // Alert both objects
                if (hasPoint) {
                    // A line was involved: give each object the point in its own local space
                    setTransform(*collisionObjects[i]);
                    Point2D iPoint = drawContext->ReverseTransform(collisionPoint);
                    setTransform(*collisionObjects[j]);
                    Point2D jPoint = drawContext->ReverseTransform(collisionPoint);

                    collisionObjects[i]->CollisionEnter(collisionObjects[j], iPoint);
                    collisionObjects[j]->CollisionEnter(collisionObjects[i], jPoint);
                } else {
                    collisionObjects[i]->CollisionEnter(collisionObjects[j]);
                    collisionObjects[j]->CollisionEnter(collisionObjects[i]);
                }
            }
        }
    }
    return;
}

}  // namespace CMPUT350
