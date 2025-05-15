#ifndef PINKY_H
#define PINKY_H

#include "Ghost.h"
#include <SFML/Graphics.hpp>

class Pinky : public Ghost
{
public:
    /**
     * @brief Pinky constructor
     * @param startPos Starting position of Pinky
     * @param moveSpeed Speed of Pinky
     * @param mapPos Position of the map
     * @param level Current level of the game
     * @details Initializes Pinky's position, speed, and animation. Calls the Ghost constructor to set m_pos, m_moveSpeed, etc.
     */
    Pinky(sf::Vector2i startPos, float moveSpeed, sf::Vector2f mapPos, int level);
    
    /**
     * @brief Pinky destructor
     * @details Default destructor.
     */
    ~Pinky() = default;

    /**
     * @brief Calculates the target tile for Pinky based on the player's position and direction.
     * @param playerPos Position of the player
     * @param playerDirection Direction of the player
     * @return Target tile for Pinky
     */
    sf::Vector2i calculateTarget(sf::Vector2i playerPos, sf::Vector2i playerDirection);

    /**
     * @brief Renders Pinky on the window.
     * @param window Reference to the SFML window
     * @details Draws Pinky's sprite on the window. If debug mode is enabled, draws additional debug information.
     */
    void render(sf::RenderWindow& window);

    /**
     * @brief Toggles debug mode.
     * @details If debug mode is enabled, additional debug information is drawn on the window.
     */
    void toggleDebug() { m_debug = !m_debug; }

private:
    /**
     * @brief Initializes Pinky's animation.
     * @details Loads the texture and sets up the animation frames for Pinky.
     */
    void initAnimation();
    sf::Vector2i m_targetTile;
    bool m_debug = false;

};

#endif // PINKY_H
