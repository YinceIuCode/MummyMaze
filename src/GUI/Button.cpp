#include <GUI/Button.hpp>
#include <Core/GameData.hpp>
#include <iostream>

Button::Button(const sf::Texture& texture, float x, float y, float scale) {
    m_sprite.emplace(texture);
    m_baseScale = scale;
    m_enableBreathing = false;
    m_isHovered = false;
    m_isPressed = false;
    m_buffHover = nullptr;
    m_buffClick = nullptr;

    sf::FloatRect bounds = m_sprite->getLocalBounds();
    m_sprite->setOrigin({ bounds.size.x / 2.f, bounds.size.y / 2.f });
    m_sprite->setPosition({ x, y });
    m_sprite->setScale({ scale, scale });
}

Button::~Button() {

}

void Button::setSound(const sf::SoundBuffer& hoverBuff, const sf::SoundBuffer& clickBuff) {
    m_buffHover = &hoverBuff;
    m_buffClick = &clickBuff;
}

void Button::setBreathing(bool enable) {
    m_enableBreathing = enable;
}

void Button::update(const sf::Vector2f& mousePos, bool isMousePressed, float totalTime, bool blocked) {
    if (blocked) {
        m_isHovered = false;
        m_isPressed = false;
        m_sprite->setColor(sf::Color::White);
        m_sprite->setScale({ m_baseScale, m_baseScale });
        return;
    }

    if (m_sprite->getGlobalBounds().contains(mousePos)) {
        if (!m_isHovered) {
            m_isHovered = true;
            if (m_buffHover) {
                m_sfx.emplace(*m_buffHover);
                if (!GameData::isSfxMuted) {
                    m_sfx->play();
                }
            }
        }

        if (isMousePressed) {
            m_isPressed = true;
            m_sprite->setScale({ m_baseScale * 0.9f, m_baseScale * 0.9f });
            m_sprite->setColor(sf::Color(180, 180, 180));
        }
        else {
            m_isPressed = false;
            m_sprite->setScale({ m_baseScale * 1.1f, m_baseScale * 1.1f });
            m_sprite->setColor(sf::Color::White);
        }
    }
    else {
        m_isHovered = false;
        m_isPressed = false;
        m_sprite->setColor(sf::Color::White);

        if (m_enableBreathing) {
            float breathing = std::sin(totalTime * 5.0f) * 0.05f;
            m_sprite->setScale({ m_baseScale + breathing, m_baseScale + breathing });
        }
        else {
            m_sprite->setScale({ m_baseScale, m_baseScale });
        }
    }
}

bool Button::isClicked() const {
    return m_isPressed;
}

void Button::render(sf::RenderTarget& target) {
    target.draw(*m_sprite);
}

void Button::executeClickEffect(std::function<void()> drawSceneFunc, std::function<void()> onFinished) {
    if (m_buffClick) {
        m_sfx.emplace(*m_buffClick);
        if (!GameData::isSfxMuted) {
            m_sfx->play();
        }
    }

    m_sprite->setScale({ m_baseScale * 0.9f, m_baseScale * 0.9f });
    m_sprite->setColor(sf::Color(150, 150, 150));

    if (drawSceneFunc) {
        drawSceneFunc();
    }

    sf::sleep(sf::milliseconds(150));

    if (onFinished) {
        onFinished();
    }
}