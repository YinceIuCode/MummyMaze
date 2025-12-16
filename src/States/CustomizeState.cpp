#include "States/CustomizeState.hpp"
#include <iostream>

CustomizeState::CustomizeState(sf::RenderWindow* window, std::stack<std::unique_ptr<State>>* states)
    : State(window, states)
{
    initVariables();
    initGui();
}

CustomizeState::~CustomizeState() {

}

void CustomizeState::initVariables() {
    if (!m_font.openFromFile("assets/fonts/Akashi.ttf")) std::cerr << "Err Font\n";
    if (m_bgTexture.loadFromFile("assets/textures/Backgrounds/menu_bg.png")) {
        m_bgSprite.emplace(m_bgTexture);
    }

    if (!m_txArrowLeft.loadFromFile("assets/textures/Menu/arrow_left.png")) {
        std::cerr << "Err Arrow Left\n";
    }
    if (!m_txArrowRight.loadFromFile("assets/textures/Menu/arrow_right.png")) {
        std::cerr << "Err Arrow Right\n";
    }
    if (!m_buffClick.loadFromFile("assets/audios/hover.wav")) {
        std::cerr << "Err Click Sound\n";
    }

    m_themes = {
        {"ANCIENT EGYPT", "Playmap"},
        {"DORAEMON",  "Nobi"},
    };

    m_previewIndex = GameData::currentTheme;
    if (m_previewIndex < 0 || m_previewIndex >= m_themes.size()) m_previewIndex = 0;
}

void CustomizeState::initGui() {
    float cx = m_window->getSize().x / 2.f;
    float cy = m_window->getSize().y / 2.f;

    m_title.emplace(m_font, "CUSTOMIZE", 80);
    sf::FloatRect tb = m_title->getLocalBounds();
    m_title->setOrigin({ tb.size.x / 2.f, 0 });
    m_title->setPosition({ cx, 60.f });

    m_btnBack.emplace(m_font, "BACK", 50);
    m_btnBack->setPosition({ 80.f, 60.f });

    m_themeNameText.emplace(m_font, "", 60);
    m_themeNameText->setFillColor(sf::Color::Cyan);
    m_themeNameText->setPosition({ cx, cy + 220.f });

    m_btnPrev = std::make_unique<Button>(m_txArrowLeft, cx - 350.f, cy, 2.0f);
    m_btnNext = std::make_unique<Button>(m_txArrowRight, cx + 350.f, cy, 2.0f);

    m_btnPrev->setSound(m_buffClick, m_buffClick);
    m_btnNext->setSound(m_buffClick, m_buffClick);

    loadThemePreview(m_previewIndex);
}

void CustomizeState::loadThemePreview(int index) {
    ThemeInfo& current = m_themes[index];

    std::string basePath = "assets/textures/" + current.folderName + "/";

    if (m_txPreviewPlayer.loadFromFile(basePath + "player.png")) {
        m_sprPreviewPlayer.emplace(m_txPreviewPlayer);
        m_sprPreviewPlayer->setScale({ 4.0f, 4.0f });

        sf::FloatRect b = m_sprPreviewPlayer->getLocalBounds();
        m_sprPreviewPlayer->setOrigin({ b.size.x / 2.f, b.size.y / 2.f });
        m_sprPreviewPlayer->setPosition({ m_window->getSize().x / 2.f - 120.f, m_window->getSize().y / 2.f });
    }
    else {
        std::cerr << "Missing: " << basePath + "player.png" << "\n";
    }

    if (m_txPreviewMummy.loadFromFile(basePath + "mummy.png")) {
        m_sprPreviewMummy.emplace(m_txPreviewMummy);
        m_sprPreviewMummy->setScale({ 4.0f, 4.0f });

        sf::FloatRect b = m_sprPreviewMummy->getLocalBounds();
        m_sprPreviewMummy->setOrigin({ b.size.x / 2.f, b.size.y / 2.f });
        m_sprPreviewMummy->setPosition({ m_window->getSize().x / 2.f + 120.f, m_window->getSize().y / 2.f });

        m_sprPreviewMummy->setScale({ -4.0f, 4.0f });
    }

    if (m_themeNameText.has_value()) {
        m_themeNameText->setString(current.name);
        sf::FloatRect tb = m_themeNameText->getLocalBounds();
        m_themeNameText->setOrigin({ tb.size.x / 2.f, tb.size.y / 2.f });
    }

    GameData::currentTheme = index;
}

void CustomizeState::updateButtons() {
    sf::Vector2f mousePos = m_window->mapPixelToCoords(sf::Mouse::getPosition(*m_window));
    bool isPressed = sf::Mouse::isButtonPressed(sf::Mouse::Button::Left);

    m_btnPrev->update(mousePos, isPressed, 0.f);
    m_btnNext->update(mousePos, isPressed, 0.f);

    static bool isHandled = false;
    if (isPressed && !isHandled) {
        if (m_btnBack.has_value() && m_btnBack->getGlobalBounds().contains(mousePos)) {
            if (!GameData::isSfxMuted) {
                static sf::Sound snd(m_buffClick); snd.play();
            }
            m_states->pop();
            return;
        }

        if (m_btnPrev->isClicked()) {
            isHandled = true;
            m_previewIndex--;
            if (m_previewIndex < 0) m_previewIndex = static_cast<int>(m_themes.size() - 1);
            loadThemePreview(m_previewIndex);
        }
        else if (m_btnNext->isClicked()) {
            isHandled = true;
            m_previewIndex++;
            if (m_previewIndex >= m_themes.size()) m_previewIndex = 0;
            loadThemePreview(m_previewIndex);
        }
    }
    if (!isPressed) isHandled = false;

    if (m_btnBack.has_value()) {
        if (m_btnBack->getGlobalBounds().contains(mousePos)) m_btnBack->setFillColor(sf::Color::Yellow);
        else m_btnBack->setFillColor(sf::Color::White);
    }
}

void CustomizeState::update(float dt) {
    updateButtons();
}

void CustomizeState::render(sf::RenderWindow& window) {
    if (m_bgSprite.has_value()) window.draw(*m_bgSprite);
    if (m_title.has_value()) window.draw(*m_title);
    if (m_btnBack.has_value()) window.draw(*m_btnBack);

    if (m_sprPreviewPlayer.has_value()) window.draw(*m_sprPreviewPlayer);
    if (m_sprPreviewMummy.has_value()) window.draw(*m_sprPreviewMummy);

    if (m_themeNameText.has_value()) window.draw(*m_themeNameText);

    m_btnPrev->render(window);
    m_btnNext->render(window);
}