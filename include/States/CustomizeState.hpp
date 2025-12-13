#pragma once
#include "States/State.hpp"
#include "GUI/Button.hpp"
#include "Core/GameData.hpp"
#include <vector>

// Struct giờ chỉ cần 2 trường này thôi
struct ThemeInfo {
    std::string name;       // Tên hiển thị (VD: "ANCIENT EGYPT")
    std::string folderName; // Tên folder (VD: "Egypt")
};

class CustomizeState : public State {
private:
    // --- VARIABLES ---
    sf::Texture m_bgTexture;
    std::optional<sf::Sprite> m_bgSprite;
    sf::Font m_font;
    std::optional<sf::Text> m_title;
    std::optional<sf::Text> m_btnBack; // Nút Back dạng Text

    // --- DATA ---
    std::vector<ThemeInfo> m_themes;
    int m_previewIndex = 0;

    // --- PREVIEW AREA ---
    sf::Texture m_txPreviewPlayer;
    sf::Texture m_txPreviewMummy;
    std::optional<sf::Sprite> m_sprPreviewPlayer;
    std::optional<sf::Sprite> m_sprPreviewMummy;
    std::optional<sf::Text> m_themeNameText;

    // --- NAVIGATION ---
    sf::Texture m_txArrowLeft, m_txArrowRight;
    std::unique_ptr<Button> m_btnPrev;
    std::unique_ptr<Button> m_btnNext;
    sf::SoundBuffer m_buffClick;

    // --- FUNCTIONS ---
    void initVariables();
    void initGui();
    void loadThemePreview(int index); // Hàm này sẽ tự ghép chuỗi đường dẫn
    void updateButtons();

public:
    CustomizeState(sf::RenderWindow* window, std::stack<std::unique_ptr<State>>* states);
    virtual ~CustomizeState();

    void update(float dt) override;
    void render(sf::RenderWindow& window) override;
};