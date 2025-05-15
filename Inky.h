#ifndef INKY_H
#define INKY_H

#include "Ghost.h"
#include <SFML/Graphics.hpp>
#include <iostream>

class Inky : public Ghost
{
public:
    /**
     * @brief Inky constructor
     * @param startPos Starting position of Inky
     * @param moveSpeed Speed of Inky
     * @param mapPos Position of the map
     * @param level Current level of the game
     * @details Initializes Inky's position, speed, and animation. Calls the Ghost constructor to set m_pos, m_moveSpeed, etc.
     */
    Inky(sf::Vector2i startPos, float moveSpeed, sf::Vector2f mapPos, int level);
    
    /**
     * @brief Inky destructor
     * @details Default destructor.
     */
    ~Inky() = default;

    /**
     * @brief Calculates the target tile for Inky based on the player's position and Blinky's position.
     * @param playerPos Position of the player
     * @param playerDirection Direction of the player
     * @param blinkyPos Position of Blinky
     * @return Target tile for Inky
     * @details If in Scatter mode, sets target to (26, 0). Otherwise, sets target to a calculated position based on the player's position and Blinky's position.
     */
    sf::Vector2i calculateTarget(sf::Vector2i playerPos, sf::Vector2i playerDirection, sf::Vector2i blinkyPos);

    /**
     * @brief renders the animation to the window
     * 
     * @param window 
     */
    void render(sf::RenderWindow& window);

    /**
     * @brief Toggles debug mode.
     * @details If debug mode is enabled, additional debug information is drawn on the window.
     */
    void toggleDebug() { m_debug = !m_debug; }

private:
    /**
     * @brief Initializes Inky's animation.
     * @details Loads the texture and sets up the animation frames for Inky.
     */
    void initAnimation();
    
    sf::Vector2i m_targetTile;
    bool m_debug = false;

};

#endif // INKY_H
