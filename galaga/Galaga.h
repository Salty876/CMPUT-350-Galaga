#ifndef GALAGA_H
#define GALAGA_H

#include <memory>
#include <string>
#include <vector>
#include "GameObject.h"
#include "MathUtil.h"

class Player;
class Enemy;

class Galaga : public CMPUT350::GameObject {
public:
    Galaga(int width, int height);

    void Initialize(CMPUT350::GameContext *context) override;
    void Update(CMPUT350::GameContext *context) override;
    void RenderUI(CMPUT350::GameContext *context) override;
    void ReceiveNotification(const std::string &key) override;

private:
    // We are using frames as a measurement of time elapsed during the game.
    // The engine runs at 30 FPS, so 30 frames is 1 second. The following variables
    // are declared as static constexpr so that they are computed at compile time
    // and fixed for the duration of the Galaga Object's lifetime.
    // We could replace them as a configuration file / environment variables too.

    // Number of waves per level.
    static constexpr int Waves = 5;

    // Number of enemies in each wave.
    static constexpr int Ships = Waves;

    // Time between consecutive spawning of enemies
    // during a wave.
    static constexpr int Spawn_Interval = Waves;

    // Time before next wave starts. (1s)
    // Can adjust this to make game easier/harder.
    static constexpr int Wave_Interval = 30;

    // Time before consecutive attacks by enemies.
    // Can adjust this to make game easier/harder.
    static constexpr int Attack_Interval = 30;

    // Starting frame (2s)
    static constexpr int Start_Interval = 60;

    // Game-over frame (2s)
    static constexpr int Game_Over_Interval = Start_Interval;

    enum class State { 
        Wait, 
        Start, 
        Waves, 
        Play, 
        Over 
    };

    void SetState(State state);
    void StartLevel(CMPUT350::GameContext *context);
    void SpawnPlayer(CMPUT350::GameContext *context);
    void ClearField(CMPUT350::GameContext *context);

    int mWidth;
    int mHeight;

    State mState = State::Wait;
    int mFrame = 0; // Last frame seen in Update()
    int mStateFrame = 0; // Last frame where the state was switched.

    int mCredits = 0;
    int mScore = 0;
    int mLives = 0;
    int mLevel = 1;
    bool mStartPressed = false;

    int mSpawned = 0;
    size_t mAttackIndex = 0;
    bool mPlayerAlive = false;
    std::weak_ptr<Player> mPlayer;
    std::vector<std::weak_ptr<Enemy>> mEnemies;
};

#endif
