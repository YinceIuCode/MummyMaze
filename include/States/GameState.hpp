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

    std::unique_ptr<Button> m_btnUndo;
    std::unique_ptr<Button> m_btnRedo;
    std::unique_ptr<Button> m_btnReset;
    std::unique_ptr<Button> m_btnSave;
    std::unique_ptr<Button> m_btnBack; // Nút Exit/Back

    sf::Texture m_txUndo, m_txRedo, m_txReset, m_txSave, m_txBack, m_txYes, m_txNo;

    float m_totalTime = 0.0f; // Để nút có hiệu ứng thở
    bool m_isWaitingForMouseRelease = true; // Chặn click thừa

    bool m_showExitConfirm;       // Cờ kiểm tra xem có đang hiện bảng hỏi không
    sf::RectangleShape m_darkLayer; // Lớp màn hình đen mờ che game lại
    sf::RectangleShape m_popupPanel; // Cái bảng nền thông báo
    std::optional<sf::Text> m_popupText;         // Dòng chữ "Do you want to save?"

    std::unique_ptr<Button> m_btnYes;
    std::unique_ptr<Button> m_btnNo;

    int m_endGameStatus; // 0: Đang chơi, 1: Thắng (Win), 2: Thua (Defeat)

    std::optional<sf::Text> m_endGameText; // Chữ hiện "VICTORY" hoặc "DEFEAT"

    // Nút cho màn hình kết thúc
    std::unique_ptr<Button> m_btnRetry;   // Nút chơi lại
    std::unique_ptr<Button> m_btnMenuEnd;

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