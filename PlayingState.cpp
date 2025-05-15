#include "PlayingState.h"

#include <SFML/Graphics/Rect.hpp>
#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/Texture.hpp>
#include <iostream>
#include <memory>
#include <string>
#include <utility>
#include <fstream>

#include "MainMenuState.h"
#include "Player.h"
#include "EndGameState.h"

PlayingState::PlayingState(StateManager& stateManager, const sf::RenderWindow& window, int lives, int level, int score, std::vector<std::vector<int>> mapVector, std::vector<std::vector<bool>> blackTiles)
    : m_level(level)
    , m_lives(lives)
    , m_score(score)
    , m_stateManager(stateManager)
    , m_window(window)
{
    // 0 = empty, 1 = wall, 2 = dot, 3 = big dot, 4 = ghost home, 5 = ghost gate!!!

    initScoreText();
    initModeTimers(level);
    initPlayerSpeeds(level);
    if (!m_mapTexture.loadFromFile("assets/map/map.png")) {
        std::cerr << "Failed to load map image: assets/map/map.png" << std::endl;
    }
    m_mapSprite.setTexture(m_mapTexture);

    // Get the window width and height
    sf::Vector2u windowSize = m_window.getSize();

    // Get the sprite's local bounds (original size)
    sf::FloatRect spriteBounds = m_mapSprite.getLocalBounds();

    // Calculate the scaling factor to fit the window's width while preserving aspect ratio
    float scaleFactor = static_cast<float>(windowSize.x) / spriteBounds.width;
    m_mapSprite.setScale(scaleFactor, scaleFactor);

    // Adjust the sprite's position so its bottom aligns with the bottom of the window
    float scaledHeight = spriteBounds.height * scaleFactor;
    m_mapSprite.setPosition(0.f, static_cast<float>(windowSize.y) - scaledHeight);

    // std::cout << "offsetx: " << m_mapSprite.getPosition().x << '\n';
    // std::cout << "offsety: " << m_mapSprite.getPosition().y << '\n';

    // Set up grid spacing and offset
    m_gridSpacing.x = 24.f;
    m_gridSpacing.y = 24.f;
    m_offset.x = 0.f;
    m_offset.y = 0.f;

    sf::Vector2f mapPos = m_mapSprite.getPosition();

    m_pacman = std::make_unique<Player>(sf::Vector2i{ 13, 23 }, mapPos);
    m_blinky = std::make_unique<Blinky>(sf::Vector2i{ 13, 14 }, m_normalSpeed, mapPos, level);
    m_pinky = std::make_unique<Pinky>(sf::Vector2i{ 12, 14 }, m_normalSpeed, mapPos, level);
    m_inky = std::make_unique<Inky>(sf::Vector2i{ 14, 14 }, m_normalSpeed, mapPos, level);
    m_clyde = std::make_unique<Clyde>(sf::Vector2i{ 15, 14 }, m_normalSpeed, mapPos, level);

    m_blinky->setMode(Ghost::Mode::Scatter);
    m_pinky->setMode(Ghost::Mode::Scatter);
    m_inky->setMode(Ghost::Mode::Scatter);
    m_clyde->setMode(Ghost::Mode::Scatter);

    if (!mapVector.empty()) {
        m_mapVector = std::move(mapVector);
        m_blackTiles = std::move(blackTiles);
    }
    else {
        analyzeGridCells();
        manualOverrides();
    }

    initLevel();

    m_timer.restart();
}

void PlayingState::handleEvents(sf::RenderWindow& window)
{
    if (window.isOpen()) {
        if ([&]() { static bool wasSpacePressed = false; bool isSpacePressed = sf::Keyboard::isKeyPressed(sf::Keyboard::Period); bool released = wasSpacePressed && !isSpacePressed; wasSpacePressed = isSpacePressed; return released; }()) {
            m_pinky->toggleDebug();
            m_blinky->toggleDebug();
            m_inky->toggleDebug();
            m_clyde->toggleDebug();
        }

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space)) {
            m_stateManager.changeState(std::make_unique<MainMenuState>(m_stateManager, m_window));
            return;
        }

        m_pacman->handleEvents();
    }
}

void PlayingState::update(sf::Time dt)
{
    if (!m_modeTimerActive) {
        if (m_timer.getElapsedTime().asSeconds() > 1.5f) {
            m_pacman->setMap(m_mapVector);
            m_blinky->setMap(m_mapVector);
            m_pinky->setMap(m_mapVector);
            m_inky->setMap(m_mapVector);
            m_clyde->setMap(m_mapVector);
            m_modeTimerActive = true;
            m_timer.restart();
        }
    }
    else {
        sf::Time phaseLimit = m_inScatter ? m_scatterTimers[m_phaseIndex] : m_chaseTimers[m_phaseIndex];

        if (phaseLimit != sf::Time::Zero && m_timer.getElapsedTime() >= phaseLimit) {
            m_inScatter = !m_inScatter;
            m_timer.restart();

            Ghost::Mode newMode = m_inScatter ? Ghost::Mode::Scatter : Ghost::Mode::Chase;

            m_blinky->setMode(newMode);
            m_pinky->setMode(newMode);
            m_inky->setMode(newMode);
            m_clyde->setMode(newMode);

            if (!m_inScatter && m_phaseIndex < 3) {
                ++m_phaseIndex;
            }
        }
    }

    m_blinky->update(dt, m_blinky->calculateTarget(m_pacman->getPlayerPos()));
    m_pinky->update(dt, m_pinky->calculateTarget(m_pacman->getPlayerPos(), m_pacman->getDirection()));
    m_inky->update(dt, m_inky->calculateTarget(m_pacman->getPlayerPos(), m_pacman->getDirection(), m_blinky->getPos()));
    m_clyde->update(dt, m_clyde->calculateTarget(m_pacman->getPlayerPos()));
    m_pacman->update(dt);
    updateMap(m_mapVector);

    if (checkDeath()) {
        sf::sleep(sf::seconds(1));
        if (m_lives == 1) {
            m_stateManager.changeState(std::make_unique<EndGameState>(m_stateManager, m_window, m_score));
        }
        else {
            m_stateManager.changeState(std::make_unique<PlayingState>(m_stateManager, m_window, --m_lives, m_level, m_score, m_mapVector, m_blackTiles));
        }
    }

    if (m_remainingPellets <= 0) {
        sf::sleep(sf::seconds(1));
        m_level++;
        m_stateManager.changeState(std::make_unique<PlayingState>(m_stateManager, m_window, m_lives, m_level, m_score));
    }
}

void PlayingState::render(sf::RenderWindow& window)
{
    // Draw the map sprite first.
    window.draw(m_mapSprite);

    drawBlackTiles(window);

    // draw pacman
    m_pacman->render(window);
    m_blinky->render(window);
    m_pinky->render(window);
    m_clyde->render(window, m_pacman->getPlayerPos());
    m_inky->render(window);

    for (auto& sprite : m_pacSprite) {
        window.draw(sprite);
    }

    window.draw(m_scoreText);
    window.draw(m_highScoreText);
}

void PlayingState::analyzeGridCells()
{
    // Get the image from the texture
    sf::Image mapImage = m_mapTexture.copyToImage();

    // Get sprite bounds and scale factor
    sf::FloatRect spriteBounds = m_mapSprite.getGlobalBounds();
    float scaleFactor = m_mapSprite.getScale().x; // assuming uniform scaling

    // Define effective grid boundaries with symmetric offset
    float leftBound = spriteBounds.left + (float)m_offset.x;
    float rightBound = spriteBounds.left + spriteBounds.width - (float)m_offset.x;
    float topBound = spriteBounds.top + (float)m_offset.y;
    float bottomBound = spriteBounds.top + spriteBounds.height - (float)m_offset.y;

    // Calculate how many columns & rows
    float gridWidth = rightBound - leftBound;
    float gridHeight = bottomBound - topBound;
    size_t numCols = static_cast<size_t>(gridWidth / (float)m_gridSpacing.x);
    size_t numRows = static_cast<size_t>(gridHeight / (float)m_gridSpacing.y);

    // Resize the map vector
    m_mapVector.clear();
    m_mapVector.resize(numRows, std::vector<int>(numCols, 0));

    m_blackTiles.clear();
    m_blackTiles.resize(m_mapVector.size());
    for (size_t row = 0; row < m_mapVector.size(); row++) {
        m_blackTiles[row].resize(m_mapVector[row].size(), false);
    }

    // We'll need the texture size as int to avoid sign warnings
    int texWidth = static_cast<int>(mapImage.getSize().x);
    int texHeight = static_cast<int>(mapImage.getSize().y);

    // Loop through each grid cell
    for (size_t i = 0; i < numRows; i++) {
        for (size_t j = 0; j < numCols; j++) {
            // Determine cell boundaries in screen space
            float cellLeft = leftBound + static_cast<float>(j) * (float)m_gridSpacing.x;
            float cellTop = topBound + static_cast<float>(i) * (float)m_gridSpacing.y;
            float cellWidth = (float)m_gridSpacing.x;
            float cellHeight = (float)m_gridSpacing.y;

            // Convert these coordinates to texture space
            int texCellLeft = static_cast<int>((cellLeft - spriteBounds.left) / scaleFactor);
            int texCellTop = static_cast<int>((cellTop - spriteBounds.top) / scaleFactor);
            int texCellWidth = static_cast<int>(cellWidth / scaleFactor);
            int texCellHeight = static_cast<int>(cellHeight / scaleFactor);

            // Clamp the region within the texture's bounds
            if (texCellLeft < 0) texCellLeft = 0;
            if (texCellTop < 0) texCellTop = 0;
            if (texCellLeft + texCellWidth > texWidth) {
                texCellWidth = texWidth - texCellLeft;
            }
            if (texCellTop + texCellHeight > texHeight) {
                texCellHeight = texHeight - texCellTop;
            }
            // Accumulate color
            unsigned long totalR = 0, totalG = 0, totalB = 0, totalA = 0;
            unsigned long count = 0;
            for (int y = texCellTop; y < texCellTop + texCellHeight; y++) {
                for (int x = texCellLeft; x < texCellLeft + texCellWidth; x++) {
                    sf::Color pixel = mapImage.getPixel(
                        static_cast<unsigned int>(x),
                        static_cast<unsigned int>(y)
                    );
                    totalR += pixel.r;
                    totalG += pixel.g;
                    totalB += pixel.b;
                    totalA += pixel.a;
                    count++;
                }
            }

            // Avoid division by zero
            if (count == 0) {
                m_mapVector[i][j] = 0;
                continue;
            }

            sf::Color avgColor(
                static_cast<sf::Uint8>(totalR / count),
                static_cast<sf::Uint8>(totalG / count),
                static_cast<sf::Uint8>(totalB / count),
                static_cast<sf::Uint8>(totalA / count)
            );

            // Classify the cell:
            // 1 = wall, 2 = small dot, 3 = big dot, 0 = empty
            const int whiteThreshold = 20;
            if (avgColor.b > avgColor.r && avgColor.b > avgColor.g) {
                // If average blue is dominant, mark as wall
                m_mapVector[i][j] = 1;
            }
            else if (avgColor.r > whiteThreshold && avgColor.g > whiteThreshold && avgColor.b > whiteThreshold) {
                // near white => big dot
                m_mapVector[i][j] = 3;
            }
            else if (avgColor.r > (whiteThreshold / 2) && avgColor.g > (whiteThreshold / 2) && avgColor.b > (whiteThreshold / 2)) {
                // fairly light => small dot
                m_mapVector[i][j] = 2;
            }
            else {
                // everything else => empty
                m_mapVector[i][j] = 0;
            }
        }
    }
}

void PlayingState::initLevel()
{
    m_remainingPellets = 0;
    // Count the number of pellets in the map
    for (int y = 0; y < (int)m_mapVector.size(); ++y) {
        for (std::size_t x = 0; x < m_mapVector[static_cast<std::size_t>(y)].size(); ++x) {
            int tile = m_mapVector[static_cast<std::size_t>(y)][x];
            // assuming 2 = small pellet, 3 = power pellet
            if (tile == 2 || tile == 3)
                ++m_remainingPellets;
        }
    }
}

void PlayingState::manualOverrides()
{
    // Mark ghost home region
    for (size_t i = 13; i < 16; i++) {
        for (size_t j = 11; j < 17; j++) {
            m_mapVector.at(i).at(j) = 4;
        }
    }

    // Mark ghost gates
    m_mapVector.at(12).at(13) = 5;
    m_mapVector.at(12).at(14) = 5;
}

// updateMap -- row = pos.y, col = pos.x
void PlayingState::updateMap(std::vector<std::vector<int>>& map)
{
    sf::Vector2i pos = m_pacman->getPlayerPos();

    // Ensure pos.x >= 0 and pos.y >= 0 if needed
    size_t row = static_cast<size_t>(pos.y);
    size_t col = static_cast<size_t>(pos.x);

    // Check if pellet is eaten
    if (map.at(row).at(col) == 2)
    {
        m_blackTiles.at(row).at(col) = true;
        map.at(row).at(col) = 0;
        m_score += 10;
        m_scoreText.setString("1UP\n " + std::to_string(m_score));
        --m_remainingPellets;
    }

    // Check if power pellet is eaten
    if (map.at(row).at(col) == 3)
    {
        m_blackTiles.at(row).at(col) = true;
        map.at(row).at(col) = 0;
        m_score += 50;
        m_blinky->setMode(Ghost::Mode::Frightened);
        m_inky->setMode(Ghost::Mode::Frightened);
        m_pinky->setMode(Ghost::Mode::Frightened);
        m_clyde->setMode(Ghost::Mode::Frightened);
        m_scoreText.setString("1UP\n " + std::to_string(m_score));
        --m_remainingPellets;
    }

    // slow down pacman when eating
    row = (size_t)m_pacman->getPlayerPos().y;
    col = (size_t)m_pacman->getPlayerPos().x;
    int dirX = m_pacman->getDirection().x;
    int dirY = m_pacman->getDirection().y;

    int mapHeight = static_cast<int>(map.size());
    int mapWidth = static_cast<int>(map[0].size());

    int targetCol = (int)col + dirX;
    int targetRow = (int)row + dirY;

    if (row >= 0 && (int)row < mapHeight && col > 0 && (int)col < mapWidth - 1 && targetCol >= 0 && targetCol < mapWidth) {

        int tile = map[(size_t)targetRow][(size_t)targetCol];

        if (tile == 2 || tile == 3) {
            m_pacman->setSpeed(m_eatingSpeed);
        }
        else {
            m_pacman->setSpeed(m_normalSpeed);
        }
    }
}

void PlayingState::drawBlackTiles(sf::RenderWindow& window)
{
    for (size_t row = 0; row < m_blackTiles.size(); row++) {
        for (size_t col = 0; col < m_blackTiles[row].size(); col++) {
            if (m_blackTiles[row][col]) {
                sf::RectangleShape blackRect(sf::Vector2f((float)m_gridSpacing.x + 2.f, (float)m_gridSpacing.y + 2.f));
                blackRect.setFillColor(sf::Color::Black);

                // Set the origin to the center of the rectangle
                float tileX = m_mapSprite.getPosition().x + (float)m_offset.x + (float)col * (float)m_gridSpacing.x - 1.f;
                float tileY = m_mapSprite.getPosition().y + (float)m_offset.y + (float)row * (float)m_gridSpacing.y - 1.f;

                // Set the position of the rectangle
                blackRect.setPosition(tileX, tileY);
                window.draw(blackRect);
            }
        }
    }
}

void PlayingState::initScoreText() {
    if (!m_scoreFont.loadFromFile("assets/fonts/PixelGaming.ttf"))
        std::cerr << "Failed to load score font.\n";

    // 1-UP (player score, top-left)
    m_scoreText.setFont(m_scoreFont);
    m_scoreText.setString("1UP\n " + std::to_string(m_score));
    m_scoreText.setCharacterSize(40);
    m_scoreText.setFillColor(sf::Color::White);
    m_scoreText.setPosition(10.f, 10.f);

    // HIGH (centered)
    int hi = readHighScore();
    m_highScoreText.setFont(m_scoreFont);
    m_highScoreText.setString("HIGH\n " + std::to_string(hi));
    m_highScoreText.setCharacterSize(40);
    m_highScoreText.setFillColor(sf::Color::White);

    // centre on the window
    float cx = static_cast<float>(m_window.getSize().x) / 2.f;
    sf::FloatRect b = m_highScoreText.getLocalBounds();
    m_highScoreText.setOrigin(b.left + b.width / 2.f, b.top);
    m_highScoreText.setPosition(cx, 10.f);

    // lives sprites …
    m_pacTexture.loadFromFile("assets/sprites/pacmanspritesheet.png");
    for (int i = 0; i < m_lives; i++) {
        m_pacSprite[i].setTexture(m_pacTexture);
        m_pacSprite[i].setTextureRect(sf::IntRect(13, 0, 13, 13));
        m_pacSprite[i].setScale(3.5f, 3.5f);
        m_pacSprite[i].setPosition(600.f - static_cast<float>(i) * 50.f, 35.f);
    }
}

int PlayingState::readHighScore()
{
    std::ifstream in("scores.txt");
    int best = 0;
    std::string line;
    while (std::getline(in, line)) {
        if (line.size() >= 4) {
            int val = std::stoi(line.substr(3));
            if (val > best) best = val;
        }
    }
    return best;
}

void PlayingState::initModeTimers(int level)
{
    const sf::Time ONE_FRAME = sf::seconds(1.f / 60.f);

    // Set the timers for each ghost
    // different levels have different timers
    if (level == 1) {
        m_scatterTimers[0] = sf::seconds(7.f);    m_chaseTimers[0] = sf::seconds(20.f);
        m_scatterTimers[1] = sf::seconds(7.f);    m_chaseTimers[1] = sf::seconds(20.f);
        m_scatterTimers[2] = sf::seconds(5.f);    m_chaseTimers[2] = sf::seconds(20.f);
        m_scatterTimers[3] = sf::seconds(5.f);    m_chaseTimers[3] = sf::Time::Zero;
    }
    else if (level >= 2 && level <= 4) {
        m_scatterTimers[0] = sf::seconds(7.f);    m_chaseTimers[0] = sf::seconds(20.f);
        m_scatterTimers[1] = sf::seconds(7.f);    m_chaseTimers[1] = sf::seconds(20.f);
        m_scatterTimers[2] = sf::seconds(5.f);    m_chaseTimers[2] = sf::seconds(1033.f);
        m_scatterTimers[3] = ONE_FRAME;           m_chaseTimers[3] = sf::Time::Zero;
    }
    else {
        m_scatterTimers[0] = sf::seconds(5.f);    m_chaseTimers[0] = sf::seconds(20.f);
        m_scatterTimers[1] = sf::seconds(5.f);    m_chaseTimers[1] = sf::seconds(20.f);
        m_scatterTimers[2] = sf::seconds(5.f);    m_chaseTimers[2] = sf::seconds(1037.f);
        m_scatterTimers[3] = ONE_FRAME;           m_chaseTimers[3] = sf::Time::Zero;
    }
}

void PlayingState::initPlayerSpeeds(int level)
{
    // Set the speed of Pac‑Man and the ghosts based on the level
    float baseSpeed = 7.5f; // base: tiles/sec
    float normalMultiplier = 1.0f;
    float eatingMultiplier = 0.9f;

    if (level == 1) {
        normalMultiplier = 0.8f;
        eatingMultiplier = 0.71f;
    }
    else if (level >= 2 && level <= 4) {
        normalMultiplier = 0.9f;
        eatingMultiplier = 0.79f;
    }
    else if (level >= 5 && level <= 20) {
        normalMultiplier = 1.0f;
        eatingMultiplier = 0.9f;
    }
    else if (level >= 21) {
        normalMultiplier = 1.1f;
        eatingMultiplier = 0.9f;
    }

    m_normalSpeed = baseSpeed * normalMultiplier;
    m_eatingSpeed = baseSpeed * eatingMultiplier;
}



bool PlayingState::checkDeath()
{
    sf::FloatRect pacBounds = m_pacman->getHitBox();
    float inset = 10.f;

    // Adjust the bounds to account for the inset gives original pacman feeling
    pacBounds = sf::FloatRect(
        pacBounds.left + inset,
        pacBounds.top + inset,
        pacBounds.width - inset * 2,
        pacBounds.height - inset * 2
    );

    // Inky
    if (m_inky->getMode() != Ghost::Mode::Eaten &&
        pacBounds.intersects(m_inky->getHitBox()))
    {
        if (m_inky->getMode() == Ghost::Mode::Frightened) {
            m_inky->setMode(Ghost::Mode::Eaten);
            m_score += 400;
            sf::sleep(sf::seconds(0.1f));
        }
        else {
            return true;  // kill Pac‑Man
        }
    }
    // Blinky
    else if (m_blinky->getMode() != Ghost::Mode::Eaten &&
        pacBounds.intersects(m_blinky->getHitBox()))
    {
        if (m_blinky->getMode() == Ghost::Mode::Frightened) {
            m_blinky->setMode(Ghost::Mode::Eaten);
            m_score += 400;
            sf::sleep(sf::seconds(0.2f));
        }
        else {
            return true;
        }
    }
    // Pinky
    else if (m_pinky->getMode() != Ghost::Mode::Eaten &&
        pacBounds.intersects(m_pinky->getHitBox()))
    {
        if (m_pinky->getMode() == Ghost::Mode::Frightened) {
            m_pinky->setMode(Ghost::Mode::Eaten);
            m_score += 400;
            sf::sleep(sf::seconds(0.2f));
        }
        else {
            return true;
        }
    }
    // Clyde
    else if (m_clyde->getMode() != Ghost::Mode::Eaten &&
        pacBounds.intersects(m_clyde->getHitBox()))
    {
        if (m_clyde->getMode() == Ghost::Mode::Frightened) {
            m_clyde->setMode(Ghost::Mode::Eaten);
            m_score += 400;
            sf::sleep(sf::seconds(0.2f));
        }
        else {
            return true;
        }
    }

    return false;
}