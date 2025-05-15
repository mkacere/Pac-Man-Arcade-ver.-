#ifndef ANIMATEDSPRITE_H
#define ANIMATEDSPRITE_H

#include <SFML/Graphics.hpp>
#include <vector>
#include <string>

class AnimatedSprite : public sf::Drawable, public sf::Transformable {
public:
    // frameTime: time each frame is shown; loop: whether to loop the animation.
    AnimatedSprite(sf::Time frameTime = sf::seconds(0.1f), bool loop = true);

    // Load the sprite sheet texture.
    bool loadTexture(const std::string& filename);

    // Add a frame (an sf::IntRect) to the animation.
    void addFrame(const sf::IntRect& frame);

    // Must be called after adding frames; assigns the texture and first frame to the sprite.
    void setup();

    // Control playback.
    void play();
    void pause();
    void stop();

    // Update the animation. Call this each frame.
    void update(sf::Time dt);

    void setFrames(const std::vector<sf::IntRect>& frames);

    // Optionally, get bounds.
    sf::FloatRect getLocalBounds() const;
    sf::FloatRect getGlobalBounds() const;

private:
    // Draw the animated sprite (called by SFML).
    virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const override;

    sf::Texture m_texture;
    std::vector<sf::IntRect> m_frames;
    sf::Sprite m_sprite;
    sf::Time m_frameTime;
    sf::Time m_elapsedTime;
    std::size_t m_currentFrame;
    bool m_isPlaying;
    bool m_loop;
};

#endif // ANIMATEDSPRITE_H
