#include "../../include/Core/Game.hpp"
#include "../../include/States/MainMenuState.hpp"
#include <iostream>

void Game::initStates() {
    m_states.push(std::make_unique<MainMenuState>(&m_window, &m_states));
}

void Game::initWindow() {
    m_videoMode = sf::VideoMode({ 1290, 960 });
    m_window.create(m_videoMode, "Mummy Game");
    m_window.setFramerateLimit(60);

    initStates();
}

Game::Game() {
    this->initWindow();
    this->initStates();
    this->initMusic();
}

Game::~Game() {
    while (!m_states.empty()) m_states.pop();
}

const bool Game::isRunning() const {
    return m_window.isOpen();
}

void Game::updateDt() {
    m_dt = m_dtClock.restart().asSeconds();
}

void Game::update() {
    this->updateDt();

    while (const std::optional event = m_window.pollEvent()) {
        if (event->is<sf::Event::Closed>()) m_window.close();
    }

    if (!m_states.empty()) {
        m_states.top()->update(m_dt);
    }
    else {
        m_window.close();
    }
}

void Game::initMusic() {
    if (!GameData::bgMusic.openFromFile("assets/audios/music.mp3")) {
        std::cerr << "ERROR: Could not load background music!\n";
    }
    else {
        GameData::bgMusic.setLooping(true);
        GameData::bgMusic.setVolume(GameData::musicVolume);
        GameData::bgMusic.play();
    }
}

void Game::render() {
    m_window.clear(sf::Color::Black);
    if (!m_states.empty()) {
        m_states.top()->render(m_window);
    }
    m_window.display();
}