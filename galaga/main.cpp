#include <random>
#include "GameEngine.h"
#include "Player.h"
#include "Enemy.h"
#include "Stars.h"

int main()
{
    CMPUT350::GameEngine engine(768, 1024, "Galaga");
    auto player = std::make_shared<Player>(CMPUT350::Point2D(768 / 2, 700));
    engine.AddGameObject(player);
    engine.AddGameObject(std::make_shared<Stars>(250, CMPUT350::Rect(0, 0, 768, 1024)));
    for (int y = 1; y <= 10; y++) {
        for (int x = 0; x < 4; x++) {
            auto enemy = std::make_shared<Enemy>(CMPUT350::Point2D(100 + (x * 200) + (std::rand() % 50), 100 * (y / 2.0f) + (std::rand() % 25)));
            engine.AddGameObject(enemy);
        }
    }
    engine.Run();
    return 0;
}
