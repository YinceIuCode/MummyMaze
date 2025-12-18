#include "../../include/States/SettingState.hpp"
#include <iostream>
#include <string>
#include <cmath>

SettingState::SettingState(sf::RenderWindow* window, std::stack<std::unique_ptr<State>>* states)
    : State(window, states)
{
    initVariables();
    initGui();
}

SettingState::~SettingState() {

}

void SettingState::reloadBackground() {
    std::string themeName;
    if (GameData::currentTheme == 0) themeName = "Playmap";
    else if (GameData::currentTheme == 1) themeName = "Nobi";

    std::string fileTheme = "assets/textures/Backgrounds/" + themeName + "_bgT.png";

    if (m_bgTexture.loadFromFile(fileTheme.c_str())) {
        m_bgTexture.setSmooth(true);
        if (!m_bgSprite.has_value()) {
            m_bgSprite.emplace(m_bgTexture);
        }
        else {
            m_bgSprite->setTexture(m_bgTexture);
        }
    }

    if (m_bgSprite.has_value()) {
        m_bgSprite->setScale({ 1.f, 1.f });
        m_bgSprite->setScale({
            1290.f / m_bgSprite->getGlobalBounds().size.x,
            960.f / m_bgSprite->getGlobalBounds().size.y
            });
    }

    m_lastThemeId = GameData::currentTheme;
    std::cout << "Background reloaded: " << themeName << "\n";
}

void SettingState::initVariables() {
    if (!m_font.openFromFile("assets/fonts/Akashi.ttf")) std::cerr << "Err Font\n";
    reloadBackground();

    if (!m_buffClick.loadFromFile("assets/audios/click.wav")) {
        std::cerr << "Err Click Sound Buffer\n";
    }

    if (!m_txBar.loadFromFile("assets/textures/Menu/slider_bar.png")) {
        std::cerr << "Err Slider Bar Texture\n";
    }
    if (!m_txKnob.loadFromFile("assets/textures/Menu/slider_knob.png")) {
        std::cerr << "Err Slider Knob Texture\n";
    }
    if (!m_txTick.loadFromFile("assets/textures/Menu/icon_tick.png")) {
        std::cerr << "Err Tick Texture\n";
    }
    if (!m_txCross.loadFromFile("assets/textures/Menu/icon_cross.png")) {
        std::cerr << "Err Cross Texture\n";
    }
}

void SettingState::initGui() {
    float cx = m_window->getSize().x / 2.f;
    float scale = 1.5f;

    m_title.emplace(m_font, "SETTINGS", 80);
    sf::FloatRect tb = m_title->getLocalBounds();
    m_title->setOrigin({ tb.size.x / 2.f, 0 });
    m_title->setPosition({ cx, 80.f });

    m_btnBack.emplace(m_font, "BACK", 50);
    m_btnBack->setPosition({ 80.f, 60.f });

    float row1_Y = 300.f;

    m_txtMusicLabel.emplace(m_font, "MUSIC VOLUME", 40);
    m_txtMusicLabel->setScale({ scale, scale });

    sf::FloatRect lbBounds = m_txtMusicLabel->getLocalBounds();
    m_txtMusicLabel->setOrigin({ lbBounds.size.x, lbBounds.size.y / 2.f });
    m_txtMusicLabel->setPosition({ cx - 20.f, row1_Y });

    m_sprBar.emplace(m_txBar);
    m_sprBar->setScale({ scale * 1.1f, scale * 1.1f });
    sf::FloatRect barBounds = m_sprBar->getLocalBounds();
    m_sprBar->setOrigin({ 0, barBounds.size.y / 2.f });
    m_sprBar->setPosition({ cx + 20.f, row1_Y });

    m_barFill.setSize({ 0.f, barBounds.size.y * scale });
    m_barFill.setFillColor(sf::Color::Cyan);
    m_barFill.setOrigin({ 0, (barBounds.size.y * scale) / 2.f });
    m_barFill.setPosition({ cx + 25.f, row1_Y });

    m_sprKnob.emplace(m_txKnob);
    m_sprKnob->setScale({ scale * 1.1f, scale * 1.1f });
    sf::FloatRect knobBounds = m_sprKnob->getLocalBounds();
    m_sprKnob->setOrigin({ knobBounds.size.x / 2.f, knobBounds.size.y / 2.f });

    float currentPercent = GameData::musicVolume / 100.f;
    float barWidthReal = barBounds.size.x * scale;

    m_sprKnob->setPosition({ m_sprBar->getPosition().x + (currentPercent * barWidthReal), row1_Y });
    m_barFill.setSize({ currentPercent * barWidthReal, barBounds.size.y * scale * 0.6f });

    float row2_Y = 450.f;

    m_txtSfxLabel.emplace(m_font, "SFX MUTE", 40);
    m_txtSfxLabel->setScale({ scale, scale });
    sf::FloatRect sfxBounds = m_txtSfxLabel->getLocalBounds();
    m_txtSfxLabel->setOrigin({ sfxBounds.size.x, sfxBounds.size.y / 2.f });
    m_txtSfxLabel->setPosition({ cx - 20.f, row2_Y });

    if (GameData::isSfxMuted) m_sprSfxToggle.emplace(m_txCross);
    else m_sprSfxToggle.emplace(m_txTick);

    m_sprSfxToggle->setScale({ scale * 1.5f, scale * 1.5f });
    sf::FloatRect togBounds = m_sprSfxToggle->getLocalBounds();
    m_sprSfxToggle->setOrigin({ 0, togBounds.size.y / 2.f });
    m_sprSfxToggle->setPosition({ cx + 20.f, row2_Y + 5.f });

    m_darkLayer.setSize(sf::Vector2f(static_cast<float>(m_window->getSize().x), static_cast<float>(m_window->getSize().y)));
    m_darkLayer.setFillColor(sf::Color(0, 0, 0, 50));
}

void SettingState::updateButtons() {
    sf::Vector2f mousePos = m_window->mapPixelToCoords(sf::Mouse::getPosition(*m_window));
    bool isPressed = sf::Mouse::isButtonPressed(sf::Mouse::Button::Left);

    if (m_btnBack.has_value()) {
        if (m_btnBack->getGlobalBounds().contains(mousePos)) {
            m_btnBack->setFillColor(sf::Color::Yellow);
            m_btnBack->setScale({ 1.1f, 1.1f });

            if (isPressed) {
                static sf::Sound soundClick(m_buffClick);
                soundClick.play();

                sf::sleep(sf::milliseconds(150));
                m_states->pop();
                return;
            }
        }
        else {
            m_btnBack->setFillColor(sf::Color::White);
            m_btnBack->setScale({ 1.0f, 1.0f });
        }
    }

    if (m_sprBar.has_value() && m_sprKnob.has_value()) {
        sf::FloatRect knobBox = m_sprKnob->getGlobalBounds();
        sf::FloatRect barBox = m_sprBar->getGlobalBounds();

        static bool wasPressed = false;
        if (isPressed && !wasPressed) {
            if (knobBox.contains(mousePos) || barBox.contains(mousePos)) {
                m_isDragging = true;
            }
            if (m_sprSfxToggle.has_value() && m_sprSfxToggle->getGlobalBounds().contains(mousePos)) {
                GameData::isSfxMuted = !GameData::isSfxMuted;
                if (GameData::isSfxMuted) m_sprSfxToggle->setTexture(m_txCross);
                else m_sprSfxToggle->setTexture(m_txTick);
            }
        }
        wasPressed = isPressed;

        if (!isPressed) m_isDragging = false;

        if (m_isDragging) {
            float newX = mousePos.x;
            float padding = 15.0f * 1.5f * 1.1f;
            float minX = barBox.position.x + padding;
            float maxX = barBox.position.x + barBox.size.x - padding;

            if (newX < minX) newX = minX;
            if (newX > maxX) newX = maxX;

            m_sprKnob->setPosition({ newX, m_sprKnob->getPosition().y });

            float fillWidth = newX - (barBox.position.x);
            m_barFill.setSize({ fillWidth, m_barFill.getSize().y });

            float slideRange = maxX - minX;
            float percentage = (newX - minX) / slideRange;

            GameData::musicVolume = percentage * 100.f;
            GameData::bgMusic.setVolume(GameData::musicVolume);
        }
    }
}

void SettingState::update(float dt) {
    updateButtons();
}

void SettingState::render(sf::RenderWindow& window) {
    if (m_lastThemeId != GameData::currentTheme) {
        reloadBackground();
    }
    if (m_bgSprite.has_value()) window.draw(*m_bgSprite);
    window.draw(m_darkLayer);
    if (m_title.has_value()) window.draw(*m_title);
    if (m_btnBack.has_value()) window.draw(*m_btnBack);

    if (m_txtMusicLabel.has_value()) window.draw(*m_txtMusicLabel);
    window.draw(m_barFill);
    if (m_sprBar.has_value()) window.draw(*m_sprBar);
    if (m_sprKnob.has_value()) window.draw(*m_sprKnob);

    if (m_txtSfxLabel.has_value()) window.draw(*m_txtSfxLabel);
    if (m_sprSfxToggle.has_value()) window.draw(*m_sprSfxToggle);
}