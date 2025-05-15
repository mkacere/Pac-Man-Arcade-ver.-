#ifndef GHOST_H
#define GHOST_H

#include "SFML/Graphics.hpp"
#include "AnimatedSprite.h"
#include <random>

class Ghost
{
public:
    /**
     * @brief Construct a new Ghost object
     * 
     * @param startPos startposition of the ghost
     * @param moveSpeed speed of the ghost
     * @param mapPos position of the ghost on the map
     * @param level current level of the game
     */

    Ghost(sf::Vector2i startPos, float moveSpeed, sf::Vector2f mapPos, int level);

    /**
     * @brief Destroy the Ghost object
     * 
     */
    virtual ~Ghost() = default;

    /**
     * @brief enum class for the different modes of the ghost
     * 
     */
    enum class Mode
    {
        Scatter,      // head for the ghost’s personal corner
        Chase,        // hunt Pac‑Man
        Frightened,   // blue / flashing, moves randomly
        Eaten         // TODO: eyes only, race back to the house
    };

    /**
     * @brief Set the Mode object
     * 
     * @param m mode of the ghost
     * @param forceImmediate boolean to force immediate mode change
     */
    void setMode(Mode m, bool forceImmediate = false);

    /**
     * @brief update the ghosts position based on the target tile
     * 
     * @param dt delta time
     * @param targetTile tile ghost targets
     */
    void update(sf::Time dt, const sf::Vector2i& targetTile);

    /**
     * @brief Set the Map object
     * 
     * @param map map of the game
     */
    void setMap(const std::vector<std::vector<int>>& map) { m_mapVector = map; }

    /**
     * @brief Get the Pos of ghost
     * 
     * @return sf::Vector2i position of the ghost
     */
    sf::Vector2i getPos() const { return m_pos; }

    /**
     * @brief Get the Mode object
     * 
     * @return Mode 
     */
    Mode getMode() const { return m_mode; }

    /**
     * @brief Get the Hit Box object
     * 
     * @return sf::FloatRect 
     */
    sf::FloatRect getHitBox() const { return m_animation.getGlobalBounds(); }

protected:

    /**
     * @brief initalize the ghosts textures and setup animation loops
     * 
     */
    virtual void initAnimation() = 0;

    /**
     * @brief returns if a candidate tile is valid
     * 
     * @param candidate 
     * @return true 
     * @return false 
     */
    bool isValidMove(sf::Vector2i& candidate) const;

    /**
     * @brief updates the ghosts animation rotation based on which way it is moving
     * 
     */
    void updateRotation();

    AnimatedSprite m_animation;
    const sf::Vector2i m_tileSize{ 24, 24 };
    std::vector<std::vector<int>> m_mapVector;

    int m_level;

    sf::Vector2i m_pos;
    sf::Vector2f m_mapPos;
    sf::Vector2i m_nextPos;
    sf::Vector2i m_currentDirection;

    bool m_isMoving;
    float m_moveProgress;
    float m_moveSpeed;
    float m_baseSpeed;

    Mode m_mode = Mode::Scatter;
    Mode m_queuedMode = Mode::Scatter;
    sf::Vector2i m_scatterTarget = { 0,0 };

    std::vector<sf::IntRect> m_framesUp;
    std::vector<sf::IntRect> m_framesRight;
    std::vector<sf::IntRect> m_framesDown;
    std::vector<sf::IntRect> m_framesLeft;

    std::vector<sf::IntRect> m_framesFrightened;   // blue animation frames
    std::vector<sf::IntRect> m_framesBlink;
    std::vector<sf::IntRect> m_framesEyes;         // (unused for now)

    // timing for Frightened mode
    sf::Time m_frightenedTotal;    // total blue duration
    sf::Time m_frightenedElapsed;  // elapsed time in frightened
    sf::Time m_blinkThreshold;     // when to switch to blink
    bool m_isBlinking;

    // home tile for Eaten mode
    sf::Vector2i m_homeTile{ 14,14 };

    // RNG for random frightened movement
    static std::mt19937 m_randEngine;
};

#endif // GHOST_H
