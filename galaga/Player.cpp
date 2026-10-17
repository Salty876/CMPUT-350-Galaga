#include <cassert>
#include "Player.h"
#include "Bullet.h"
#include "Enemy.h"

Player::Player(CMPUT350::Point2D loc)
{
    // TODO: Update code
    mLocation = loc;
    mPlayerSIze = 40.f;
    mMoveSpeed = 18.f;
    mIsAlive = true;
    mMovingLeft = false;
    mMovingRight = false;
    mFire = false;

    updateBounds();
}

void Player::Initialize(CMPUT350::GameContext* context)
{
    for (const char* message : {"MoveLeftDown", "MoveLeftUp", "MoveRightDown", "MoveRightUp", "Fire"}) {
        context->mNotificationManager->Register(context->CurrObject, message);
    }
}

void Player::LateUpdate(CMPUT350::GameContext* context)
{
    // No action needed
}

void Player::Update(CMPUT350::GameContext* context)
{
    float halfSize = mPlayerSIze / 2.f;
    float windowWidth = context->ScreenContext->GetWindowWidth();

    if (mMovingLeft) {
        mLocation.x -= mMoveSpeed;
    }
    if (mMovingRight) {
        mLocation.x += mMoveSpeed;
    }

    // Stop player from leaving the screen
    if (mLocation.x - halfSize < 0) {
        mLocation.x = halfSize;
    }
    if (mLocation.x + halfSize > windowWidth) {
        mLocation.x = windowWidth - halfSize;
    }

    if (mFire) {
        mFire = false;

        CMPUT350::Point2D bulletLocation(mLocation.x, mLocation.y);
        CMPUT350::Point2D bulletHeading(0.f, -1.f);

        // CurrObject is this player while it is being updated
        if (mBullet1.expired()) {
            std::shared_ptr<Bullet> bullet = std::make_shared<Bullet>(bulletLocation, bulletHeading, true, context->CurrObject);
            context->mEngineView->AddGameObject(bullet);
            mBullet1 = bullet;
        }
        else if (mBullet2.expired()) {
            std::shared_ptr<Bullet> bullet = std::make_shared<Bullet>(bulletLocation, bulletHeading, true, context->CurrObject);
            context->mEngineView->AddGameObject(bullet);
            mBullet2 = bullet;
        }
    }
}

void Player::ReceiveNotification(const std::string& key)
{
    if (key == "MoveLeftDown") mMovingLeft = true;
    else if (key == "MoveLeftUp") mMovingLeft = false;
    else if (key == "MoveRightDown") mMovingRight = true;
    else if (key == "MoveRightUp") mMovingRight = false;
    else if (key == "Fire") mFire = true;
}

void Player::RenderBackground(CMPUT350::GameContext* context)
{
    // No action needed
}

void Player::RenderForeground(CMPUT350::GameContext* context)
{
    CMPUT350::DrawContext *drawMachine = context->ScreenContext;
    float halfSize = mPlayerSIze / 2.f;

    // Draw at (0, 0) and let game engine do the translation
    CMPUT350::Point2D front(0.f, -halfSize);
    CMPUT350::Point2D left(-halfSize, halfSize);
    CMPUT350::Point2D right(halfSize, halfSize);

    drawMachine->DrawLine(left, front, 3.f, CMPUT350::Colors::white);
    drawMachine->DrawLine(right, front, 3.f, CMPUT350::Colors::white);
    drawMachine->DrawLine(right, left, 3.f, CMPUT350::Colors::white);
}

CMPUT350::Point2D Player::GetLocation() const
{
    return mLocation;
}

float Player::GetRotation() const
{
    return 0.f;
}

void Player::CollisionEnter(const std::shared_ptr<CMPUT350::CollisionObject>& obj)
{
    std::shared_ptr<Bullet> bullet = std::dynamic_pointer_cast<Bullet>(obj);

    if (bullet != nullptr) {
        // ignore the player bullets
        if (!bullet->IsPlayerBullet()) {
            Kill();
        }
    }
    else if (std::dynamic_pointer_cast<Enemy>(obj) != nullptr) Kill();
}

void Player::Kill()
{
    mIsAlive = false;
}

bool Player::IsAlive() const
{
    // TODO: Update code
    return mIsAlive;
}

const CMPUT350::Rect& Player::GetBounds()
{
    return mBounds;
}

const std::vector<CMPUT350::Shape>& Player::GetShapes()
{
    return mShapes;
}

void Player::updateBounds() {
    CMPUT350::Point2D topLeft(-mPlayerSIze / 2.f, -mPlayerSIze / 2.f);
    mBounds = CMPUT350::Rect(topLeft, mPlayerSIze, mPlayerSIze);

    mShapes.clear();
    mShapes.push_back(CMPUT350::Shape(mBounds));
}


