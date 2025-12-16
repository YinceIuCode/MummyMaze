#pragma once

#include <SFML/Graphics.hpp>
#include <stack>
#include <memory>
#include "../../include/States/State.hpp"
#include "../../include/Core/GameData.hpp"

class Game {
private:
    // --- Quản lý Cửa sổ ---
    sf::RenderWindow m_window;
    sf::VideoMode m_videoMode;

    // --- Quản lý Thời gian (Delta Time) ---
    sf::Clock m_dtClock;
    float m_dt;

    // --- QUAN TRỌNG: Stack chứa các màn hình (Menu, Game, Pause...) ---
    // Dùng unique_ptr để tự động quản lý bộ nhớ, không cần delete thủ công
    std::stack<std::unique_ptr<State>> m_states;

    // --- Hàm khởi tạo nội bộ ---
    void initWindow();
    void initStates(); 
    void initMusic();

public:
    // --- Constructor / Destructor ---
    Game();
    virtual ~Game();

    // --- Accessors ---
    const bool isRunning() const; // Kiểm tra xem cửa sổ còn mở không

    // --- Core Functions ---
    void updateDt(); // Tính toán thời gian trôi qua giữa 2 frame
    void update();   // Update logic của State hiện tại
    void render();   // Vẽ State hiện tại
};