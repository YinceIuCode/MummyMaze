#pragma once
#include "States/State.hpp"
#include "Core/Maze.hpp"
#include "Entities/Player.hpp"
#include "Entities/Mummy.hpp"
#include "Core/MazeGenerator.hpp"
#include "Core/GameData.hpp"
#include "Core/TimeControl.hpp"
#include "GUI/Button.hpp"

enum class TurnState {
    PlayerInput,
    PlayerMoving,
    MummyThinking,
    MummyMoving
};

class GameState : public State {
private:
    Map m_map;
    Player m_player;
    Mummy m_mummy;
    time_controller time_machine;

    int m_currentMapSize;
    std::string m_currentMapPath = "assets/mazes/maze1.txt";

    std::unique_ptr<Button> m_btnUndo;
    std::unique_ptr<Button> m_btnRedo;
    std::unique_ptr<Button> m_btnReset;
    std::unique_ptr<Button> m_btnSave;
    std::unique_ptr<Button> m_btnBack;

    sf::Texture m_txUndo, m_txRedo, m_txReset, m_txSave, m_txBack, m_txYes, m_txNo, m_txNext;

    float m_totalTime = 0.0f;
    bool m_isWaitingForMouseRelease = true;

    bool m_showExitConfirm;
    sf::RectangleShape m_darkLayer;
    sf::RectangleShape m_popupPanel;
    std::optional<sf::Text> m_popupText;

    std::unique_ptr<Button> m_btnYes;
    std::unique_ptr<Button> m_btnNo;

    int m_endGameStatus;
    std::optional<sf::Text> m_endGameText;

    std::unique_ptr<Button> m_btnRetry;
    std::unique_ptr<Button> m_btnMenuEnd;
    std::unique_ptr<Button> m_btnNext;

    TurnState m_turn;

    void saveGame();
    std::string loadGameData(int& pGridX, int& pGridY, int& mGridX, int& mGridY);
    void initVariables();
    void generateNewMaze(int mapsize);

public:
    GameState(sf::RenderWindow* window, std::stack<std::unique_ptr<State>>* states, std::string mapPath, bool isResuming = false);
    virtual ~GameState();

    void update(float dt) override;
    void render(sf::RenderWindow& window) override;
};