#ifndef BULLET_H
#define BULLET_H

#include "CollisionObject.h"
#include "GameContext.h"

class Bullet : public CMPUT350::CollisionObject
{
public:
	Bullet(CMPUT350::Point2D location, CMPUT350::Point2D heading, bool player, std::weak_ptr<CMPUT350::GameObject> parent);
	bool IsPlayerBullet();
	void Update(CMPUT350::GameContext *context);
	void LateUpdate(CMPUT350::GameContext *context);
	void RenderForeground(CMPUT350::GameContext *context);
	bool IsAlive() const override;
	void Kill() override;
	const std::vector<CMPUT350::Shape> &GetShapes() override;
	void CollisionEnter(const std::shared_ptr<CMPUT350::CollisionObject> &obj) override;
	void CollisionEnter(const std::shared_ptr<CollisionObject> &obj, const CMPUT350::Point2D &collisionPoint) override;
	const CMPUT350::Rect &GetBounds() override;
	
	CMPUT350::Point2D GetLocation() const override;
	float GetRotation() const override;

private:
    bool mIsAlive;
    bool mIsPlayerBullet;
    CMPUT350::Point2D mCurrentLocation;
    CMPUT350::Point2D mPreviousLocation;
    CMPUT350::Point2D mHeading; // Direction
    CMPUT350::Rect mBounds;
    float mBulletSpeed;
};

#endif // BULLET_H
