#ifndef ENEMY_H
#define ENEMY_H

#include <vector>
#include "CollisionObject.h"
#include "GameContext.h"
#include "Bezier.h"

enum state {
    ENTRY,
    FORMATION,
    ATTACK,
    RETURNING
};

class Enemy : public CMPUT350::CollisionObject
{
public:
    Enemy(CMPUT350::Point2D loc, const std::vector<CMPUT350::Point2D> &path);

    // GameObject Functions
    void Initialize(CMPUT350::GameContext* context) override;
    void Update(CMPUT350::GameContext* context) override;
    void LateUpdate(CMPUT350::GameContext* context) override;
    bool IsAlive() const override;
    void Kill() override;

    // Graphics Object Functions
    void RenderBackground(CMPUT350::GameContext* context) override;
    void RenderForeground(CMPUT350::GameContext* context) override;
    CMPUT350::Point2D GetLocation() const override;
    float GetRotation() const override;

    // Collision Object Functions
    using CMPUT350::CollisionObject::CollisionEnter;
    void CollisionEnter(const std::shared_ptr<CMPUT350::CollisionObject>& obj) override;
    const CMPUT350::Rect& GetBounds() override;
    const std::vector<CMPUT350::Shape>& GetShapes() override;

    // Enemy Functions
    // Returns true if the enemy started an attack, false if it could not
    virtual bool Attack(CMPUT350::GameContext *context, const CMPUT350::Point2D &target) = 0;
    bool InFormation() const;
    int GetPoints() const;

    bool mIsAlive;
    CMPUT350::Point2D mLocation;
    CMPUT350::Rect mBounds;
    CMPUT350::Point2D mFormationLocation;
    float mRotation;
    CMPUT350::Bezier mInitialPath;
    CMPUT350::Bezier mAttackPath;
    std::vector<CMPUT350::Shape> mShapes;
    float mPathProgress;
    float mEntrySegments;   // Number of cubic segments in the entry path
    float mSpeed;           // Pixels per frame along a path
    int mHealth;
    int mPoints;            // Points when destroyed in formation (doubled otherwise)
    state mState;
    CMPUT350::NotificationManager *mNotifier;

private:
    // Moves along a path at a constant speed. Returns true when the end is reached.
    bool FollowPath(const CMPUT350::Bezier &path, float segments);
};


class Enemy1: public Enemy
{
public:
    Enemy1(CMPUT350::Point2D loc, const std::vector<CMPUT350::Point2D> &path);
    bool Attack(CMPUT350::GameContext *context, const CMPUT350::Point2D &target) override;
};

class Enemy2: public Enemy
{
public:
    Enemy2(CMPUT350::Point2D loc, const std::vector<CMPUT350::Point2D> &path);
    bool Attack(CMPUT350::GameContext *context, const CMPUT350::Point2D &target) override;
    void RenderForeground(CMPUT350::GameContext *context) override;
};

class Enemy3: public Enemy
{
public:
    Enemy3(CMPUT350::Point2D loc, const std::vector<CMPUT350::Point2D> &path);
    bool Attack(CMPUT350::GameContext *context, const CMPUT350::Point2D &target) override;
    void RenderForeground(CMPUT350::GameContext *context) override;
};


#endif

