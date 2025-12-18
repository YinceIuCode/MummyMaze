#include "../../include/States/ModeSelectState.hpp"
#include <iostream>

ModeSelectState::ModeSelectState(sf::RenderWindow* window, std::stack<std::unique_ptr<State>>* states)
    : State(window, states),
    m_title(m_font), m_btn6x6(m_font), m_btn8x8(m_font), m_btn10x10(m_font), m_btnBack(m_font)
{
    initFonts();
    initGui();
}

ModeSelectState::~ModeSelectState() {

}

void ModeSelectState::reloadBackground() {
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

void ModeSelectState::initFonts() {
    if (!m_font.openFromFile("assets/fonts/Akashi.ttf")) {
        std::cerr << "ERROR: Could not load font\n";
    }
}

void ModeSelectState::initGui() {
    m_title.setString("SELECT MODE");
    m_title.setCharacterSize(100);
    sf::FloatRect titleRect = m_title.getGlobalBounds();
    m_title.setOrigin(titleRect.getCenter());
    m_title.setPosition({ m_window->getSize().x / 2.0f, 250.f });

    m_btn6x6.setString("6 x 6");
    m_btn6x6.setCharacterSize(70);
    sf::FloatRect rect6 = m_btn6x6.getGlobalBounds();
    m_btn6x6.setOrigin(rect6.getCenter());
    m_btn6x6.setPosition({ m_window->getSize().x / 2.0f, 500.f });

    m_btn8x8.setString("8 x 8");
    m_btn8x8.setCharacterSize(70);
    sf::FloatRect rect8 = m_btn8x8.getGlobalBounds();
    m_btn8x8.setOrigin(rect8.getCenter());
    m_btn8x8.setPosition({ m_window->getSize().x / 2.0f, 650.f });

    m_btn10x10.setString("10 x 10");
    m_btn10x10.setCharacterSize(70);
    sf::FloatRect rect10 = m_btn10x10.getGlobalBounds();
    m_btn10x10.setOrigin(rect10.getCenter());
    m_btn10x10.setPosition({ m_window->getSize().x / 2.0f, 800.f });

    m_btnBack.setString("BACK");
    sf::FloatRect rectBack = m_btnBack.getGlobalBounds();
    m_btnBack.setOrigin(rectBack.getCenter());
    m_btnBack.setPosition({ 100.f, 50.f });

    reloadBackground();
    m_darkLayer.setSize(sf::Vector2f(static_cast<float>(m_window->getSize().x), static_cast<float>(m_window->getSize().y)));
    m_darkLayer.setFillColor(sf::Color(0, 0, 0, 50));

    if (!m_buffHover.loadFromFile("assets/audios/hover.wav")) {
        std::cerr << "Error loading hover sound\n";
    }
    if (!m_buffClick.loadFromFile("assets/audios/click.wav")) {
        std::cerr << "Error loading click sound\n";
    }

	m_loadingText.emplace(m_font, "LOADING...", 40);
    m_loadingText->setFillColor(sf::Color::Yellow);
    m_loadingText->setStyle(sf::Text::Italic | sf::Text::Bold);

    sf::FloatRect lb = m_loadingText->getLocalBounds();
    m_loadingText->setOrigin({ 0, lb.size.y / 2.0f });
}

void ModeSelectState::updateButtons() {
    sf::Vector2f mousePos = m_window->mapPixelToCoords(sf::Mouse::getPosition(*m_window));
    static bool isHandled = false;

    auto handleHover = [&](sf::Text& txt) {
        if (txt.getGlobalBounds().contains(mousePos)) {
            txt.setFillColor(sf::Color::Red);
            txt.setScale({ 1.1f, 1.1f });
            if (m_currentHoveredBtn != &txt) {
                m_currentHoveredBtn = &txt;
                m_sfx.emplace(m_buffHover);
                if (!GameData::isSfxMuted) {
                    m_sfx->play();
                }
            }
        }
        else {
            txt.setFillColor(sf::Color::White);
            txt.setScale({ 1.0f, 1.0f });
            if (m_currentHoveredBtn == &txt) {
                m_currentHoveredBtn = nullptr;
            }
        }
        };

    handleHover(m_btn6x6);
    handleHover(m_btn8x8);
    handleHover(m_btn10x10);
    handleHover(m_btnBack);

    auto createAndPlay = [&](int size) {
        generate_maze gen(size);
        gen.generate();
        gen.print_maze();

        auto newGameState = std::make_unique<GameState>(m_window, m_states, "assets/mazes/maze1.txt", false);
        auto statesStack = m_states;
        statesStack->pop();
        statesStack->push(std::move(newGameState));
        };

    if (sf::Mouse::isButtonPressed(sf::Mouse::Button::Left)) {
        if (!isHandled) {
            m_sfx.emplace(m_buffClick);

            auto clickAction = [&](sf::Text& btn, auto action, bool showLoading) {
                btn.setScale({ 0.9f, 0.9f }); 

                m_sfx.emplace(m_buffClick);
                if (!GameData::isSfxMuted) m_sfx->play();

                if (showLoading) {
                    sf::FloatRect btnBounds = btn.getGlobalBounds();
                    float posX = btn.getPosition().x + btnBounds.size.x / 2.f + 50.f;
                    float posY = btn.getPosition().y;

                    m_loadingText->setPosition({ posX, posY });

                    m_window->clear();
                    this->render(*m_window);
                    m_window->draw(*m_loadingText); 
                    m_window->display();

                    sf::sleep(sf::milliseconds(400));
                }
                else {
                    m_window->clear();
                    this->render(*m_window);
                    m_window->display();
                    sf::sleep(sf::milliseconds(150));
                }

                action();
                };

            if (m_btn6x6.getGlobalBounds().contains(mousePos)) {
                clickAction(m_btn6x6, [&]() { createAndPlay(6); }, true);
            }
            else if (m_btn8x8.getGlobalBounds().contains(mousePos)) {
                clickAction(m_btn8x8, [&]() { createAndPlay(8); }, true);
            }
            else if (m_btn10x10.getGlobalBounds().contains(mousePos)) {
                clickAction(m_btn10x10, [&]() { createAndPlay(10); }, true);
            }
            else if (m_btnBack.getGlobalBounds().contains(mousePos)) {
                clickAction(m_btnBack, [&]() { m_states->pop(); }, false);
            }
            isHandled = true;
        }
    }
    else {
        isHandled = false;
        m_btn6x6.setScale({ 1.0f, 1.0f });
        m_btn8x8.setScale({ 1.0f, 1.0f });
        m_btn10x10.setScale({ 1.0f, 1.0f });
        m_btnBack.setScale({ 1.0f, 1.0f });
    }
}

void ModeSelectState::update(float dt) {
    updateButtons();
}

void ModeSelectState::render(sf::RenderWindow& window) {
    if (m_lastThemeId != GameData::currentTheme) {
        reloadBackground();
    }
    if (m_bgSprite.has_value()) window.draw(*m_bgSprite);
    window.draw(m_darkLayer);
    window.draw(m_title);
    window.draw(m_btn6x6);
    window.draw(m_btn8x8);
    window.draw(m_btn10x10);
    window.draw(m_btnBack);
}