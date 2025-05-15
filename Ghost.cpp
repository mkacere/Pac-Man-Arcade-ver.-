#include "Ghost.h"

#include <SFML/System/Vector2.hpp>
#include <algorithm>
#include <random>
#include <cstddef>

// static RNG definition
std::mt19937 Ghost::m_randEngine{ std::random_device{}() };

Ghost::Ghost(sf::Vector2i startPos, float moveSpeed, sf::Vector2f mapPos, int level)
    : m_level(level),
    m_pos(startPos),
    m_mapPos(mapPos),
    m_nextPos(startPos),
    m_currentDirection(0, 0),
    m_isMoving(false),
    m_moveProgress(0.f),
    m_baseSpeed(moveSpeed),
    m_isBlinking(false),  // initialize blinking flag
    m_homeTile(startPos)
{
    m_moveSpeed = m_baseSpeed;

    // initialize frightened timer & blink thresholds
    m_frightenedTotal = sf::seconds(8.f);
    m_blinkThreshold = sf::seconds(6.f);
    m_frightenedElapsed = sf::Time::Zero;

    // define universal frightened/white frames
    int y = 13 + 14 * 4;
    m_framesFrightened = {
        sf::IntRect(0, y, 14, 14),
        sf::IntRect(14, y, 14, 14),
    };
    m_framesBlink = {
        sf::IntRect(28, y, 14, 14),
        sf::IntRect(42, y, 14, 14),
    };

    // eyes frames to be set elsewhere

    // center animation at spawn tile
    sf::Vector2f offset(
        static_cast<float>(m_tileSize.x) / 2.f + m_mapPos.x,
        static_cast<float>(m_tileSize.y) / 2.f + m_mapPos.y);
    m_animation.setPosition(
        static_cast<float>(m_pos.x) * static_cast<float>(m_tileSize.x) + offset.x,
        static_cast<float>(m_pos.y) * static_cast<float>(m_tileSize.y) + offset.y);
}

void Ghost::setMode(Mode mode, bool forceImmediate) {
    if (!forceImmediate
        && (m_mode == Mode::Frightened || m_mode == Mode::Eaten)
        && (mode == Mode::Chase || mode == Mode::Scatter))
    {
        m_queuedMode = mode;
        return;
    }

    m_mode = mode;
    float multiplier = 1.0f;
    switch (mode) {
    case Mode::Scatter:
    case Mode::Chase:
        m_animation.stop();
        initAnimation();
        m_animation.setup();
        m_animation.play();
        break;
    case Mode::Frightened:
        m_frightenedElapsed = sf::Time::Zero;
        m_isBlinking = false;
        m_animation.setScale(
            std::abs(m_animation.getScale().x),
            m_animation.getScale().y
        );

        m_animation.stop();
        m_animation.setFrames(m_framesFrightened);
        m_animation.setup();
        m_animation.play();
        break;
    case Mode::Eaten:
        m_animation.setScale(
            std::abs(m_animation.getScale().x),
            m_animation.getScale().y
        );

        m_animation.stop();
        m_animation.setFrames(m_framesEyes);
        m_animation.setup();
        m_animation.play();
        break;
    }

    sf::Vector2f offset(
        static_cast<float>(m_tileSize.x) / 2.f + m_mapPos.x,
        static_cast<float>(m_tileSize.y) / 2.f + m_mapPos.y);
    m_animation.setPosition(
        static_cast<float>(m_pos.x) * static_cast<float>(m_tileSize.x) + offset.x,
        static_cast<float>(m_pos.y) * static_cast<float>(m_tileSize.y) + offset.y);

    switch (mode) {
    case Mode::Chase:
    case Mode::Scatter:
        if (m_level == 1) {
            multiplier = 0.75f;
        }
        else if (m_level <= 4) {
            multiplier = 0.85f;
        }
        else {
            multiplier = 0.95f;
        }
        break;
    case Mode::Frightened:
        if (m_level == 1) {
            multiplier = 0.5f;
        }
        else if (m_level <= 4) {
            multiplier = 0.55f;
        }
        else {
            multiplier = 0.6f;
        }
        break;
    case Mode::Eaten:
        multiplier = 2.0f;
        break;
    }
    m_moveSpeed = m_baseSpeed * multiplier;
}

void Ghost::update(sf::Time dt, const sf::Vector2i& targetTile) {
    m_animation.update(dt);

    // mode transitions
    if (m_mode == Mode::Frightened) {
        m_frightenedElapsed += dt;
        if (m_frightenedElapsed >= m_frightenedTotal) {
            setMode(m_queuedMode, true);
        }
        else if (!m_isBlinking && m_frightenedElapsed >= m_blinkThreshold) {
            m_isBlinking = true;

            // combine blue and blink frames
            std::vector<sf::IntRect> blinkAnim;
            blinkAnim.reserve(
                m_framesFrightened.size() + m_framesBlink.size());
            blinkAnim.insert(
                blinkAnim.end(),
                m_framesFrightened.begin(), m_framesFrightened.end());
            blinkAnim.insert(
                blinkAnim.end(),
                m_framesBlink.begin(), m_framesBlink.end());

            m_animation.stop();
            m_animation.setFrames(blinkAnim);
            m_animation.setup();
            m_animation.play();

            sf::Vector2f blinkOffset(
                static_cast<float>(m_tileSize.x) / 2.f + m_mapPos.x,
                static_cast<float>(m_tileSize.y) / 2.f + m_mapPos.y);
            m_animation.setPosition(
                static_cast<float>(m_pos.x) * static_cast<float>(m_tileSize.x) + blinkOffset.x,
                static_cast<float>(m_pos.y) * static_cast<float>(m_tileSize.y) + blinkOffset.y);
        }
    }

    if (m_mode == Mode::Eaten && !m_isMoving && m_pos == m_homeTile) {
        setMode(m_queuedMode, true);
    }

    // decide mapping for tunnel warp
    sf::Vector2i localTarget = targetTile;
    if (!m_mapVector.empty()
        && m_pos.y >= 0
        && m_pos.y < static_cast<int>(m_mapVector.size())
        && m_pos.x >= 0
        && m_pos.x < static_cast<int>(m_mapVector[static_cast<std::size_t>(m_pos.y)].size())
        && m_mapVector[static_cast<std::size_t>(m_pos.y)][static_cast<std::size_t>(m_pos.x)] == 4)
    {
        localTarget = { 14, 0 };
    }
    if (m_mode == Mode::Eaten) {
        localTarget = m_homeTile;
    }

    // movement decision
    if (!m_isMoving) {
        std::vector<sf::Vector2i> dirs = { {0, -1}, {-1, 0}, {0, 1}, {1, 0} };
        if (m_currentDirection != sf::Vector2i{ 0, 0 }) {
            dirs.erase(
                std::remove(dirs.begin(), dirs.end(), -m_currentDirection),
                dirs.end());
        }

        sf::Vector2i bestDir{ 0, 0 };
        if (m_mode == Mode::Frightened) {
            std::shuffle(dirs.begin(), dirs.end(), m_randEngine);
            for (auto& d : dirs) {
                sf::Vector2i cand = m_pos + d;
                if (isValidMove(cand)) {
                    bestDir = d;
                    break;
                }
            }
        }
        else {
            int bestDist = std::numeric_limits<int>::max();
            for (auto& d : dirs) {
                if (d == sf::Vector2i{ 0, -1 } && m_pos.y == 23 && m_pos.x >= 10 && m_pos.x <= 16) {
                    continue;
                }
                sf::Vector2i cand = m_pos + d;
                if (!isValidMove(cand)) {
                    continue;
                }
                int dx = cand.x - localTarget.x;
                int dy = cand.y - localTarget.y;
                int dist2 = dx * dx + dy * dy;
                if (dist2 < bestDist) {
                    bestDist = dist2;
                    bestDir = d;
                }
            }
        }
        if (bestDir != sf::Vector2i{ 0, 0 }) {
            m_currentDirection = bestDir;
            m_nextPos = m_pos + bestDir;
            m_isMoving = true;
            m_moveProgress = 0.f;
            updateRotation();
        }
    }

    // movement interpolation + snapping
    sf::Vector2f offset(
        static_cast<float>(m_tileSize.x) / 2.f + m_mapPos.x,
        static_cast<float>(m_tileSize.y) / 2.f + m_mapPos.y);
    if (m_isMoving) {
        m_moveProgress += m_moveSpeed * dt.asSeconds();
        if (m_moveProgress >= 1.f) {
            m_pos = m_nextPos;
            m_moveProgress = 0.f;
            m_isMoving = false;
        }
        sf::Vector2f start(
            static_cast<float>(m_pos.x) * static_cast<float>(m_tileSize.x) + offset.x,
            static_cast<float>(m_pos.y) * static_cast<float>(m_tileSize.y) + offset.y);
        sf::Vector2f end(
            static_cast<float>(m_nextPos.x) * static_cast<float>(m_tileSize.x) + offset.x,
            static_cast<float>(m_nextPos.y) * static_cast<float>(m_tileSize.y) + offset.y);
        sf::Vector2f interp = start + (end - start) * m_moveProgress;
        m_animation.setPosition(interp);
    }
    else {
        m_animation.setPosition(sf::Vector2f(
            static_cast<float>(m_pos.x) * static_cast<float>(m_tileSize.x) + offset.x,
            static_cast<float>(m_pos.y) * static_cast<float>(m_tileSize.y) + offset.y));
    }
}

bool Ghost::isValidMove(sf::Vector2i& candidate) const {
    if (candidate.y < 0
        || candidate.y >= static_cast<int>(m_mapVector.size())) {
        return false;
    }
    if (candidate.x < 0
        || candidate.x >= static_cast<int>(m_mapVector[0].size())) {
        return false;
    }
    int mapWidth = static_cast<int>(m_mapVector[0].size());
    if (candidate.y == 14
        && (candidate.x == 5 || candidate.x == mapWidth - 6)) {
        return false;
    }
    int tile = m_mapVector[static_cast<std::size_t>(candidate.y)][static_cast<std::size_t>(candidate.x)];
    if (tile == 1) {
        return false;
    }
    if (tile == 5) {
        if (m_mode == Mode::Eaten) {
            // allow leaving home
        }
        else if (
            m_mapVector[static_cast<std::size_t>(m_pos.y)][static_cast<std::size_t>(m_pos.x)] != 4) {
            return false;
        }
    }
    return true;
}

void Ghost::updateRotation() {
    if (m_mode == Mode::Frightened) {
        return;
    }
    float curX = std::abs(m_animation.getScale().x);
    float curY = m_animation.getScale().y;
    const std::vector<sf::IntRect>* frames = nullptr;
    if (m_mode == Mode::Eaten) {
        frames = &m_framesEyes;
    }
    else if (m_currentDirection == sf::Vector2i{ 0, 1 }) {
        frames = &m_framesUp;
    }
    else if (m_currentDirection == sf::Vector2i{ 1, 0 }) {
        frames = &m_framesRight;
        m_animation.setScale(curX, curY);
    }
    else if (m_currentDirection == sf::Vector2i{ 0, -1 }) {
        frames = &m_framesDown;
    }
    else if (m_currentDirection == sf::Vector2i{ -1, 0 }) {
        frames = &m_framesRight;
        m_animation.setScale(-curX, curY);
    }

    if (frames && !frames->empty()) {
        m_animation.stop();
        m_animation.setFrames(*frames);
        m_animation.setup();
        m_animation.play();
    }
}
