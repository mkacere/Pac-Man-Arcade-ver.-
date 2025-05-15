#include "Game.h"
#include "StateManager.h"
#include "MainMenuState.h"

#include <iostream>
#include <SFML/System/Clock.hpp>

Game::Game() : m_window(sf::VideoMode(224 * 3, 288 * 3), "PacMan ~ maxkacere", sf::Style::Titlebar | sf::Style::Close)
{
    m_stateManager.pushState(std::make_unique<MainMenuState>(m_stateManager, m_window));
}

void Game::run()
{
    sf::Clock clock;
    while (m_window.isOpen()) {
        // Process window events.
        sf::Event event;
        while (m_window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                m_window.close();
        }

        // Calculate time since last frame.
        sf::Time dt = clock.restart();

        // Handle events using the current state.
        if (auto state = m_stateManager.getCurrentState()) {
            state->handleEvents(m_window);
        }

        // Re-fetch the state after handling events,
        // so if a state change occurred, update the new state.
        if (auto state = m_stateManager.getCurrentState()) {
            state->update(dt);
        }

        m_window.clear();

        // Re-check the current state again before rendering.
        if (auto state = m_stateManager.getCurrentState()) {
            state->render(m_window);
        }

        m_window.display();
    }
}

