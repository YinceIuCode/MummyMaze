#pragma once
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <stack>
#include <memory>

class State {
protected:
    // Con trỏ trỏ ngược về stack trạng thái của Game để có thể push/pop màn hình
    std::stack<std::unique_ptr<State>>* m_states;
    sf::RenderWindow* m_window;

public:
    // Constructor nhận vào cửa sổ và stack trạng thái
    State(sf::RenderWindow* window, std::stack<std::unique_ptr<State>>* states)
        : m_window(window), m_states(states) {
    }

    virtual ~State() {}

    // Các hàm ảo buộc phải định nghĩa lại ở class con
    virtual void update(float dt) = 0;
    virtual void render(sf::RenderWindow& window) = 0;
};