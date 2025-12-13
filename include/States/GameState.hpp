#pragma once
#include "States/State.hpp"
#include "Core/Maze.hpp"
#include "Entities/Player.hpp"
#include "Entities/Mummy.hpp"
#include "Core/MazeGenerator.hpp"
#include "Core/Maze.hpp"
#include "Core/GameData.hpp"
#include "States/State.hpp"

enum class TurnState {
    PlayerInput,   // Chờ người chơi bấm nút
    PlayerMoving,  // Người chơi đang trượt tới ô mới
    MummyThinking, // Tính toán đường đi cho Mummy
    MummyMoving    // Mummy đang trượt tới ô mới
};

class GameState : public State {
private:
    Map m_map;
    Player m_player;
	Mummy m_mummy;
    int m_currentMapSize;
    bool m_isWin = false;
	bool m_isDefeat = false;

    void initVariables();
    void generateNewMaze(int mapsize); // Hàm copy từ Game qua
    TurnState m_turn;

public:
    GameState(sf::RenderWindow* window, std::stack<std::unique_ptr<State>>* states, int mapSize);
    virtual ~GameState();

    void update(float dt) override;
    void render(sf::RenderWindow& window) override;
    bool isWin() { return m_isWin; }
	bool isDefeat() { return m_isDefeat; }
};