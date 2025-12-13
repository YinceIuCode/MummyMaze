#pragma once
#include "States/State.hpp"
#include "Core/Maze.hpp"
#include "Entities/Player.hpp"
#include "Entities/Mummy.hpp"
#include "Core/MazeGenerator.hpp"
#include "Core/GameData.hpp"
#include "Core/TimeControl.hpp"

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
    time_controller time_machine;

    int m_currentMapSize;
    string m_currentMapPath = "assets/mazes/maze1.txt";

    void saveGame();
    std::string loadGameData(int& pGridX, int& pGridY, int& mGridX, int& mGridY);

    void initVariables();
    void generateNewMaze(int mapsize); // Hàm copy từ Game qua
    TurnState m_turn;

public:
    GameState(sf::RenderWindow* window, std::stack<std::unique_ptr<State>>* states, std::string mapPath, bool isResuming = false);
    virtual ~GameState();
    

    void update(float dt) override;
    void render(sf::RenderWindow& window) override;
};