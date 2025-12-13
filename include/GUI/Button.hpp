#pragma once
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <functional>
#include <cmath>

class Button {
private:
    // --- VISUALS ---
    std::optional<sf::Sprite> m_sprite;

    // --- STATE ---
    bool m_isHovered;
    bool m_isPressed;
    float m_baseScale;    // Kích thước gốc (1.0 hoặc 2.0)
    
    // --- SETTINGS ---
    bool m_enableBreathing; // Có bật hiệu ứng "thở" khi đứng yên không?

    // --- SOUNDS ---
    // Lưu con trỏ tới Buffer (vì Buffer phải được load ở State/ResourceHolder)
    const sf::SoundBuffer* m_buffHover;
    const sf::SoundBuffer* m_buffClick;
    std::optional<sf::Sound> m_sfx;

public:
    // Constructor: Texture, Tọa độ, Scale gốc
    Button(const sf::Texture& texture, float x, float y, float scale = 1.0f);
    ~Button();

    // Cài đặt âm thanh (Truyền Buffer từ bên ngoài vào)
    void setSound(const sf::SoundBuffer& hoverBuff, const sf::SoundBuffer& clickBuff);

    // Bật tắt hiệu ứng "Thở" (Dùng cho nút Play to)
    void setBreathing(bool enable);

    // Cập nhật logic (Gọi mỗi frame)
    // mousePos: Vị trí chuột
    // isMousePressed: Chuột có đang nhấn không
    // totalTime: Thời gian tổng (để tính sin wave cho hiệu ứng thở)
    // blocked: Nếu true thì nút bị vô hiệu hóa (dùng để chặn click thừa)
    void update(const sf::Vector2f& mousePos, bool isMousePressed, float totalTime, bool blocked = false);

    // Kiểm tra xem nút có vừa được bấm xong không?
    bool isClicked() const;

    // Vẽ nút
    void render(sf::RenderTarget& target);

    // Getters/Setters phụ trợ
    sf::Vector2f getPosition() const { return m_sprite->getPosition(); }
    void setPosition(float x, float y) { m_sprite->setPosition({ x, y }); }
    sf::FloatRect getGlobalBounds() const { return m_sprite->getGlobalBounds(); }
    
    // Hàm đặc biệt: Thực hiện hiệu ứng Click (Thu nhỏ -> Vẽ -> Chờ -> Callback)
    // Hàm này giúp tái sử dụng logic "Force Render" ở mọi nơi
    void executeClickEffect(std::function<void()> drawSceneFunc, std::function<void()> onFinished);
};