#ifndef MAINMENUSTATE_H
#define MAINMENUSTATE_H

#include "GameState.h"
#include "StateManager.h"
#include "AnimatedSprite.h"

#include <SFML/Graphics.hpp>
#include <vector>

class MainMenuState : public GameState {
public:
    /**
     * @brief MainMenuState constructor
     * @param stateManager Reference to the StateManager
     * @param window Reference to the SFML window
     */
    MainMenuState(StateManager& stateManager, const sf::RenderWindow& window);
    
    /**
     * @brief MainMenuState destructor
     * @details Default destructor.
     */
    ~MainMenuState() override = default;

    /**
     * @brief Handle events for the main menu state
     * @param window Reference to the SFML window
     */
    void handleEvents(sf::RenderWindow& window) override;
    
    /**
     * @brief Update the main menu state
     * @param dt Time since last update
     */
    void update(sf::Time dt) override;

    /**
     * @brief Render the main menu state
     * @param window Reference to the SFML window
     */
    void render(sf::RenderWindow& window) override;

private:

    /**
     * @brief initialize the animations for the menu
     * 
     */
    void initAnimations();

    StateManager& m_stateManager;
    sf::Font m_font;
    sf::Text m_menuText;

    sf::Text m_titleText;
    sf::Text m_playText;
    sf::Text m_exitText;

    AnimatedSprite m_pacMan;
    AnimatedSprite m_ghost;
    AnimatedSprite m_BlueGhost;

    sf::Vector2f m_velocity;

    const sf::RenderWindow& m_window;
};

#endif // MAINMENUSTATE_H
