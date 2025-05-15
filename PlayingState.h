#ifndef PLAYINGSTATE_H
#define PLAYINGSTATE_H

#include "GameState.h"
#include "StateManager.h"
#include "AnimatedSprite.h"
#include "Player.h"
#include "Blinky.h"
#include "Pinky.h"
#include "Inky.h"
#include "Clyde.h"
#include <SFML/Graphics/Texture.hpp>
#include <vector>

class PlayingState : public GameState
{
public:
    /**
     * @brief PlayingState constructor
     * @param stateManager Reference to the StateManager
     * @param window Reference to the SFML window
     * @param lives Number of lives
     * @param level Current level
     * @param score Current score
     * @param mapVector 2D vector representing the map layout
     * @param blackTiles 2D vector representing the black tiles
     */
    PlayingState(StateManager& stateManager, const sf::RenderWindow& window, int lives = 3, int level = 1, int score = 0, std::vector<std::vector<int>> mapVector = {}, std::vector<std::vector<bool>> blackTiles = {});
    
    /**
     * @brief PlayingState destructor
     * @details Default destructor.
     */
    ~PlayingState() override = default;

    /**
     * @brief Handle events for the playing state
     * @param window Reference to the SFML window
     */
    void handleEvents(sf::RenderWindow& window) override;
    
    /**
     * @brief Update the playing state
     * @param dt Time since last update
     */
    void update(sf::Time dt) override;
    
    /**
     * @brief Render the playing state
     * @param window Reference to the SFML window
     */
    void render(sf::RenderWindow& window) override;

private:
    /**
     * @brief Initialize the map by analyzing the baked map texture
     * @param level Current level
     */
    void analyzeGridCells();

    /**
     * @brief manually override the map to get more accurate results
     * 
     */
    void manualOverrides();

    /**
     * @brief Update the map based on the current state
     * @param map 2D vector representing the map layout
     */
    void updateMap(std::vector<std::vector<int>>& map);

    /**
     * @brief Draw the black tiles on the map
     * @param window Reference to the SFML window
     */
    void drawBlackTiles(sf::RenderWindow& window);

    /**
     * @brief Initialize the score text
     */
    void initScoreText();

    /**
     * @brief Initialize the high score text from file
     */
    int readHighScore();

    /**
     * @brief Initialize the timers for the ghost modes
     * @param level Current level
     */
    void initModeTimers(int level);

    /**
     * @brief Initialize the player speeds based on the level
     * @param level Current level
     */
    void initPlayerSpeeds(int level);

    /**
     * @brief Check if the player has collided with a ghost
     * @return true if a ghost is not in frighten mode and collided with the player
     * @return false otherwise
     */
    bool checkDeath();

    /**
     * @brief initialize the possible pellets on the map
     */
    void initLevel();

    std::vector<std::vector<int>> m_mapVector;
    std::vector<std::vector<bool>> m_blackTiles;
    int  m_remainingPellets = 0;

    int m_level;
    int m_lives;

    int m_score;
    sf::Font m_scoreFont;
    sf::Text m_scoreText;
    sf::Text m_highScoreText;

    sf::Sprite m_pacSprite[3];
    sf::Texture m_pacTexture;

    sf::Texture m_mapTexture;
    sf::Sprite m_mapSprite;

    sf::Vector2i m_gridSpacing;
    sf::Vector2i m_offset;

    std::unique_ptr<Player> m_pacman;
    float m_normalSpeed;
    float m_eatingSpeed;

    std::unique_ptr<Blinky> m_blinky;
    std::unique_ptr<Pinky> m_pinky;
    std::unique_ptr<Inky> m_inky;
    std::unique_ptr<Clyde> m_clyde;

    sf::Clock m_timer;

    sf::Time m_scatterTimers[4]{};
    sf::Time m_chaseTimers[4]{};

    int m_phaseIndex = 0;
    bool m_inScatter = true;
    bool m_modeTimerActive = false;

    StateManager& m_stateManager;
    const sf::RenderWindow& m_window;
};


#endif // PLAYINGSTATE_H
