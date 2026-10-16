#ifndef ENEMY_H
#define ENEMY_H

#include "CollisionObject.h"
#include "GameContext.h"
#include "bezier.h"

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
    bool HandleKeyEvent(CMPUT350::GameContext* context, char key) override;
    bool IsAlive() const override;
    void Kill() override;

    // Graphics Object Functions
    void RenderBackground(CMPUT350::GameContext* context) override;
    void RenderForeground(CMPUT350::GameContext* context) override;


    // Collision Object Functions
    void CollisionEnter(const std::shared_ptr<CMPUT350::CollisionObject>& obj) override;
    const CMPUT350::Rect& GetBounds() override;

    bool mIsAlive;
    CMPUT350::Point2D mLocation;
    CMPUT350::Rect mBounds;
    CMPUT350::Point2D mFormationLocation;
    float mRotation;
    CMPUT350::Bezier mInitialPath;
    CMPUT350::Bezier mAttackPath;
    std::vector<CMPUT350::Shape> mShapes;
    float mPathProgress;
    float mFormationDirection;
    int mHealth;
    state mState;

};


class Enemy1: public Enemy
{
public:
    Enemy1(CMPUT350::Point2D loc, const std::vector<CMPUT350::Point2D> &path)
	    :Enemy(loc, path){}
	void Initialize(CMPUT350::GameContext *context) override;
	void LateUpdate(CMPUT350::GameContext *context) override;
	bool Attack(CMPUT350::GameContext *context, const CMPUT350::Point2D &target) override;
	void RenderForeground(CMPUT350::GameContext *context) override;

};

class Enemy2: public Enemy
{
public:
    Enemy2(CMPUT350::Point2D loc, const std::vector<CMPUT350::Point2D> &path)
	    :Enemy(loc, path){}
	void Initialize(CMPUT350::GameContext *context) override;
	void LateUpdate(CMPUT350::GameContext *context) override;
	bool Attack(CMPUT350::GameContext *context, const CMPUT350::Point2D &target) override;
	void RenderForeground(CMPUT350::GameContext *context) override;

};


#endif

