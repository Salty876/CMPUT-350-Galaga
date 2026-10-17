#include <algorithm>
#include <cmath>
#include "Enemy.h"
#include "Bullet.h"
#include "Player.h"
#include "GameContext.h"
#include "MathUtil.h"
#include "Bezier.h"

Enemy::Enemy(CMPUT350::Point2D loc, const std::vector<CMPUT350::Point2D> &path): mInitialPath(path), mAttackPath(path)
{   
    float mEnemySize = 20.0f; // This can be changed to a parameter

    this->mLocation = mInitialPath.GetPoint(0.f);
    this->mFormationLocation = loc;
    this->mIsAlive = true;
    this->mState = ENTRY;
    this->mRotation = 0.f;
    this->mPathProgress = 0.f;
    this->mEntrySegments = (path.size() - 1) / 3.f; // 3k + 1 points make k segments
    this->mSpeed = 12.f;
    this->mHealth = 1;
    this->mPoints = 50;
    this->mNotifier = nullptr;

    CMPUT350::Point2D localTopLeft = {-mEnemySize / 2, -mEnemySize / 2};
    this->mBounds = CMPUT350::Rect(localTopLeft, mEnemySize, mEnemySize); // Enemy is a square of length mEnemySize
    this->mShapes.push_back(this->mBounds);

}

Enemy1::Enemy1(CMPUT350::Point2D loc, const std::vector<CMPUT350::Point2D> &path): Enemy(loc, path) {
    this->mHealth = 1;
    this->mPoints = 50;
}

Enemy2::Enemy2(CMPUT350::Point2D loc, const std::vector<CMPUT350::Point2D> &path): Enemy(loc, path) {
    this->mHealth = 2;
    this->mPoints = 80;
}

Enemy3::Enemy3(CMPUT350::Point2D loc, const std::vector<CMPUT350::Point2D> &path): Enemy(loc, path) {
    this->mHealth = 1;
    this->mPoints = 150;
}

void Enemy::Initialize(CMPUT350::GameContext* context)
{
    // Kept so the enemy can announce its death from CollisionEnter
    mNotifier = context->mNotificationManager;
}

bool Enemy::FollowPath(const CMPUT350::Bezier &path, float segments)
{
    // Stay just inside the end so the last segment is always valid
    float end = segments - 0.001f;

    // The slope's length is how far one unit of t moves us, so dividing the
    // speed by it gives the same pixels per frame wherever the control points are
    CMPUT350::Point2D slope = path.GetSlope(std::min(mPathProgress, end));
    float length = std::sqrt(slope.Dot(slope));
    mPathProgress = std::min(mPathProgress + mSpeed / std::max(length, 1.f), end);

    this->mLocation = path.GetPoint(mPathProgress);
    slope = path.GetSlope(mPathProgress);

    // The enemy is drawn pointing down at rotation 0, so subtract a quarter turn
    this->mRotation = std::atan2(slope.y, slope.x) - 1.5707963f;

    return mPathProgress >= end;
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
            // Check if entry is finished
            if (FollowPath(mInitialPath, mEntrySegments)){
                this->mLocation = mFormationLocation;
                this->mState = FORMATION;
            }
            break;
        }
        case FORMATION: {
            // Based only on the frame, so every enemy sways in sync
            mLocation.x = mFormationLocation.x + 20.f * std::sin(context->currentFrame / 15.f);
            mLocation.y = mFormationLocation.y;
            mRotation = 0;
            break;
        }

        case ATTACK: {
            bool finished = FollowPath(mAttackPath, 1.f);

            if (context->currentFrame % 60 == 0) {
                CMPUT350::Point2D heading = CMPUT350::Point2D(0, 1);
                std::shared_ptr<Bullet> shot = std::make_shared<Bullet>(mLocation, heading, false, context->CurrObject);
                context->mEngineView->AddGameObject(shot);
            }

            if (finished){
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
            // Check if return is finished
            if (FollowPath(mAttackPath, 1.f)){
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

bool Enemy3::Attack(CMPUT350::GameContext *context, const CMPUT350::Point2D &target)  {
    if (this->mState != FORMATION) {
        return false;
    }

    // Mirror image of Enemy2's path
    CMPUT350::Point2D p0 = mLocation;
    CMPUT350::Point2D p1 = mLocation + CMPUT350::Point2D(-110.f, -150.f);
    CMPUT350::Point2D p2 = target + CMPUT350::Point2D(110.f, 150.f);
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

void Enemy::RenderBackground(CMPUT350::GameContext* context)
{
    // No action required
}

// Enemy1: red square
void Enemy::RenderForeground(CMPUT350::GameContext* context)
{
    CMPUT350::DrawContext* api = context->ScreenContext;
    api->DrawRect(GetBounds(), CMPUT350::Colors::red);
}

// Enemy2: circle, purple at full health and red once it has been hit
void Enemy2::RenderForeground(CMPUT350::GameContext* context)
{
    CMPUT350::RGBColor color = (mHealth > 1) ? CMPUT350::RGBColor(160, 60, 220) : CMPUT350::Colors::red;
    context->ScreenContext->DrawCircle(CMPUT350::Point2D(0.f, 0.f), 11.f, color);
}

// Enemy3: cyan triangle pointing down
void Enemy3::RenderForeground(CMPUT350::GameContext* context)
{
    CMPUT350::DrawContext* api = context->ScreenContext;
    CMPUT350::RGBColor color(0, 200, 255);

    CMPUT350::Point2D tip(0.f, 11.f);
    CMPUT350::Point2D left(-11.f, -9.f);
    CMPUT350::Point2D right(11.f, -9.f);

    api->DrawLine(left, tip, 3.f, color);
    api->DrawLine(right, tip, 3.f, color);
    api->DrawLine(left, right, 3.f, color);
}

CMPUT350::Point2D Enemy::GetLocation() const
{
    return mLocation;
}

float Enemy::GetRotation() const
{
    return mRotation;
}

void Enemy::CollisionEnter(const std::shared_ptr<CMPUT350::CollisionObject>& obj)
{
    if (!mIsAlive) return;

    Bullet* bullet = dynamic_cast<Bullet*>(obj.get());
    if (bullet != nullptr && !bullet->IsPlayerBullet()) return;
    if (bullet == nullptr && dynamic_cast<Player*>(obj.get()) == nullptr) return;
    if (bullet != nullptr && --mHealth > 0) return;

    Kill();
    mNotifier->Notify("EnemyDied");
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

const std::vector<CMPUT350::Shape>& Enemy::GetShapes()
{
    return mShapes;
}

bool Enemy::InFormation() const
{
    return mState == FORMATION;
}

int Enemy::GetPoints() const
{
    return InFormation() ? mPoints : mPoints * 2;
}
