#include "../../include/States/HowToPlayState.hpp"
#include "../../include/Core/GameData.hpp"
#include "../../include/States/GameState.hpp"
#include <iostream>

HowToPlayState::HowToPlayState(sf::RenderWindow* window, std::stack<std::unique_ptr<State>>* states)
    : State(window, states)
{
    initVariables();
    initGui();
}

HowToPlayState::~HowToPlayState() {

}

void HowToPlayState::initVariables() {
    if (!m_font.openFromFile("assets/fonts/Akashi.ttf")) std::cerr << "Err Font\n";
    if (m_bgTexture.loadFromFile("assets/textures/Backgrounds/menu_bg.png")) {
        m_bgSprite.emplace(m_bgTexture);
    }
    if (!m_buffClick.loadFromFile("assets/audios/click.wav")) {
        std::cerr << "Err Click Sound\n";
    }
}

void HowToPlayState::initGui() {
    float cx = m_window->getSize().x / 2.f;
    float cy = m_window->getSize().y / 2.f;

    m_title.emplace(m_font, "HOW TO PLAY", 80);
    sf::FloatRect tb = m_title->getLocalBounds();
    m_title->setOrigin({ tb.size.x / 2.f, 0 });
    m_title->setPosition({ cx, 60.f });

    m_btnBack.emplace(m_font, "BACK", 50);
    m_btnBack->setPosition({ 80.f, 60.f });

    m_btnTry.emplace(m_font, "TRY IT NOW!", 50);
	sf::FloatRect tbTry = m_btnTry->getLocalBounds();
	m_btnTry->setOrigin({ tbTry.size.x / 2.f, tbTry.size.y / 2.f });
	m_btnTry->setPosition({ cx, m_window->getSize().y - 80.f });

    m_container.setSize({ 1000.f, 600.f });
    m_container.setFillColor(sf::Color(0, 0, 0, 150));
    m_container.setOutlineColor(sf::Color::Cyan);
    m_container.setOutlineThickness(2.f);
    m_container.setOrigin({ 500.f, 300.f });
    m_container.setPosition({ cx, cy + 60.f });

    std::vector<std::pair<std::string, std::string>> content = {
        { "WASD / ARROWS",  "Move Character" },
        { "SPACE",          "Mummy Move Next" },
        { "Key 'U'",        "Undo (Step Back)" },
        { "Key 'I'",        "Redo (Step Forward)" },
        { "Key 'P'",        "Save Game" },
        { "Key 'R'",        "Reset Level" },
        { "Goal",           "Reach the Exit" }
    };

    float startY = cy - 150.f;
    float gapY = 70.f;

    for (int i = 0; i < content.size(); i++) {
        sf::Text keyText(m_font, content[i].first, 40);
        sf::Text descText(m_font, content[i].second, 40);

        keyText.setFillColor(sf::Color::Yellow);
        sf::FloatRect kb = keyText.getLocalBounds();
        keyText.setOrigin({ kb.size.x, kb.size.y / 2.f });
        keyText.setPosition({ cx - 30.f, startY + i * gapY });

        descText.setFillColor(sf::Color::White);
        sf::FloatRect db = descText.getLocalBounds();
        descText.setOrigin({ 0, db.size.y / 2.f });
        descText.setPosition({ cx + 20.f, startY + i * gapY });

        m_instructions.push_back({ keyText, descText });
    }
}

void HowToPlayState::updateInput(float dt) {
    sf::Vector2f mousePos = m_window->mapPixelToCoords(sf::Mouse::getPosition(*m_window));

    if (m_btnBack.has_value()) {
        if (m_btnBack->getGlobalBounds().contains(mousePos)) {
            m_btnBack->setFillColor(sf::Color::Yellow);

            if (sf::Mouse::isButtonPressed(sf::Mouse::Button::Left)) {
                if (!GameData::isSfxMuted) {
                    sf::Sound snd(m_buffClick);
                    if (snd.getStatus() != sf::Sound::Status::Playing) snd.play();
                }

                sf::sleep(sf::milliseconds(150));
                m_states->pop();
                return;
            }
        }
        else {
            m_btnBack->setFillColor(sf::Color::White);
        }
    }
    if (m_btnTry.has_value()) {
        if (m_btnTry->getGlobalBounds().contains(mousePos)) {
            m_btnTry->setFillColor(sf::Color::Yellow);
			m_btnTry->setStyle(sf::Text::Bold);
            if (sf::Mouse::isButtonPressed(sf::Mouse::Button::Left)) {
                if (!GameData::isSfxMuted) {
                    sf::Sound snd(m_buffClick);
                    if (snd.getStatus() != sf::Sound::Status::Playing) snd.play();
                }
                sf::sleep(sf::milliseconds(150));
                m_states->push(std::make_unique<HowToPlayState>(m_window, m_states));
                auto createAndPlay = [&]() {
                    auto newGameState = std::make_unique<GameState>(m_window, m_states, "assets/mazes/mazeplay.txt", false);
                    auto statesStack = m_states;
                    statesStack->push(std::move(newGameState));
                    };
				createAndPlay();
            }
        }
        else {
            m_btnTry->setFillColor(sf::Color::White);
            m_btnTry->setStyle(sf::Text::Bold);
        }
	}
}

void HowToPlayState::update(float dt) {
    updateInput(dt);
}

void HowToPlayState::render(sf::RenderWindow& window) {
    if (m_bgSprite.has_value()) window.draw(*m_bgSprite);
    if (m_title.has_value()) window.draw(*m_title);
    if (m_btnTry.has_value()) window.draw(*m_btnTry);

    window.draw(m_container);

    for (const auto& line : m_instructions) {
        window.draw(line.first);
        window.draw(line.second);
    }

    if (m_btnBack.has_value()) window.draw(*m_btnBack);
}