#include "States/GameState.hpp"
#include <cmath>
#include <iostream>
#include <fstream>
#include <filesystem>

void clearSaveData() {
    std::string filePath = "assets/mazes/mazesave.txt";

    try {
        if (std::filesystem::exists(filePath)) {
            if (std::filesystem::remove(filePath)) {
                std::cout << "Deleted save file successfully: " << filePath << "\n";
            }
            else {
                std::cerr << "Failed to delete file.\n";
            }
        }
        else {
            std::cout << "No save file found to delete.\n";
        }
    }
    catch (const std::filesystem::filesystem_error& e) {
        std::cerr << "Error handling file: " << e.what() << "\n";
    }
}

GameState::GameState(sf::RenderWindow* window, std::stack<std::unique_ptr<State>>* states, std::string mapPath, bool isResuming)
    : State(window, states)
{
    m_currentMapPath = mapPath;

    int pGX = 0, pGY = 0, mGX = 0, mGY = 0;
    std::string savedMapPath = "";

    if (isResuming) {
        savedMapPath = loadGameData(pGX, pGY, mGX, mGY);
        if (!savedMapPath.empty()) {
            m_currentMapPath = savedMapPath;
        }
    }

    initVariables();

    m_map.loadMap(m_currentMapPath, m_player, m_mummy);

    if (isResuming && !savedMapPath.empty()) {
        float tileSize = m_map.getTileSize();
        sf::Vector2f mapPos = m_map.getPosition();
        float offset = m_map.getDynamicOffset();

        float pPixelX = pGX * tileSize + mapPos.x - offset;
        float pPixelY = pGY * tileSize + mapPos.y - offset;
        m_player.setPosition(pPixelX, pPixelY);

        float mPixelX = mGX * tileSize + mapPos.x - offset;
        float mPixelY = mGY * tileSize + mapPos.y - offset;

        m_mummy.setSpawn(mGY, mGX, mPixelX, mPixelY);

        std::cout << "Resumed Game Successfully!\n";
    }
}

GameState::~GameState() {

}

void GameState::generateNewMaze(int mapsize) {
    generate_maze maze(mapsize);
    do {
        maze.generate();
    } while (maze.can_solve_the_maze() > 2);
    maze.print_maze();
}

void GameState::saveGame() {
    std::ofstream outFile("assets/mazes/mazesave.txt");
    if (outFile.is_open()) {
        outFile << m_currentMapPath << "\n";

        auto getPlayerGrid = [&]() -> sf::Vector2i {
            float tileSize = m_map.getTileSize();
            sf::Vector2f mapPos = m_map.getPosition();
            sf::Vector2f pPos = m_player.getPosition();
            float offset = m_map.getDynamicOffset();

            float relativeX = pPos.x - mapPos.x + offset + tileSize / 2;
            float relativeY = pPos.y - mapPos.y + offset + tileSize / 2;

            int c = static_cast<int>(std::floor(relativeX / tileSize));
            int r = static_cast<int>(std::floor(relativeY / tileSize));

            return { c, r };
            };

        sf::Vector2i pGrid = getPlayerGrid();
        outFile << pGrid.x << " " << pGrid.y << "\n";
        outFile << m_mummy.getR() << " " << m_mummy.getC() << "\n";

        std::cout << "Game Saved!\n";
        outFile.close();
    }
}

std::string GameState::loadGameData(int& pGridX, int& pGridY, int& mGridX, int& mGridY) {
    std::ifstream inFile("assets/mazes/mazesave.txt");
    std::string mapPath = "";

    if (inFile.is_open()) {
        std::getline(inFile, mapPath);
        inFile >> pGridX >> pGridY;

        int mR, mC;
        inFile >> mR >> mC;
        mGridY = mR;
        mGridX = mC;

        inFile.close();
    }
    return mapPath;
}

void GameState::initVariables() {
    if (!m_txUndo.loadFromFile("assets/textures/Menu/btn_undo.png")) std::cerr << "Err Undo\n";
    if (!m_txRedo.loadFromFile("assets/textures/Menu/btn_redo.png")) std::cerr << "Err Redo\n";
    if (!m_txReset.loadFromFile("assets/textures/Menu/btn_reset.png")) std::cerr << "Err Reset\n";
    if (!m_txBack.loadFromFile("assets/textures/Menu/btn_back.png")) std::cerr << "Err Back\n";
    if (!m_txSave.loadFromFile("assets/textures/Menu/btn_save.png")) std::cerr << "Err Save\n";
    if (!m_txYes.loadFromFile("assets/textures/Menu/icon_tick.png")) std::cerr << "Err Yes\n";
    if (!m_txNo.loadFromFile("assets/textures/Menu/icon_cross.png")) std::cerr << "Err No\n";
    if (!m_txNext.loadFromFile("assets/textures/Menu/btn_next.png")) std::cerr << "Err Next\n";

    float startX = 100.f;
    float startY = 200.f;
    float gap = 120.f;
    float scale = 2.0f;

    m_btnUndo = std::make_unique<Button>(m_txUndo, startX, startY, scale);
    m_btnReset = std::make_unique<Button>(m_txReset, startX, startY + gap, scale);
    m_btnSave = std::make_unique<Button>(m_txSave, startX, startY + gap * 2, scale);
    m_btnRedo = std::make_unique<Button>(m_txRedo, startX, startY + gap * 3, scale);
    m_btnBack = std::make_unique<Button>(m_txBack, 1220.f, 50.f, scale);

    std::ifstream fileCount(m_currentMapPath);
    int lineCount = 0;
    std::string tempLine;
    if (fileCount.is_open()) {
        while (std::getline(fileCount, tempLine)) if (tempLine.length() > 1) lineCount++;
        fileCount.close();
    }
    if (lineCount < 3) lineCount = 13;
    int logicalSize = (lineCount - 1) / 2;
    m_currentMapSize = logicalSize;

    float maxMapHeight = 720.0f;
    float tileSize = maxMapHeight / logicalSize;
    if (tileSize > 120.0f) tileSize = 120.0f;
    m_map.setTileSize(tileSize);

    std::string themePath = (GameData::currentTheme == 1) ? "Nobi" : "Playmap";
    m_map.loadTheme(themePath);
    m_player.loadTheme(themePath);
    m_mummy.loadTheme(themePath);

    float realMapWidth = logicalSize * tileSize;
    m_map.setPosition(1290.f / 2.f - 300.f, 210.f);

    m_map.loadMap(m_currentMapPath, m_player, m_mummy);
    time_machine.push_state(m_player, m_mummy);
    m_turn = TurnState::PlayerInput;

    m_showExitConfirm = false;

    m_darkLayer.setSize(sf::Vector2f(m_window->getSize().x, m_window->getSize().y));
    m_darkLayer.setFillColor(sf::Color(0, 0, 0, 150));

    m_popupPanel.setSize(sf::Vector2f(800.f, 300.f));
    m_popupPanel.setFillColor(sf::Color(50, 40, 30));
    m_popupPanel.setOutlineThickness(4.f);
    m_popupPanel.setOutlineColor(sf::Color(200, 180, 50));
    m_popupPanel.setOrigin({ 400.f, 150.f });
    m_popupPanel.setPosition({ m_window->getSize().x / 2.f, m_window->getSize().y / 2.f });

    static sf::Font font;
    if (!font.openFromFile("assets/fonts/Akashi.ttf")) {
        std::cerr << "Error loading font for popup text!\n";
    }

    m_popupText.emplace(font, "Save Game Before Exit?", 50);
    m_popupText->setFillColor(sf::Color::White);

    sf::FloatRect textRect = m_popupText->getLocalBounds();
    m_popupText->setOrigin({ textRect.size.x / 2.0f, textRect.size.y / 2.0f });
    m_popupText->setPosition({ m_window->getSize().x / 2.f, m_window->getSize().y / 2.f - 50.f });

    float centerX = m_window->getSize().x / 2.f;
    float centerY = m_window->getSize().y / 2.f;

    m_btnYes = std::make_unique<Button>(m_txYes, centerX - 100.f, centerY + 60.f, 1.5f);
    m_btnNo = std::make_unique<Button>(m_txNo, centerX + 100.f, centerY + 60.f, 1.5f);

    m_endGameStatus = 0;

    m_endGameText.emplace(font, "", 100);
    m_endGameText->setStyle(sf::Text::Bold);
    m_endGameText->setOutlineThickness(3.f);
    m_endGameText->setOutlineColor(sf::Color::Black);

    float cx = m_window->getSize().x / 2.f;
    float cy = m_window->getSize().y / 2.f;

    m_btnRetry = std::make_unique<Button>(m_txReset, cx - 80.f, cy + 80.f, 1.5f);
    m_btnMenuEnd = std::make_unique<Button>(m_txBack, cx, cy + 80.f, 1.5f);
    m_btnNext = std::make_unique<Button>(m_txNext, cx + 80.f, cy + 80.f, 1.5f);
}

void GameState::update(float dt) {
    m_totalTime += dt;
    sf::Vector2f mousePos = m_window->mapPixelToCoords(sf::Mouse::getPosition(*m_window));
    bool isMousePressed = sf::Mouse::isButtonPressed(sf::Mouse::Button::Left);

    if (isMousePressed && m_isWaitingForMouseRelease) return;
    if (!isMousePressed) m_isWaitingForMouseRelease = false;

    if (m_endGameStatus != 0) {
        m_btnRetry->update(mousePos, isMousePressed, m_totalTime);
        m_btnMenuEnd->update(mousePos, isMousePressed, m_totalTime);
        m_btnNext->update(mousePos, isMousePressed, m_totalTime);

        if (m_btnRetry->isClicked()) {
            m_map.loadMap(m_currentMapPath, m_player, m_mummy);
            clearSaveData();
            m_endGameStatus = 0;
            m_turn = TurnState::PlayerInput;
            time_machine.reset();
            time_machine.push_state(m_player, m_mummy);
            sf::sleep(sf::milliseconds(200));
        }

        if (m_btnMenuEnd->isClicked()) {
            clearSaveData();
            m_states->pop();
            return;
        }

        if (m_btnNext->isClicked()) {
            generate_maze gen(m_currentMapSize);
            gen.generate();
            gen.print_maze();

            auto newGameState = std::make_unique<GameState>(m_window, m_states, "assets/mazes/maze1.txt", false);
            auto statesStack = m_states;
            statesStack->pop();

            statesStack->push(std::move(newGameState));
        }
        return;
    }

    if (m_showExitConfirm) {
        m_btnYes->update(mousePos, isMousePressed, m_totalTime);
        m_btnNo->update(mousePos, isMousePressed, m_totalTime);

        if (m_btnYes->isClicked()) {
            saveGame();
            sf::sleep(sf::milliseconds(200));
            m_states->pop();
            return;
        }

        if (m_btnNo->isClicked()) {
            sf::sleep(sf::milliseconds(200));
            m_states->pop();
            return;
        }

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Escape)) {
            m_showExitConfirm = false;
            m_isWaitingForMouseRelease = true;
            sf::sleep(sf::milliseconds(200));
        }

        return;
    }

    m_player.update(dt);
    m_mummy.update(dt);

    if (m_btnUndo)  m_btnUndo->update(mousePos, isMousePressed, m_totalTime);
    if (m_btnRedo)  m_btnRedo->update(mousePos, isMousePressed, m_totalTime);
    if (m_btnReset) m_btnReset->update(mousePos, isMousePressed, m_totalTime);
    if (m_btnSave)  m_btnSave->update(mousePos, isMousePressed, m_totalTime);
    if (m_btnBack)  m_btnBack->update(mousePos, isMousePressed, m_totalTime);

    bool isPressingKey = false;

    auto getPlayerGrid = [&]() -> sf::Vector2i {
        float tileSize = m_map.getTileSize();
        sf::Vector2f mapPos = m_map.getPosition();
        sf::Vector2f pPos = m_player.getPosition();
        float offset = m_map.getDynamicOffset();
        float relativeX = pPos.x - mapPos.x + offset + tileSize / 2;
        float relativeY = pPos.y - mapPos.y + offset + tileSize / 2;
        int c = static_cast<int>(std::floor(relativeX / tileSize));
        int r = static_cast<int>(std::floor(relativeY / tileSize));
        return { c, r };
        };

    switch (m_turn) {
    case TurnState::PlayerInput:
    {
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::U) || (m_btnUndo && m_btnUndo->isClicked())) {
            auto old_state = time_machine.undo_state(m_player, m_mummy);
            m_player = old_state.first;
            m_mummy = old_state.second;
            sf::sleep(sf::milliseconds(200));
            return;
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::I) || (m_btnRedo && m_btnRedo->isClicked())) {
            auto future_state = time_machine.redo_state();
            m_player = future_state.first;
            m_mummy = future_state.second;
            sf::sleep(sf::milliseconds(200));
            return;
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::R) || (m_btnReset && m_btnReset->isClicked())) {
            time_machine.push_state(m_player, m_mummy);
            m_map.loadMap(m_currentMapPath, m_player, m_mummy);
            clearSaveData();
            sf::sleep(sf::milliseconds(200));
            return;
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::P) || (m_btnSave && m_btnSave->isClicked())) {
            saveGame();
            sf::sleep(sf::milliseconds(200));
            return;
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Escape) || (m_btnBack && m_btnBack->isClicked())) {
            m_showExitConfirm = true;
            m_isWaitingForMouseRelease = true;
            return;
        }

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Space)) {
            time_machine.push_state(m_player, m_mummy);
            m_turn = TurnState::MummyThinking;
            sf::sleep(sf::milliseconds(200));
            return;
        }

        isPressingKey = (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Left) ||
            sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Right) ||
            sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Up) ||
            sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Down) ||
            sf::Keyboard::isKeyPressed(sf::Keyboard::Key::A) ||
            sf::Keyboard::isKeyPressed(sf::Keyboard::Key::D) ||
            sf::Keyboard::isKeyPressed(sf::Keyboard::Key::W) ||
            sf::Keyboard::isKeyPressed(sf::Keyboard::Key::S));

        if (isPressingKey && !m_player.isMoving()) {
            time_machine.push_state(m_player, m_mummy);
            m_player.processInput(m_map);
            if (m_player.isMoving()) m_turn = TurnState::PlayerMoving;
            else time_machine.undo_state(m_player, m_mummy);
        }
        break;
    }
    case TurnState::PlayerMoving:
        if (!m_player.isMoving()) {
            sf::Vector2i pGrid = getPlayerGrid();
            int mapW = m_map.getWidth();
            int mapH = m_map.getHeight();

            if (pGrid.x < 0 || pGrid.x >= mapW || pGrid.y < 0 || pGrid.y >= mapH) {
                std::cout << ">>> VICTORY! <<<\n";

                m_endGameStatus = 1;
                m_endGameText->setString("VICTORY!");
                m_endGameText->setFillColor(sf::Color::Yellow);

                sf::FloatRect b = m_endGameText->getLocalBounds();
                m_endGameText->setOrigin({ b.size.x / 2.f, b.size.y / 2.f });
                m_endGameText->setPosition({ m_window->getSize().x / 2.f, m_window->getSize().y / 2.f - 50.f });

                clearSaveData();
                return;
            }
            m_turn = TurnState::MummyThinking;
        }
        break;

    case TurnState::MummyThinking:
        m_mummy.move(m_map, m_player);

        if (m_mummy.isMoving()) {
            m_turn = TurnState::MummyMoving;
        }
        else {
            sf::Vector2i pGrid = getPlayerGrid();

            if (m_mummy.getR() == pGrid.y && m_mummy.getC() == pGrid.x) {
                m_endGameStatus = 2;
                m_endGameText->setString("DEFEAT!");
                m_endGameText->setFillColor(sf::Color::Red);

                sf::FloatRect b = m_endGameText->getLocalBounds();
                m_endGameText->setOrigin({ b.size.x / 2.f, b.size.y / 2.f });
                m_endGameText->setPosition({ m_window->getSize().x / 2.f, m_window->getSize().y / 2.f - 50.f });

                clearSaveData();
                return;
            }

            m_turn = TurnState::PlayerInput;
        }
        break;

    case TurnState::MummyMoving:
        if (!m_mummy.isMoving()) {
            sf::Vector2i pGrid = getPlayerGrid();
            if (m_mummy.getR() == pGrid.y && m_mummy.getC() == pGrid.x) {
                m_endGameStatus = 2;
                m_endGameText->setString("YOU DIED!");
                m_endGameText->setFillColor(sf::Color::Red);

                sf::FloatRect b = m_endGameText->getLocalBounds();
                m_endGameText->setOrigin({ b.size.x / 2.f, b.size.y / 2.f });
                m_endGameText->setPosition({ m_window->getSize().x / 2.f, m_window->getSize().y / 2.f - 50.f });

                clearSaveData();
                return;
            }
            m_turn = TurnState::PlayerInput;
        }
        break;
    }
}

void GameState::render(sf::RenderWindow& window) {
    m_map.draw(window, m_player, m_mummy);
    if (m_btnUndo)  m_btnUndo->render(window);
    if (m_btnRedo)  m_btnRedo->render(window);
    if (m_btnReset) m_btnReset->render(window);
    if (m_btnSave)  m_btnSave->render(window);
    if (m_btnBack)  m_btnBack->render(window);

    if (m_showExitConfirm) {
        window.draw(m_darkLayer);
        window.draw(m_popupPanel);
        window.draw(*m_popupText);

        m_btnYes->render(window);
        m_btnNo->render(window);
    }
    if (m_endGameStatus != 0) {
        window.draw(m_darkLayer);
        window.draw(m_popupPanel);

        window.draw(*m_endGameText);

        m_btnRetry->render(window);
        m_btnMenuEnd->render(window);
        m_btnNext->render(window);
    }
}