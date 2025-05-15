#include "AnimatedSprite.h"

// Constructor initializes the frame time, loop flag, and other member variables.
AnimatedSprite::AnimatedSprite(sf::Time frameTime, bool loop)
    : m_frameTime(frameTime)
    , m_elapsedTime(sf::Time::Zero)
    , m_currentFrame(0)
    , m_isPlaying(false)
    , m_loop(loop)
{
}

bool AnimatedSprite::loadTexture(const std::string& filename) {
    return m_texture.loadFromFile(filename);
}

void AnimatedSprite::addFrame(const sf::IntRect& frame) {
    m_frames.push_back(frame);
}

void AnimatedSprite::setup() {
    // Assign the texture to the sprite and set the first frame.
    m_sprite.setTexture(m_texture);
    if (!m_frames.empty()) {
        m_sprite.setTextureRect(m_frames[0]);
    }
}

void AnimatedSprite::play() {
    m_isPlaying = true;
}

void AnimatedSprite::pause() {
    m_isPlaying = false;
}

void AnimatedSprite::stop() {
    m_isPlaying = false;
    m_currentFrame = 0;
    m_elapsedTime = sf::Time::Zero;
    if (!m_frames.empty())
        m_sprite.setTextureRect(m_frames[0]);
}

void AnimatedSprite::update(sf::Time dt) {
    if (!m_isPlaying || m_frames.empty())
        return;

    // Update the elapsed time and check if we need to switch frames.
    // If the animation is paused, do not update the current frame.
    // If the animation is stopped, reset the current frame.
    m_elapsedTime += dt;
    while (m_elapsedTime >= m_frameTime) {
        m_elapsedTime -= m_frameTime;
        if (m_currentFrame + 1 < m_frames.size()) {
            m_currentFrame++;
        }
        else {
            if (m_loop)
                m_currentFrame = 0;
            else {
                m_isPlaying = false;
                break;
            }
        }
        m_sprite.setTextureRect(m_frames[m_currentFrame]);
    }
}

void AnimatedSprite::setFrames(const std::vector<sf::IntRect>& frames) {
    m_frames = frames;
    m_currentFrame = 0;
    // Reset the elapsed time and set the first frame.
    m_elapsedTime = sf::Time::Zero;
    if (!m_frames.empty())
        m_sprite.setTextureRect(m_frames[0]);
}


sf::FloatRect AnimatedSprite::getLocalBounds() const {
    return m_sprite.getLocalBounds();
}

sf::FloatRect AnimatedSprite::getGlobalBounds() const {
    return getTransform().transformRect(m_sprite.getGlobalBounds());
}

void AnimatedSprite::draw(sf::RenderTarget& target, sf::RenderStates states) const {
    states.transform *= getTransform();
    target.draw(m_sprite, states);
}
