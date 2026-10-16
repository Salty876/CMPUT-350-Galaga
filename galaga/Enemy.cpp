#include "Enemy.h"
#include "Bullet.h"
#include "MathUtil.h"
#include "bezier.h"



Enemy::Enemy(CMPUT350::Point2D loc, const std::vector<CMPUT350::Point2D> &path): mInitialPath(path), mAttackPath(path)
{   
    float mEnemySize = 20.0f; // This can be changed to a parameter

    this->mLocation = mInitialPath.GetPoint(0.f);
    this->mFormationLocation = loc;
    this->mIsAlive = true;
    this->mState = ENTRY;
    this->mRotation = 0.f;
    this->mPathProgress = 0.f;
    this->mFormationDirection = 1.f;

    CMPUT350::Point2D localTopLeft = {-mEnemySize / 2, -mEnemySize / 2};
    this->mBounds = CMPUT350::Rect(localTopLeft, mEnemySize, mEnemySize); // Enemy is a square of length mEnemySize
    this->mShapes.push_back(this->mBounds);

}

Enemy1::Enemy1(CMPUT350::Point2D loc, const std::vector<CMPUT350::Point2D> &path): Enemy(loc, path) {
    this->mHealth = 1;
}

Enemy2::Enemy2(CMPUT350::Point2D loc, const std::vector<CMPUT350::Point2D> &path): Enemy(loc, path) {
    this->mHealth = 2;
}

void Enemy::Initialize(CMPUT350::GameContext* context)
{
    // No action required
}

void Enemy::Update(CMPUT350::GameContext* context)
{
    // Check if enemy even is ALIVE
    if (!this->IsAlive()){
        return ;
    }

    switch (this->mState){
        // ON ENTRY
        case ENTRY: {
            this->mLocation = mInitialPath.GetPoint(mPathProgress);
            CMPUT350::Point2D slope = mInitialPath.GetSlope(mPathProgress);
            this->mRotation = std::atan2(slope.y, slope.x);
            mPathProgress += (1.f / 60.f);
            
            // Check if entry is finished
            if (mPathProgress >= 1.f){
                this->mLocation = mFormationLocation;
                this->mState = FORMATION;
            }
            break;
        }
        case FORMATION: {
            float speed = 0.5f;
            float maxOff = 20.f;
            

            mLocation.x += speed * mFormationDirection;

            float maxRight = mFormationLocation.x + maxOff;
            float maxLeft  = mFormationLocation.x - maxOff;

            if (mLocation.x >= maxRight) {
                mLocation.x = maxRight;
                mFormationDirection = -1.f;
            }
            else if (mLocation.x <= maxLeft) {
                mLocation.x = maxLeft;
                mFormationDirection = 1.f;
            }

            mLocation.y = mFormationLocation.y;
            mRotation = 0;
            break;
        }

        case ATTACK: {
            mPathProgress += 1.f / 60.f;
            mLocation = mAttackPath.GetPoint(mPathProgress);
            CMPUT350::Point2D newSlope = mAttackPath.GetSlope(mPathProgress);
            mRotation = std::atan2(newSlope.y, newSlope.x);
            // will fire bullets here

            if (mPathProgress >= 1){
                mPathProgress = 0;
                CMPUT350::Point2D p0 = mLocation;
                CMPUT350::Point2D p1 = mLocation + CMPUT350::Point2D(50.f, -150.f);
                CMPUT350::Point2D p2 = mFormationLocation + CMPUT350::Point2D(0, 150.f);
                CMPUT350::Point2D p3 = mFormationLocation;
                mAttackPath = CMPUT350::Bezier({p0, p1, p2, p3});
                mState = RETURNING;
            }
            break;

        }

        case RETURNING: {
            this->mLocation = mInitialPath.GetPoint(mPathProgress);
            CMPUT350::Point2D slope = mInitialPath.GetSlope(mPathProgress);
            this->mRotation = std::atan2(slope.y, slope.x);
            mPathProgress += (1.f / 60.f);
            
            // Check if return is finished
            if (mPathProgress >= 1.f){
                this->mLocation = mFormationLocation;
                this->mState = FORMATION;
                mRotation = 0;
            }
            break;
        }
    }
}

bool Enemy1::Attack(CMPUT350::GameContext *context, const CMPUT350::Point2D &target)  {
    if (this->mState != FORMATION) {
        return false;
    }

    CMPUT350::Point2D p0 = mLocation;
    CMPUT350::Point2D p1 = mLocation + CMPUT350::Point2D(0.f, 150.f);
    CMPUT350::Point2D p2 = target + CMPUT350::Point2D(0.f, -150.f);
    CMPUT350::Point2D p3 = target;

    mAttackPath = CMPUT350::Bezier({p0, p1, p2, p3});

    mPathProgress = 0.f;
    mState = ATTACK;
    return true;
}

bool Enemy2::Attack(CMPUT350::GameContext *context, const CMPUT350::Point2D &target)  {
    if (this->mState != FORMATION) {
        return false;
    }

    CMPUT350::Point2D p0 = mLocation;
    CMPUT350::Point2D p1 = mLocation + CMPUT350::Point2D(110.f, -150.f);
    CMPUT350::Point2D p2 = target + CMPUT350::Point2D(-110.f, 150.f);
    CMPUT350::Point2D p3 = target;

    mAttackPath = CMPUT350::Bezier({p0, p1, p2, p3});

    mPathProgress = 0.f;
    mState = ATTACK;
    return true;
}

void Enemy::LateUpdate(CMPUT350::GameContext* context)
{
    // No action required
}

bool Enemy::HandleKeyEvent(CMPUT350::GameContext* context, char key)
{
    return false;
}

void Enemy::RenderBackground(CMPUT350::GameContext* context)
{
    // No action required
}

void Enemy::RenderForeground(CMPUT350::GameContext* context)
{
    CMPUT350::DrawContext* api = context->ScreenContext;
    api->DrawRect(GetBounds(), CMPUT350::Colors::red);
}

void Enemy::CollisionEnter(const std::shared_ptr<CMPUT350::CollisionObject>& obj)
{
    // Test the type of colliding object
    Bullet* ptr = dynamic_cast<Bullet*>(obj.get());
    if (ptr != nullptr) Kill();
}

void Enemy::Kill()
{
    mIsAlive = false;
}

bool Enemy::IsAlive() const
{
    return mIsAlive;
}

const CMPUT350::Rect& Enemy::GetBounds()
{
    return mBounds;
}
