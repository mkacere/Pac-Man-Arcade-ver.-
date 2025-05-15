#include "Player.h"
#include "PlayingState.h"
#include <iostream>

Player::Player(sf::Vector2i startPos, sf::Vector2f mapPos)
    : m_gridPos(startPos)
    , m_targetPos(startPos)
    , m_mapPos(mapPos)
{
    // Initialize sprite animation
    initAnimations();

    m_moveSpeed = 0.f;
    // Position the sprite at the start tile
    m_animation.setPosition(
        static_cast<float>(startPos.x) * static_cast<float>(m_tileSize.x) + m_animation.getLocalBounds().width + m_mapPos.x,
        static_cast<float>(startPos.y) * static_cast<float>(m_tileSize.y) + m_animation.getLocalBounds().height + m_mapPos.y
    );

    m_animation.play();
}

void Player::handleEvents()
{
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::W) || sf::Keyboard::isKeyPressed(sf::Keyboard::Up)) {
        m_queuedDirection = { 0, -1 };
        //std::cout << "up is pressed" << '\n';
    }
    else if (sf::Keyboard::isKeyPressed(sf::Keyboard::S) || sf::Keyboard::isKeyPressed(sf::Keyboard::Down)) {
        m_queuedDirection = { 0, 1 };
        //std::cout << "Down is pressed" << '\n';
    }
    else if (sf::Keyboard::isKeyPressed(sf::Keyboard::A) || sf::Keyboard::isKeyPressed(sf::Keyboard::Left)) {
        m_queuedDirection = { -1, 0 };
        //std::cout << "Left is pressed" << '\n';
    }
    else if (sf::Keyboard::isKeyPressed(sf::Keyboard::D) || sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) {
        m_queuedDirection = { 1, 0 };
        //std::cout << "Right is pressed" << '\n';
    }

}

void Player::update(sf::Time dt)
{
    // Update animation frames
    m_animation.update(dt);

    // If we’re not currently moving between tiles.......
    if (!m_isMoving)
    {
        // Check if queued direction is valid; if so, include it
        if (isValidMove(m_gridPos + m_queuedDirection))
        {
            m_currentDirection = m_queuedDirection;
        }

        // Attempt to move one tile in the current direction
        sf::Vector2i newTarget = m_gridPos + m_currentDirection;
        if (isValidMove(newTarget))
        {
            m_targetPos = newTarget;
            m_isMoving = true;
            m_moveProgress = 0.f;
            updateRotation();
        }
    }

    // If we are moving between tiles, interpolate sprite position
    if (m_isMoving)
    {
        // Use dt.asSeconds() to scale movement by real time
        m_moveProgress += m_moveSpeed * dt.asSeconds();

        // Once we surpass 1.0, we’ve finished moving to the next tile
        if (m_moveProgress >= 1.0f)
        {
            m_gridPos = m_targetPos;
            m_moveProgress = 0.f;
            m_isMoving = false;

            // Wrap the player’s position if it went off the grid
            if (!m_mapVector.empty() && !m_mapVector[0].empty())
            {
                int mapWidth = static_cast<int>(m_mapVector[0].size());

                if (m_gridPos.x < 0) {
                    m_gridPos.x = mapWidth - 1;
                }
                else if (m_gridPos.x >= mapWidth) {
                    m_gridPos.x = 0;
                }
            }
        }

        sf::Vector2f offset(
            m_animation.getLocalBounds().width + m_mapPos.x,
            m_animation.getLocalBounds().height + m_mapPos.y
        );

        // Interpolate between the old tile (m_gridPos) and the new tile (m_targetPos)
        sf::Vector2f start(static_cast<float>(m_gridPos.x) * static_cast<float>(m_tileSize.x) + offset.x,
            static_cast<float>(m_gridPos.y) * static_cast<float>(m_tileSize.y) + offset.y);
        sf::Vector2f end(static_cast<float>(m_targetPos.x) * static_cast<float>(m_tileSize.x) + offset.x,
            static_cast<float>(m_targetPos.y) * static_cast<float>(m_tileSize.y) + offset.y);

        sf::Vector2f interpolatedPos = start + (end - start) * m_moveProgress;
        m_animation.setPosition(interpolatedPos);
    }
}

void Player::render(sf::RenderWindow& window)
{
    window.draw(m_animation);
}

void Player::setMap(const std::vector<std::vector<int>>& map)
{
    m_mapVector = map;
}

sf::Vector2i Player::getPlayerPos()
{
    return m_gridPos;
}

void Player::initAnimations()
{
    // Load the texture and set up the animation frames for the player
    m_animation = AnimatedSprite(sf::seconds(0.1f), true);

    if (!m_animation.loadTexture("assets/sprites/pacmanspritesheet.png")) {
        std::cerr << "Failed to load PacMan texture!" << std::endl;
    }

    const int frameSize = 13;
    for (int i = 0; i < 3; ++i) {
        m_animation.addFrame(sf::IntRect(frameSize * i, 0, frameSize, frameSize));
    }

    m_animation.addFrame(sf::IntRect(frameSize * 1, 0, frameSize, frameSize));

    m_animation.setup();

    // Center origin
    sf::FloatRect pacBounds = m_animation.getLocalBounds();
    m_animation.setOrigin(pacBounds.width / 2.f, pacBounds.height / 2.f);

    // Scale PacMan to be bigger
    float sizeScale = 2.4f;
    m_animation.setScale(sizeScale, sizeScale);
    m_animation.setRotation(180);
}

void Player::updateRotation()
{
    // Rotate sprite to match movement direction
    if (m_currentDirection == sf::Vector2i(1, 0)) {
        m_animation.setRotation(0);
    }
    else if (m_currentDirection == sf::Vector2i(-1, 0)) {
        m_animation.setRotation(180);
    }
    else if (m_currentDirection == sf::Vector2i(0, -1)) {
        m_animation.setRotation(270);
    }
    else if (m_currentDirection == sf::Vector2i(0, 1)) {
        m_animation.setRotation(90);
    }
}

bool Player::isValidMove(const sf::Vector2i& pos)
{
    // If the map is empty, return false
    if (m_mapVector.empty() || m_mapVector[0].empty()) {
        return false;
    }

    int mapWidth = static_cast<int>(m_mapVector[0].size());

    // Wrap 'pos' so it tunnels from one edge to the other
    sf::Vector2i wrappedPos = pos;

    if (wrappedPos.x < 0) {
        wrappedPos.x = mapWidth - 1;
    }
    else if (wrappedPos.x >= mapWidth) {
        wrappedPos.x = 0;
    }

    int tile = m_mapVector.at(static_cast<size_t>(wrappedPos.y))
        .at(static_cast<size_t>(wrappedPos.x));

    if (tile == 1 || tile == 4 || tile == 5) {
        return false;
    }

    return true;
}


