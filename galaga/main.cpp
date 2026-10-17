#include <random>
#include "GameEngine.h"
#include "Player.h"
#include "Enemy.h"
#include "Stars.h"
#include "Galaga.h"

int main()
{
    CMPUT350::GameEngine engine(480, 640, "Galaga");
    engine.AddGameObject(std::make_shared<Galaga>(480, 640));
    engine.Run();
    return 0;
}
