#include "Blinky.h"

#include <iostream>
#include <string>

Blinky::Blinky(sf::Vector2i startPos, float moveSpeed, sf::Vector2f mapPos, int level) :
    Ghost(startPos, moveSpeed, mapPos, level) // Call Ghost constructor to set m_pos, m_moveSpeed, etc.
{
    // Initialize sprite animation
    initAnimation();

    // Position the sprite at the start tile
    m_animation.setPosition(
        static_cast<float>(startPos.x) * static_cast<float>(m_tileSize.x) + m_animation.getLocalBounds().width + mapPos.x,
        static_cast<float>(startPos.y) * static_cast<float>(m_tileSize.y) + m_animation.getLocalBounds().height + mapPos.y
    );

    m_scatterTarget = { 24, 0 };
}

sf::Vector2i Blinky::calculateTarget(sf::Vector2i playerPos)
{
    m_targetTile = playerPos;

    if (m_mode == Ghost::Mode::Scatter) {
        m_targetTile = { 26, 0 };
    }

    return m_targetTile;
}

void Blinky::render(sf::RenderWindow& window)
{
    window.draw(m_animation);

    if (m_debug) {
        sf::Vector2f tileCenter(
            static_cast<float>(m_targetTile.x) * static_cast<float>(m_tileSize.x) + static_cast<float>(m_tileSize.x) / 2.f + m_mapPos.x,
            static_cast<float>(m_targetTile.y) * static_cast<float>(m_tileSize.y) + static_cast<float>(m_tileSize.y) / 2.f + m_mapPos.y
        );

        const float span = 0.6f * static_cast<float>(std::min(m_tileSize.x, m_tileSize.y)); // 60% of tile size.
        const float thickness = 4.f;                                                             // 4 pixels thick.

        // Create first line: rectangle spanning 'span' with the specified thickness.
        sf::RectangleShape line1(sf::Vector2f(span, thickness));
        // Center the rectangle's origin.
        line1.setOrigin(span / 2.f, thickness / 2.f);
        line1.setPosition(tileCenter);
        // Rotate 45 degrees.
        line1.setRotation(45.f);
        // Set color (semi‑transparent red).
        line1.setFillColor(sf::Color(255, 0, 0, 200));

        // Create second line: same dimensions.
        sf::RectangleShape line2(sf::Vector2f(span, thickness));
        line2.setOrigin(span / 2.f, thickness / 2.f);
        line2.setPosition(tileCenter);
        // Rotate –45 degrees.
        line2.setRotation(-45.f);
        line2.setFillColor(sf::Color(255, 0, 0, 200));

        // Draw both lines.
        window.draw(line1);
        window.draw(line2);
    }
}


void Blinky::initAnimation()
{
    // Create a new AnimatedSprite (with a frame time of 0.1 sec and looping enabled)
    m_animation = AnimatedSprite(sf::seconds(0.1f), true);

    if (!m_animation.loadTexture("assets/sprites/pacmanspritesheet.png"))
    {
        std::cerr << "Failed to load PacMan texture!" << std::endl;
    }

    // Clear any previous frame sets.
    m_framesRight.clear();
    m_framesUp.clear();
    m_framesDown.clear();
    m_framesLeft.clear();

    // Populate frame sets for different directions.
    m_framesRight.push_back(sf::IntRect(0, 13, 14, 14));
    m_framesRight.push_back(sf::IntRect(14, 13, 14, 14));

    m_framesUp.push_back(sf::IntRect(14 * 2, 13, 14, 14));
    m_framesUp.push_back(sf::IntRect(14 * 3, 13, 14, 14));

    m_framesDown.push_back(sf::IntRect(14 * 4, 13, 14, 14));
    m_framesDown.push_back(sf::IntRect(14 * 5, 13, 14, 14));

    // For left, we use the right frames and flip them later.
    // Set default frames (using right frames)
    m_animation.addFrame(m_framesRight[0]);
    m_animation.addFrame(m_framesRight[1]);

    // Set a scale to get desired size.
    m_animation.setScale(2.5f, 2.5f);

    // Setup the animation (which applies the frames)
    m_animation.setup();

    // Set the sprite's origin to its center.
    sf::FloatRect bounds = m_animation.getLocalBounds();
    m_animation.setOrigin(bounds.width / 2.f, bounds.height / 2.f);

    m_animation.play();
}
