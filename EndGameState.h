#ifndef ENDGAMESTATE_H
#define ENDGAMESTATE_H

#include "GameState.h"
#include "StateManager.h"
#include <SFML/Graphics.hpp>
#include <utility>
#include <vector>

class EndGameState : public GameState
{
public:
    /**
     * @brief EndGameState constructor
     * @param manager Reference to the StateManager
     * @param window Reference to the SFML window
     * @param finalScore Final score of the game
     */
    EndGameState(StateManager& manager, const sf::RenderWindow& window, int finalScore);

    /**
     * @brief EndGameState destructor
     * @details Default destructor.
     */
    ~EndGameState() = default;

    /**
     * @brief EndGameState destructor
     * @details Default destructor.
     */
    void handleEvents(sf::RenderWindow& window) override;

    /**
     * @brief Updates the state
     * @param dt Time since last update
     */
    void update(sf::Time dt) override;

    /**
     * @brief Renders the state
     * @param window Reference to the SFML window
     */
    void render(sf::RenderWindow& window) override;

private:
    /**
     * @brief Sets the origin of the text to its center
     * @param text Reference to the SFML text
     */
    void centerOrigin(sf::Text& text);

    /**
     * @brief sets up the high score table
     * 
     */
    void buildTable();

    /**
     * @brief loads previous scores from file
     * 
     */
    void loadScores();

    /**
     * @brief saves current scores to file
     * 
     */
    void saveScores();

    /**
     * @brief Adds the current score to the high score table
     */
    void addCurrentScore();

    StateManager& m_manager;
    const sf::RenderWindow& m_window;
    int m_finalScore;
    bool m_scoreSubmitted = false;

    sf::Font m_titleFont;
    sf::Font m_monoFont;
    sf::Text m_titleText;
    sf::Text m_scoreText;
    sf::Text m_promptText;
    sf::Text m_nameText;
    std::vector<sf::Text> m_tableRows;

    std::vector<std::pair<std::string, int>> m_scores;  // name, value
    std::string m_playerName;
};

#endif  // ENDGAMESTATE_H
