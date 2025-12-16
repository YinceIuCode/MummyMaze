#pragma once
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <functional>
#include <cmath>

class Button {
private:
    std::optional<sf::Sprite> m_sprite;

    bool m_isHovered;
    bool m_isPressed;
    float m_baseScale;

    bool m_enableBreathing;

    const sf::SoundBuffer* m_buffHover;
    const sf::SoundBuffer* m_buffClick;
    std::optional<sf::Sound> m_sfx;

public:
    Button(const sf::Texture& texture, float x, float y, float scale = 1.0f);
    ~Button();

    void setSound(const sf::SoundBuffer& hoverBuff, const sf::SoundBuffer& clickBuff);
    void setBreathing(bool enable);
    void setColor(const sf::Color& color) {
        if (m_sprite.has_value()) {
            m_sprite->setColor(color);
        }
	}

    void update(const sf::Vector2f& mousePos, bool isMousePressed, float totalTime, bool blocked = false);
    bool isClicked() const;
    void render(sf::RenderTarget& target);

    sf::Vector2f getPosition() const { return m_sprite->getPosition(); }
    void setPosition(float x, float y) { m_sprite->setPosition({ x, y }); }
    sf::FloatRect getGlobalBounds() const { return m_sprite->getGlobalBounds(); }

    void executeClickEffect(std::function<void()> drawSceneFunc, std::function<void()> onFinished);
};