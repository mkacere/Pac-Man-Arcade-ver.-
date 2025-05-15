#ifndef GAME_H
#define GAME_H

#include <SFML/Graphics.hpp>
#include "StateManager.h"

class Game
{
public:

    /**
     * @brief Construct a new Game object
     * 
     */
    Game();

    /**
     * @brief the main game loop
     * 
     */
    void run();

private:
    sf::RenderWindow m_window;
    StateManager m_stateManager;
};

#endif // GAME_H
