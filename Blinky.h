#ifndef BLINKY_H
#define BLINKY_H

#include "Ghost.h"
#include <SFML/Graphics.hpp>

class Blinky : public Ghost
{
public:
    /**
     * @brief Blinky constructor
     * @param startPos Starting position of Blinky
     * @param moveSpeed Speed of Blinky
     * @param mapPos Position of the map
     * @param level Current level of the game
     * @details Initializes Blinky's position, speed, and animation. Calls the Ghost constructor to set m_pos, m_moveSpeed, etc.
     */
    Blinky(sf::Vector2i startPos, float moveSpeed, sf::Vector2f mapPos, int level);

    /**
     * @brief Blinky destructor
     * @details Default destructor.
     */
    ~Blinky() = default;

    /**
     * @brief Calculates the target tile for Blinky based on the player's position.
     * @param playerPos Position of the player
     * @return Target tile for Blinky
     * @details If in Scatter mode, sets target to (26, 0). Otherwise, sets target to player's position.
     */
    sf::Vector2i calculateTarget(sf::Vector2i playerPos);
    
    /**
     * @brief Sets the target tile for Blinky.
     * @param targetTile Target tile for Blinky
     * @details Sets the target tile for Blinky.
     */
    sf::Vector2i getPos() { return m_pos; } // Added getter for position

    /**
     * @brief Renders Blinky on the window.
     * @param window Reference to the SFML window
     * @details Draws Blinky's sprite on the window. If debug mode is enabled, draws additional debug information.
     */
    void render(sf::RenderWindow& window);

    /**
     * @brief Toggles debug mode.
     * @details If debug mode is enabled, additional debug information is drawn on the window.
     */
    void toggleDebug() { m_debug = !m_debug; }

private:
    /**
     * @brief Initializes Blinky's animation.
     * @details Loads the texture and sets up the animation frames for Blinky.
     */
    void initAnimation();
    sf::Vector2i m_targetTile;
    bool m_debug = false;

};

#endif // BLINKY_H
