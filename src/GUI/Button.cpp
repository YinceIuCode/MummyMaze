#include <GUI/Button.hpp>
#include <iostream>

Button::Button(const sf::Texture& texture, float x, float y, float scale) {
    m_sprite.emplace(texture);
    m_baseScale = scale;
    m_enableBreathing = false;
    m_isHovered = false;
    m_isPressed = false;
    m_hasText = false;
    m_buffHover = nullptr;
    m_buffClick = nullptr;

    // Luôn đặt tâm ở giữa để scale đẹp
    sf::FloatRect bounds = m_sprite->getLocalBounds();
    m_sprite->setOrigin({ bounds.size.x / 2.f, bounds.size.y / 2.f });
    m_sprite->setPosition({ x, y });
    m_sprite->setScale({ scale, scale });
}

Button::~Button() {}

void Button::setSound(const sf::SoundBuffer& hoverBuff, const sf::SoundBuffer& clickBuff) {
    m_buffHover = &hoverBuff;
    m_buffClick = &clickBuff;
}

void Button::setBreathing(bool enable) {
    m_enableBreathing = enable;
}

void Button::update(const sf::Vector2f& mousePos, bool isMousePressed, float totalTime, bool blocked) {
    // Nếu bị chặn (blocked) -> Reset về trạng thái bình thường
    if (blocked) {
        m_isHovered = false;
        m_isPressed = false;
        m_sprite->setColor(sf::Color::White);
        m_sprite->setScale({ m_baseScale, m_baseScale });
        return;
    }

    // 1. Kiểm tra va chạm chuột
    if (m_sprite->getGlobalBounds().contains(mousePos)) {
        // --- LOGIC HOVER ---
        if (!m_isHovered) {
            // Vừa mới lướt chuột vào -> Kêu "Tách"
            m_isHovered = true;
            if (m_buffHover) {
                m_sfx.emplace(*m_buffHover);
                m_sfx->play();
            }
        }

        // --- LOGIC PRESS ---
        if (isMousePressed) {
            m_isPressed = true;
            // Hiệu ứng nhấn: Thu nhỏ + Tối
            m_sprite->setScale({ m_baseScale * 0.9f, m_baseScale * 0.9f });
            m_sprite->setColor(sf::Color(180, 180, 180));
        }
        else {
            m_isPressed = false;
            // Hiệu ứng hover: Phóng to + Sáng
            m_sprite->setScale({ m_baseScale * 1.1f, m_baseScale * 1.1f });
            m_sprite->setColor(sf::Color::White);
        }
    }
    else {
        // --- KHÔNG HOVER ---
        m_isHovered = false;
        m_isPressed = false;
        m_sprite->setColor(sf::Color::White);

        // Hiệu ứng thở (nếu được bật)
        if (m_enableBreathing) {
            float breathing = std::sin(totalTime * 5.0f) * 0.05f;
            m_sprite->setScale({ m_baseScale + breathing, m_baseScale + breathing });
        }
        else {
            m_sprite->setScale({ m_baseScale, m_baseScale });
        }
    }
}

// Kiểm tra xem có đang nhấn trúng nút không (dùng để kích hoạt hành động)
bool Button::isClicked() const {
    return m_isPressed;
}

void Button::render(sf::RenderTarget& target) {
    target.draw(*m_sprite);
}

// Logic Force Render "thần thánh" được đóng gói vào đây
void Button::executeClickEffect(std::function<void()> drawSceneFunc, std::function<void()> onFinished) {
    // 1. Phát tiếng Click
    if (m_buffClick) {
        m_sfx.emplace(*m_buffClick);
        m_sfx->play();
    }

    // 2. Chỉnh Visual thành trạng thái "Đang Nhấn" (Thu nhỏ + Tối)
    // Lưu ý: Lúc này m_sprite đã được update visual mới, nhưng chưa được vẽ lên màn hình
    m_sprite->setScale({ m_baseScale * 0.9f, m_baseScale * 0.9f });
    m_sprite->setColor(sf::Color(150, 150, 150));

    // 3. GỌI HÀM VẼ TỪ BÊN NGOÀI VÀO (QUAN TRỌNG)
    // Hàm này sẽ thực hiện: Clear -> Draw Background -> Draw Buttons (gồm nút này) -> Display
    if (drawSceneFunc) {
        drawSceneFunc();
    }

    // 4. Đóng băng màn hình 0.15s để người chơi kịp thấy nút lún xuống
    sf::sleep(sf::milliseconds(150));

    // 5. Chuyển màn
    if (onFinished) {
        onFinished();
    }
}