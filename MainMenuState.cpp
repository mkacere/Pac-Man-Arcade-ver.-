#include "MainMenuState.h"
#include "PlayingState.h"
#include <iostream>

MainMenuState::MainMenuState(StateManager& stateManager, const sf::RenderWindow& window)
    : m_stateManager(stateManager),
    m_velocity(200.f, 0.f),
    m_window(window)
{
    // Load the font
    if (!m_font.loadFromFile("assets/fonts/pacfont.ttf")) {
        std::cerr << "Failed to load font!" << std::endl;
    }

    // Get window width for centering
    unsigned int windowWidth = m_window.getSize().x;

    // Configure Title Text
    m_titleText.setFont(m_font);
    m_titleText.setString("PacMan 1009:");
    m_titleText.setCharacterSize(75);
    {
        sf::FloatRect titleBounds = m_titleText.getLocalBounds();
        m_titleText.setOrigin(titleBounds.left + titleBounds.width / 2.f,
            titleBounds.top + titleBounds.height / 2.f);
        m_titleText.setPosition((float)windowWidth / 2.f, 100.f);
    }

    // Configure "Play" Text
    m_playText.setFont(m_font);
    m_playText.setString("Press Enter to Play");
    m_playText.setCharacterSize(40);
    {
        sf::FloatRect playBounds = m_playText.getLocalBounds();
        m_playText.setOrigin(playBounds.left + playBounds.width / 2.f,
            playBounds.top + playBounds.height / 2.f);
        m_playText.setPosition((float)windowWidth / 2.f, 200.f);
    }

    // Configure "Exit" Text
    m_exitText.setFont(m_font);
    m_exitText.setString("Press Esc to Exit");
    m_exitText.setCharacterSize(40);
    {
        sf::FloatRect exitBounds = m_exitText.getLocalBounds();
        m_exitText.setOrigin(exitBounds.left + exitBounds.width / 2.f,
            exitBounds.top + exitBounds.height / 2.f);
        m_exitText.setPosition((float)windowWidth / 2.f, 300.f);
    }

    initAnimations();
}

void MainMenuState::handleEvents(sf::RenderWindow& window) {
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Enter)) {
        m_stateManager.changeState(std::make_unique<PlayingState>(m_stateManager, m_window));
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Escape)) {
        window.close();
    }
}

void MainMenuState::update(sf::Time dt)
{
    unsigned int windowWidth = m_window.getSize().x;
    sf::FloatRect pacBounds = m_pacMan.getGlobalBounds();

    // When Pac-Man is moving left (chased by the regular ghost)
    if (m_velocity.x < 0)
    {
        m_pacMan.move(m_velocity * dt.asSeconds());
        m_ghost.move(m_velocity * dt.asSeconds());
        m_pacMan.update(dt);
        m_ghost.update(dt);

        // When Pac-Man has completely gone off the left edge:
        if (pacBounds.left + pacBounds.width + 200 < 0)
        {
            // Reposition Pac-Man to the left edge (off-screen left)
            // m_pacMan.setPosition (-pacBounds.width / 2, m_pacMan.getPosition().y);
            // Change direction to move right
            m_velocity.x = std::abs(m_velocity.x);

            // Set blue ghost to chase Pac-Man from behind.
            m_BlueGhost.setPosition(m_pacMan.getPosition().x + 125, m_pacMan.getPosition().y);

            // Ensure both Pac-Man and blue ghost face right.
            if (m_pacMan.getScale().x < 0)
                m_pacMan.setScale(std::abs(m_pacMan.getScale().x), m_pacMan.getScale().y);
            if (m_BlueGhost.getScale().x < 0)
                m_BlueGhost.setScale(std::abs(m_BlueGhost.getScale().x), m_BlueGhost.getScale().y);
        }
    }

    // When Pac-Man is moving right (chasing the blue ghost)
    else if (m_velocity.x > 0)
    {
        m_pacMan.move(m_velocity * dt.asSeconds());
        m_BlueGhost.move(m_velocity * dt.asSeconds());
        m_pacMan.update(dt);
        m_BlueGhost.update(dt);

        // When Pac-Man has completely gone off the right edge:
        if (pacBounds.left - 100 > (float)windowWidth)
        {
            // Reposition Pac-Man to the right edge (off-screen right)
            m_pacMan.setPosition((float)windowWidth + pacBounds.width / 2, m_pacMan.getPosition().y);
            // Change direction to move left
            m_velocity.x = -std::abs(m_velocity.x);

            // Set the regular ghost to chase Pac-Man from behind.
            m_ghost.setPosition(m_pacMan.getPosition().x + 125, m_pacMan.getPosition().y);

            // Ensure both Pac-Man and the regular ghost face left.
            if (m_pacMan.getScale().x > 0)
                m_pacMan.setScale(-std::abs(m_pacMan.getScale().x), m_pacMan.getScale().y);
            if (m_ghost.getScale().x > 0)
                m_ghost.setScale(-std::abs(m_ghost.getScale().x), m_ghost.getScale().y);
        }
    }
}

void MainMenuState::render(sf::RenderWindow& window) {
    window.draw(m_titleText);
    window.draw(m_playText);
    window.draw(m_exitText);
    window.draw(m_pacMan);
    window.draw(m_ghost);
    window.draw(m_BlueGhost);
}

void MainMenuState::initAnimations()
{
    unsigned int windowWidth = m_window.getSize().x;
    int heightOfAnimation = 600;
    const int sizeScale = 5;


    m_pacMan = AnimatedSprite(sf::seconds(0.1f), true);
    if (!m_pacMan.loadTexture("assets/sprites/pacmanspritesheet.png")) {
        std::cerr << "Failed to load PacMan texture!" << std::endl;
    }
    const int frameSizePac = 13;
    for (int i = 0; i < 3; i++) {
        m_pacMan.addFrame(sf::IntRect(frameSizePac * i, 0, frameSizePac, frameSizePac));
    }
    m_pacMan.addFrame(sf::IntRect(frameSizePac * 1, 0, frameSizePac, frameSizePac));

    m_pacMan.setup();
    sf::FloatRect pacBounds = m_pacMan.getLocalBounds();
    // Set origin to center so transformations occur around the middle
    m_pacMan.setOrigin(pacBounds.width / 2, pacBounds.height / 2);
    m_pacMan.setScale(-sizeScale, sizeScale);
    // Start off-screen to the right (using half the width so it appears fully off)
    m_pacMan.setPosition((float)windowWidth + pacBounds.width / 2, (float)heightOfAnimation);
    m_pacMan.play();

    m_ghost = AnimatedSprite(sf::seconds(0.15f), true);
    if (!m_ghost.loadTexture("assets/sprites/pacmanspritesheet.png")) {
        std::cerr << "Failed to load Ghost texture!" << std::endl;
    }
    const int frameSizeGhost = 14;
    for (int i = 0; i < 2; i++) {
        m_ghost.addFrame(sf::IntRect(frameSizeGhost * i, 13, frameSizeGhost, frameSizeGhost));
    }
    m_ghost.setup();
    sf::FloatRect ghostBounds = m_ghost.getLocalBounds();
    m_ghost.setOrigin(ghostBounds.width / 2, ghostBounds.height / 2);
    // Face left (flip horizontally) so it chases Pac-Man coming from the right.
    m_ghost.setScale(-sizeScale, sizeScale);
    // Position ghost behind Pac-Man (to his right) when he starts moving left.
    m_ghost.setPosition(m_pacMan.getPosition().x + 125, (float)heightOfAnimation);
    m_ghost.play();

    m_BlueGhost = AnimatedSprite(sf::seconds(0.15f), true);
    if (!m_BlueGhost.loadTexture("assets/sprites/pacmanspritesheet.png")) {
        std::cerr << "Failed to load BlueGhost texture!" << std::endl;
    }
    for (int i = 0; i < 2; i++) {
        m_BlueGhost.addFrame(sf::IntRect(frameSizeGhost * i, 13 + 4 * frameSizeGhost, frameSizeGhost, frameSizeGhost));
    }
    m_BlueGhost.setup();
    sf::FloatRect blueGhostBounds = m_BlueGhost.getLocalBounds();
    m_BlueGhost.setOrigin(blueGhostBounds.width / 2, blueGhostBounds.height / 2);
    m_BlueGhost.setScale(sizeScale, sizeScale); // Not flipped, so faces right.
    // Place blue ghost off-screen to the left.
    m_BlueGhost.setPosition(-blueGhostBounds.width / 2 - 30, (float)heightOfAnimation);
    m_BlueGhost.play();

    // Start with Pac-Man moving left (i.e. being chased by the regular ghost)
    m_velocity.x = -200;
}
