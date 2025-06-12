#include "ECS.h"
#include <GameSettings.h>

#include <iostream>

class MyCharacter: public Game::Entity
{
public:
    MyCharacter()
    {
        std::cout << this << "\n";
    }
    ~MyCharacter()
    {
        std::cout << "Character destroyed!\n";
    }

private:

};

void GameSettings::onSet()
{ 
}

EntryWorld::EntryWorld()
{
    auto myChar = this->addEntity<MyCharacter>();
}