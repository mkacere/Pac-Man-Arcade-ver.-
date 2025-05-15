#ifndef CLYDE_H
#define CLYDE_H

#include "Ghost.h"
#include <SFML/Graphics.hpp>
#include <iostream>

class Clyde : public Ghost
{
public:
    /**
     * @brief Clyde constructor
     * @param startPos Starting position of Clyde
     * @param moveSpeed Speed of Clyde
     * @param mapPos Position of the map
     * @param level Current level of the game
     * @details Initializes Clyde's position, speed, and animation. Calls the Ghost constructor to set m_pos, m_moveSpeed, etc.
     */
    Clyde(sf::Vector2i startPos, float moveSpeed, sf::Vector2f mapPos, int level);
    ~Clyde() = default;

    /**
     * @brief Calculates the target tile for Clyde based on the player's position.
     * @param playerPos Position of the player
     * @return Target tile for Clyde
     * @details If in Scatter mode, sets target to (26, 0). Otherwise, sets target to player's position.
     */
    sf::Vector2i calculateTarget(sf::Vector2i playerPos);

    /**
     * @brief Sets the target tile for Clyde.
     * @param targetTile Target tile for Clyde
     * @details Sets the target tile for Clyde.
     */
    void render(sf::RenderWindow& window, sf::Vector2i pacPos);

    /**
     * @brief Renders Clyde on the window.
     * @param window Reference to the SFML window
     * @details Draws Clyde's sprite on the window. If debug mode is enabled, draws additional debug information.
     */
    void toggleDebug() { m_debug = !m_debug; }

private:
    /**
     * @brief Initializes Clyde's animation.
     * @details Loads the texture and sets up the animation frames for Clyde.
     */
    void initAnimation();
    sf::Vector2i m_targetTile;
    bool m_debug = false;
};

#endif // CLYDE_H
