#ifndef PLAYER_H
#define PLAYER_H

#include "AnimatedSprite.h"

#include <SFML/Graphics.hpp>

class Player
{
public:
    /**
     * @brief Player constructor
     * @param startPos Starting position of the player
     * @param mapPos Position of the map
     * @details Initializes the player's position, speed, and animation.
     */
    Player(sf::Vector2i startPos, sf::Vector2f mapPos);
    
    /**
     * @brief Player destructor
     * @details Default destructor.
     */
    ~Player() = default;

    /**
     * @brief Handle events for the player
     */
    void handleEvents();

    /**
     * @brief Update the player's position
     * @param dt Time since last update
     */
    void update(sf::Time dt);

    /**
     * @brief Render the player on the window
     * @param window Reference to the SFML window
     */
    void render(sf::RenderWindow& window);

    /**
     * @brief Set the map for the player
     * @param map The map or level layout
     */
    void setMap(const std::vector<std::vector<int>>& map);

    /**
     * @brief Set the player's speed
     * @param speed The speed of the player
     */
    void setSpeed(const float speed) { m_moveSpeed = speed; }

    /**
     * @brief Get the player's direction
     * @return sf::Vector2i The player's direction
     */
    sf::Vector2i getDirection() const { return m_currentDirection; }

    /**
     * @brief Get the player's position
     * @return sf::Vector2i The player's position
     */
    sf::Vector2i getPlayerPos();

    /**
     * @brief Get the Hit Box object
     * 
     * @return sf::FloatRect of the animated sprite
     */
    sf::FloatRect getHitBox() const { return m_animation.getGlobalBounds(); }

private:
    /**
     * @brief Initialize the player's animations
     * @details Loads the texture and sets up the animation frames for the player.
     */
    void initAnimations();

    /**
     * @brief Update the player's rotation based on the current direction
     */
    void updateRotation();

    /**
     * @brief Check if the player can move to a given position
     * @param pos The position to check
     * @return true if the move is valid, false otherwise
     */
    bool isValidMove(const sf::Vector2i& pos);

    AnimatedSprite m_animation;

    const sf::Vector2i m_tileSize{ 24, 24 };
    sf::Vector2i m_gridPos;       // Current tile
    sf::Vector2i m_targetPos;     // Target tile
    sf::Vector2i m_currentDirection{ -1, 0 };
    sf::Vector2i m_queuedDirection{ -1, 0 };
    sf::Vector2f m_mapPos;        // Map position

    bool m_isMoving = false;
    float m_moveSpeed;
    float m_moveProgress = 0.f;

    // The map layout:
    std::vector<std::vector<int>> m_mapVector;
};

#endif // PLAYER_H
