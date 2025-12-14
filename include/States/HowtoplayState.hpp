#pragma once
#include "States/State.hpp"
#include <vector>

class HowToPlayState : public State {
private:
    // --- GIAO DIỆN ---
    sf::Texture m_bgTexture;
    std::optional<sf::Sprite> m_bgSprite;
    sf::Font m_font;

    std::optional<sf::Text> m_title;
    std::optional<sf::Text> m_btnBack; // Nút Back dạng chữ (giống Settings)

    // Khung chứa nội dung (Hộp đen mờ)
    sf::RectangleShape m_container;

    // Danh sách các dòng chữ hướng dẫn
    // Pair: first = Phím (Màu vàng), second = Mô tả (Màu trắng)
    std::vector<std::pair<sf::Text, sf::Text>> m_instructions;

    // --- ÂM THANH ---
    sf::SoundBuffer m_buffClick;

    // --- HÀM ---
    void initVariables();
    void initGui();
    void updateInput(float dt);

public:
    HowToPlayState(sf::RenderWindow* window, std::stack<std::unique_ptr<State>>* states);
    virtual ~HowToPlayState();

    void update(float dt) override;
    void render(sf::RenderWindow& window) override;
};