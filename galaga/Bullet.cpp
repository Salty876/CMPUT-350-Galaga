#include "Bullet.h"
#include "CollisionObject.h"
#include "DrawContext.h"
#include "Enemy.h"
#include "MathUtil.h"
#include "Player.h"
#include <cmath>
#include<memory>

Bullet::Bullet(CMPUT350::Point2D location, CMPUT350::Point2D heading, bool player, std::weak_ptr<CMPUT350::GameObject> parent)
{
    this->mCurrentLocation = location;
    this->mPreviousLocation = location;
    this->mHeading = heading;
    this->mIsPlayerBullet = player;
    this->mIsAlive = true;

    // Since collision in the same frame as creation is not possible
    // initiate the bound as a point and update it in the first Update()
    this->mBounds = CMPUT350::Rect({0.f, 0.f}, {0.f, 0.f});

    // Using a hard-coded for now, we could
    // make this a parameter if needed
    this->mBulletSpeed = 15.0f;
    this->mParent = parent;
    this->collided = false;
    this->mCollisionPoint = CMPUT350::Point2D(0.f, 0.f);
    this->hasCollisionPoint = false;
}

bool Bullet::IsPlayerBullet()
{
    return mIsPlayerBullet;
}

void Bullet::Initialize(CMPUT350::GameContext* context)
{
    // No action required
}

void Bullet::Update(CMPUT350::GameContext* context)
{
    mPreviousLocation = mCurrentLocation;
    mCurrentLocation += mBulletSpeed * mHeading; // Speed * Direction

    bool condition = (
        mCurrentLocation.x < 0 ||
        mCurrentLocation.x > context->ScreenContext->GetWindowWidth() ||
        mCurrentLocation.y < 0 ||
        mCurrentLocation.y > context->ScreenContext->GetWindowHeight()
    );



    if (condition) Kill();

    if (mParent.lock() == nullptr){ 
        Kill() ;
        return;
    }

    else if (mParent.lock()->IsAlive() == false){ 
        Kill() ;
        return;
    }
    // Kill any bullets that escape the screen
    

    // Update bounds accordingly
    float length = CMPUT350::Line(mPreviousLocation, mCurrentLocation).Length();
    CMPUT350::Point2D tail(-length, 0.f);
    CMPUT350::Point2D head(0.f, 0.f);

    mShapes.clear();
    mShapes.push_back(CMPUT350::Shape(CMPUT350::Line(head, tail)));
    this->mBounds = CMPUT350::Rect(CMPUT350::Point2D(-length, -3.f), length, 6.f);
    return;
}

void Bullet::LateUpdate(CMPUT350::GameContext* context)
{
    if (collided) {
        Kill();
    }
}


void Bullet::RenderBackground(CMPUT350::GameContext* context)
{
    // No action required
}

void Bullet::RenderForeground(CMPUT350::GameContext* context)
{
    CMPUT350::DrawContext* api = context->ScreenContext;
    if (collided) {
        if (hasCollisionPoint){
            api->DrawCircle(mCollisionPoint, 10.f, CMPUT350::Colors::white);
        }else {
            api->DrawCircle({0.f, 0.f}, 10.f, CMPUT350::Colors::white);
        }
    }   
    else {
        if (IsPlayerBullet()){
            api->DrawLine({-1.f * CMPUT350::Line(mPreviousLocation, mCurrentLocation).Length(), 0.f}, {0.f, 0.f}, 5.0f, CMPUT350::Colors::white);
        }
        else {
            api->DrawLine({-1.f * CMPUT350::Line(mPreviousLocation, mCurrentLocation).Length(), 0.f}, {0.f, 0.f}, 5.0f, CMPUT350::Colors::red);
        }
    }
}

void Bullet::CollisionEnter(const std::shared_ptr<CMPUT350::CollisionObject>& obj)
{   
    // check if we have an enemy parent
    if (mParent.lock() == nullptr) {
        // parent dont exist anymore act accordingly
        Kill();
        return;
    }

    if (mParent.lock()->IsAlive() != true) {
        Kill();
        return ;
    }
    bool bigValid = false;

    // check if parent is enemy
    std::shared_ptr<Enemy> eObj = std::dynamic_pointer_cast<Enemy>(mParent.lock());
    
    if (eObj) {
        // we have a parent as enemy
        bool valid = IsValidTargetPlayer(obj);
        if (valid) {
            // destroy bullet
            bigValid = true;
           
        }
    }

    // check if parent is player
    std::shared_ptr<Player> pObj = std::dynamic_pointer_cast<Player>(mParent.lock());

    if (pObj) {
        // we have a parent as enemy
        bool valid = IsValidTargetEnemy(obj);
        if (valid) {
            // destroy bullet
            bigValid = true;
           
        }
    }

    if (bigValid == false) {
        return ;
    }

    collided = true;

}

void Bullet::CollisionEnter(const std::shared_ptr<CMPUT350::CollisionObject>& obj, const CMPUT350::Point2D &collisionPoint) {
    bool valid = false;
    // check if we have an enemy parent
    if (mParent.lock() == nullptr) {
        // parent dont exist anymore act accordingly
        Kill();
        return;
    }

    if (mParent.lock()->IsAlive() != true) {
        Kill();
        return ;
    }

    // check if parent is enemy
    std::shared_ptr<Enemy> eObj = std::dynamic_pointer_cast<Enemy>(mParent.lock());

    if (eObj) {
        // we have a parent as enemy
        if (IsValidTargetPlayer(obj)) {
            valid = true;
        } 
    }

    // check if parent is player
    std::shared_ptr<Player> pObj = std::dynamic_pointer_cast<Player>(mParent.lock());

    if (pObj) {
        // we have a parent as enemy
        if (IsValidTargetEnemy(obj)) {
            valid = true;
        } 
    }

    if (valid == false) {
        return ;
    }

    if (!this->hasCollisionPoint) {
        this->mCollisionPoint = collisionPoint;
        this->hasCollisionPoint = true;
    }

    else if (collisionPoint.y > this->mCollisionPoint.y){
        mCollisionPoint = collisionPoint;
    }

    collided = true;


}

void Bullet::Kill()
{
    this->mIsAlive = false;
}

bool Bullet::IsAlive() const
{
    return mIsAlive;
}

const CMPUT350::Rect& Bullet::GetBounds()
{
    return mBounds;
}

CMPUT350::Point2D Bullet::GetLocation() const {
    return this->mCurrentLocation;
}

float Bullet::GetRotation() const {
    return std::atan2(this->mHeading.y, this->mHeading.x);
}

const std::vector<CMPUT350::Shape> & Bullet::GetShapes() {
    return mShapes;
};

bool Bullet::IsValidTargetPlayer(std::shared_ptr<CMPUT350::CollisionObject> obj) {
    std::shared_ptr<Player> mObj = std::dynamic_pointer_cast<Player>(obj);
    if (mObj) // Colliding Object is Player
    {
        if (IsPlayerBullet()) {
            return false;
        }

        return true;
    }

    return false;
    
}

bool Bullet::IsValidTargetEnemy(std::shared_ptr<CMPUT350::CollisionObject> obj) {
    std::shared_ptr<Enemy> mObj = std::dynamic_pointer_cast<Enemy>(obj);

    // enemy to enemy bullets are bad
    if (mObj) // Colliding Object is Enemy
    {
        if (IsPlayerBullet()) {
            return true;
        }
    }
    return false;
}





