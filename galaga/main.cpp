#include <random>
#include "GameEngine.h"
#include "Player.h"
#include "Enemy.h"
#include "Stars.h"
#include "Galaga.h"

unsigned int WIDTH = 600;
unsigned int HEIGHT = 800;

int main()
{
    CMPUT350::GameEngine engine(WIDTH, HEIGHT, "Galaga");
    engine.AddGameObject(std::make_shared<Galaga>(WIDTH, HEIGHT));
    engine.Run();
    return 0;
}
