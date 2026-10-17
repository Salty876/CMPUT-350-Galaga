#include "Galaga.h"
#include "GameContext.h"
#include "Keyboard.h"
#include "Player.h"
#include "Enemy.h"
#include "Bullet.h"
#include "Stars.h"

using namespace CMPUT350;

Galaga::Galaga(int width, int height) : mWidth(width), mHeight(height) {
    // No action needed
}

void Galaga::Initialize(GameContext *context) {
    EngineView *engine = context->mEngineView;

    // Movement can be done in two ways.
    for (Key k : {Key::Left, Key::A}) {
        engine->InstallKeyDownNotification(static_cast<int>(k), "MoveLeftDown");
        engine->InstallKeyUpNotification(static_cast<int>(k), "MoveLeftUp");
    }
    for (Key k : {Key::Right, Key::D}) {
        engine->InstallKeyDownNotification(static_cast<int>(k), "MoveRightDown");
        engine->InstallKeyUpNotification(static_cast<int>(k), "MoveRightUp");
    }

    engine->InstallKeyDownNotification(static_cast<int>(Key::Space), "Fire");

    // Press 5 to launch/restart game
    engine->InstallKeyDownNotification(static_cast<int>(Key::Num5), "InsertCoin");

    // Press 1 to start the game
    engine->InstallKeyDownNotification(static_cast<int>(Key::Num1), "StartGame");

    for (const char *message : {"InsertCoin", "StartGame", "EnemyDied"}) {
        context->mNotificationManager->Register(context->CurrObject, message);
    }

    engine->AddGameObject(std::make_shared<Stars>(250, Rect(0, 0, mWidth, mHeight)));
}

void Galaga::Update(GameContext *context) {
    mFrame = context->currentFrame;
    int elapsed = mFrame - mStateFrame;

    if (mState == State::Wait) {
        if (mStartPressed && mCredits > 0) {
            --mCredits;

            // Reset state
            mScore = 0;
            mLives = 3;
            mLevel = 1;
            StartLevel(context);
        }
        mStartPressed = false;
        return;
    }

    if (mState == State::Over) {
        if (elapsed >= Game_Over_Interval) {
            SetState(State::Wait);
        }
        return;
    }

    int alive = 0;
    bool allHome = true;

    // Count alive enemies
    for (const auto &weakEnemy : mEnemies) {
        if (weakEnemy.expired()) continue;

        std::shared_ptr<Enemy> enemy(weakEnemy);
        if (!enemy->IsAlive()) continue;

        ++alive;
        if (!enemy->InFormation()) allHome = false;
    }

    // Count each player death once
    if (
         mPlayerAlive && 
        (mPlayer.expired() || !std::shared_ptr<Player>(mPlayer)->IsAlive())) {
        
        mPlayerAlive = false;

        if (--mLives <= 0) {
            ClearField(context);
            SetState(State::Over);
            return;
        }
    }

    if (mState == State::Start) {
        if (elapsed >= Start_Interval) SetState(State::Waves);
    } else if (mState == State::Waves) {
        int wave = mSpawned / Ships;

        if (mSpawned == Waves * Ships) {
            if (allHome) SetState(State::Play);
        } else if (elapsed >= mSpawned * Spawn_Interval + wave * Wave_Interval) {
            // One column per ship
            int ship = mSpawned % Ships;

            float left = (mWidth - (Ships - 1) * 70.f) / 2.f;

            Point2D slot(left + ship * 70.f, 120.f + wave * 60.f);

            // Alternate sides between waves
            float startX = (wave % 2 == 0) ? mWidth * 0.2f : mWidth * 0.8f;
            float farX = mWidth - startX;
            std::vector<Point2D> path = {
                Point2D(startX, -40.f), Point2D(startX, mHeight * 0.7f),
                Point2D(farX, mHeight * 0.7f), Point2D(farX, mHeight * 0.45f),
                Point2D(farX, mHeight * 0.3f), Point2D(slot.x, slot.y + 80.f), slot};

            std::shared_ptr<Enemy> enemy;
            if (wave == 0) enemy = std::make_shared<Enemy3>(slot, path);
            else if (wave < 3) enemy = std::make_shared<Enemy2>(slot, path);
            else enemy = std::make_shared<Enemy1>(slot, path);

            context->mEngineView->AddGameObject(enemy);
            mEnemies.push_back(enemy);
            ++mSpawned;
        }
    } else if (mState == State::Play) {
        if (alive == 0) {
            ++mLevel;
            StartLevel(context);
        } else if (!mPlayerAlive) {
            if (allHome) SpawnPlayer(context);
        } else if (mFrame % Attack_Interval == 0) {
            mAttackIndex += 5;
            std::weak_ptr<Enemy> weakEnemy = mEnemies[mAttackIndex % mEnemies.size()];

            if (!weakEnemy.expired()) {
                std::shared_ptr<Enemy> enemy(weakEnemy);

                if (enemy->IsAlive() && enemy->InFormation()) {
                    enemy->Attack(context, std::shared_ptr<Player>(mPlayer)->GetLocation());
                }
            }
        }
    }
}

void Galaga::RenderUI(GameContext *context) {
    DrawContext *gui = context->GUIContext;
    float x = mWidth / 2.f;
    float y = mHeight / 2.f;

    auto centered = [&](const std::string &text, int size, float y) {
        gui->DrawCenteredText(text, size, Point2D(x, y), Colors::white);
    };

    centered("Credits: " + std::to_string(mCredits), 24, mHeight - 30.f);

    if (mState == State::Wait) {
        centered("Galaga", 72, y - 150.f);
        centered(mCredits > 0 ? "Press 1 to start the game." : "Press 5 to insert a coin.", 32, y);
        return;
    }

    gui->DrawText("Score: " + std::to_string(mScore), 24, Point2D(20.f, 10.f), Colors::white);
    gui->DrawText("Level: " + std::to_string(mLevel), 24, Point2D(mWidth - 140.f, 10.f), Colors::white);
    gui->DrawText("Remaining lives: " + std::to_string(mLives), 24, Point2D(20.f, mHeight - 70.f), Colors::white);

    if (mState == State::Start) centered("Level " + std::to_string(mLevel), 48, y);
    if (mState == State::Over) centered("Game over!", 72, y);
}

void Galaga::ReceiveNotification(const std::string &key) {
    if (key == "InsertCoin") {
        ++mCredits;
    } else if (key == "StartGame") {
        mStartPressed = true;
    } else if (key == "EnemyDied") {
        for (auto i = mEnemies.begin(); i != mEnemies.end();) {
            if (it->expired()) {
                i = mEnemies.erase(it);
                continue;
            }

            std::shared_ptr<Enemy> enemy(*i);
            if (enemy->IsAlive()) {
                ++i;
                continue;
            }

            mScore += enemy->GetPoints();
            i = mEnemies.erase(i);
        }
    }
}

void Galaga::SetState(State state) {
    mState = state;
    mStateFrame = mFrame;
}

void Galaga::StartLevel(GameContext *context) {
    ClearField(context);
    if (!mPlayerAlive) SpawnPlayer(context);

    mSpawned = 0;
    SetState(State::Start);
}

void Galaga::SpawnPlayer(GameContext *context) {
    std::shared_ptr<Player> player = std::make_shared<Player>(Point2D(mWidth / 2.f, mHeight - 120.f));

    context->mEngineView->AddGameObject(player);
    mPlayer = player;
    mPlayerAlive = true;
}

// Kill every object except Player
void Galaga::ClearField(GameContext *context) {
    mEnemies.clear();
    for (const auto &obj : *context->mEngineView) {
        if (std::dynamic_pointer_cast<Bullet>(obj) != nullptr || std::dynamic_pointer_cast<Enemy>(obj) != nullptr) {
            obj->Kill();
        }
    }
}
