#include "Clyde.h"

#include <SFML/System/Vector2.hpp>
#include <iostream>

Clyde::Clyde(sf::Vector2i startPos, float moveSpeed, sf::Vector2f mapPos, int level) :
    Ghost(startPos, moveSpeed, mapPos, level) // Call Ghost constructor to set m_pos, m_moveSpeed, etc.
{
    // Initialize sprite animation
    initAnimation();

    // Position the sprite at the start tile
    m_animation.setPosition(
        (float)(startPos.x * m_tileSize.x) + m_animation.getLocalBounds().width + mapPos.x,
        (float)(startPos.y * m_tileSize.y) + m_animation.getLocalBounds().height + mapPos.y
    );

}

sf::Vector2i Clyde::calculateTarget(sf::Vector2i playerPos)
{
    // Define the threshold distance (in tiles) at which Clyde switches behavior.
    const float threshold = 8.0f;

    // Compute the Euclidean distance from Clyde's current position (m_pos) to Pac-Man's position.
    float dx = static_cast<float>(playerPos.x - m_pos.x);
    float dy = static_cast<float>(playerPos.y - m_pos.y);
    float distance = std::sqrt(dx * dx + dy * dy);

    // If Clyde is farther than the threshold from Pac-Man, his target is Pac-Man's position.
    if (distance > threshold) {
        m_targetTile = playerPos;
    }
    else {
        int mapHeight = static_cast<int>(m_mapVector.size());
        sf::Vector2i scatterCorner(1, mapHeight - 1);
        m_targetTile = scatterCorner;
    }

    if (m_mode == Ghost::Mode::Scatter) {
        m_targetTile = { 1, 30 };
    }

    return m_targetTile;
}


void Clyde::render(sf::RenderWindow& window, sf::Vector2i pacPos)
{
    // Draw Clyde's animation.
    window.draw(m_animation);

    if (m_debug)
    {
        float tileSize = (float)m_tileSize.x;
        // Only draw the debug "X" if the target tile is different from the player's tile.
        if (m_targetTile != pacPos)
        {
            // --- Draw an "X" at the target tile for debugging ---
            sf::Vector2f targetTileCenter(
                (float)m_targetTile.x * tileSize + tileSize / 2.f + m_mapPos.x,
                (float)m_targetTile.y * tileSize + tileSize / 2.f + m_mapPos.y
            );

            float span = 0.6f * (float)std::min(m_tileSize.x, m_tileSize.y); // 60% of tile size.
            float thickness = 4.f; // 4 pixels thick.

            sf::RectangleShape line1(sf::Vector2f(span, thickness));
            line1.setOrigin(span / 2.f, thickness / 2.f);
            line1.setPosition(targetTileCenter);
            line1.setRotation(45.f);
            line1.setFillColor(sf::Color(255, 165, 0, 200)); // semi-transparent orange

            sf::RectangleShape line2(sf::RectangleShape(sf::Vector2f(span, thickness)));
            line2.setOrigin(span / 2.f, thickness / 2.f);
            line2.setPosition(targetTileCenter);
            line2.setRotation(-45.f);
            line2.setFillColor(sf::Color(255, 165, 0, 200));

            window.draw(line1);
            window.draw(line2);
        }

        // --- Draw a ring around Pac-Man, but only if the target is not the scatter corner ---
        if (!m_mapVector.empty())
        {
            int mapHeight = static_cast<int>(m_mapVector.size());
            sf::Vector2i scatterCorner(1, mapHeight - 1);
            // Only draw the ring if Clyde's current target is not the scatter corner.
            if (m_targetTile != scatterCorner)
            {
                // Calculate Pac-Man's center in world coordinates:
                sf::Vector2f pacCenter(
                    (float)pacPos.x * tileSize + tileSize / 2.f + m_mapPos.x,
                    (float)pacPos.y * tileSize + tileSize / 2.f + m_mapPos.y
                );

                // Use Clyde's threshold, e.g. 8 tiles (adjust as needed).
                const float threshold = 8.0f;
                float radius = threshold * tileSize; // Assuming square tiles.

                sf::CircleShape ring(radius);
                ring.setFillColor(sf::Color::Transparent);
                ring.setOutlineThickness(3.f);
                ring.setOutlineColor(sf::Color(255, 165, 0, 200));

                // Center the ring on pacCenter.
                ring.setOrigin(radius, radius);
                ring.setPosition(pacCenter);

                window.draw(ring);
            }
        }
    }
}


void Clyde::initAnimation()
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
    m_framesRight.push_back(sf::IntRect(0, 13 + 14 * 2, 14, 14));
    m_framesRight.push_back(sf::IntRect(14, 13 + 14 * 2, 14, 14));

    m_framesUp.push_back(sf::IntRect(14 * 2, 13 + 14 * 2, 14, 14));
    m_framesUp.push_back(sf::IntRect(14 * 3, 13 + 14 * 2, 14, 14));

    m_framesDown.push_back(sf::IntRect(14 * 4, 13 + 14 * 2, 14, 14));
    m_framesDown.push_back(sf::IntRect(14 * 5, 13 + 14 * 2, 14, 14));

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
