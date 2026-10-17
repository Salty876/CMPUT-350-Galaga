#include <random>
#include "GameEngine.h"
#include "Player.h"
#include "Enemy.h"
#include "Stars.h"
#include "Galaga.h"

int main()
{
    CMPUT350::GameEngine engine(768, 1024, "Galaga");
    engine.AddGameObject(std::make_shared<Galaga>(768, 1024));
    engine.Run();
    return 0;
}
