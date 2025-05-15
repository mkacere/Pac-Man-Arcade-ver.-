#ifndef GAMESTATE_H
#define GAMESTATE_H

#include <SFML/Graphics.hpp>

class GameState {
public:
    
    /**
     * @brief Destroy the Game State object
     * 
     */
    virtual ~GameState() = default;

    /**
     * @brief Handle events for the game state
     * 
     * @param window Reference to the SFML window
     */
    virtual void handleEvents(sf::RenderWindow& window) = 0;

    /**
     * @brief Update the game state
     * 
     * @param dt Time since last update
     */
    virtual void update(sf::Time dt) = 0;

    /**
     * @brief Render the game state
     * 
     * @param window Reference to the SFML window
     */
    virtual void render(sf::RenderWindow& window) = 0;
};

#endif // GAMESTATE_H
