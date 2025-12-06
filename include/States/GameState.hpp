#pragma once
#include "States/State.hpp"
#include "Core/Maze.hpp"
#include "Entities/Player.hpp"
#include "Core/MazeGenerator.hpp"
#include "Core/Maze.hpp"
#include "Core/GameData.hpp"
#include "States/State.hpp"

class GameState : public State {
private:
    Map m_map;
    Player m_player;
    int m_currentMapSize;

    void initVariables();
    void generateNewMaze(int mapsize); // Hàm copy từ Game qua

public:
    GameState(sf::RenderWindow* window, std::stack<std::unique_ptr<State>>* states, int mapSize);
    virtual ~GameState();

    void update(float dt) override;
    void render(sf::RenderWindow& window) override;
};