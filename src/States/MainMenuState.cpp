#include "States/MainMenuState.hpp"
#include <iostream>

MainMenuState::MainMenuState(sf::RenderWindow* window, std::stack<std::unique_ptr<State>>* states)
    : State(window, states)
{
    // Không cần khởi tạo Sprite trong này nữa vì đã dùng std::optional
    initVariables();
    initGui();
}

MainMenuState::~MainMenuState() {
}

void MainMenuState::initVariables() {
    // 1. Load Background
    if (m_bgTexture.loadFromFile("assets/textures/Backgrounds/menu_bg.png")) {
        m_bgTexture.setSmooth(true);
        m_bgSprite.emplace(m_bgTexture);
    }

	// Load font
    if (!m_font.openFromFile("assets/fonts/Akashi.ttf")) {
        std::cerr << "ERROR: Could not load Akashi.ttf\n";
    }

	m_titleText.emplace(m_font, "SYSTEM MAZE", 100);

    // 2. Load Textures cho các nút
    // Bạn nhớ kiểm tra đúng tên file ảnh nhé
    if (!m_txPlay.loadFromFile("assets/textures/Menu/btn_play.png")) {
        std::cerr << "ERROR: Could not load btn_play.png\n";
    }
    if (!m_txCustomize.loadFromFile("assets/textures/Menu/btn_customize.png")) {
        std::cerr << "ERROR: Could not load btn_customize.png\n";
    }
    if (!m_txSettings.loadFromFile("assets/textures/Menu/btn_settings.png")) {
        std::cerr << "ERROR: Could not load btn_settings.png\n";
    }
    if (!m_txHowToPlay.loadFromFile("assets/textures/Menu/btn_howtoplay.png")) {
        std::cerr << "ERROR: Could not load btn_howtoplay.png\n";
    }
    if (!m_txExit.loadFromFile("assets/textures/Menu/btn_exit.png")) {
        std::cerr << "ERROR: Could not load btn_exit.png\n";
    }
    if (!m_txResume.loadFromFile("assets/textures/Menu/btn_resume.png")) {
        std::cerr << "ERROR: Could not load btn_resume.png\n";
	}

    // 3. Gán Texture vào Sprite
    m_btnPlay.emplace(m_txPlay);
    m_btnCustomize.emplace(m_txCustomize);
    m_btnSettings.emplace(m_txSettings);
    m_btnHowToPlay.emplace(m_txHowToPlay);
    m_btnExit.emplace(m_txExit);
	m_btnResume.emplace(m_txResume);

    // --- LOAD SOUND ---
    if (m_buffHover.loadFromFile("assets/audios/hover.wav")) {
        m_sfxHover.emplace(m_buffHover);
        m_sfxHover->setVolume(50.f);
    }
    if (m_buffClick.loadFromFile("assets/audios/click.wav")) {
        m_sfxClick.emplace(m_buffClick);
    }
}

void MainMenuState::initGui() {
    float centerX = m_window->getSize().x / 2.f;
    float startY = 370.f;
	float gapY = 110.f;

    m_titleText->setOrigin({
        m_titleText->getLocalBounds().size.x / 2.f,
        m_titleText->getLocalBounds().size.y / 2.f
    });
	m_titleText->setPosition({ centerX, 200.f });

    // Hàm lambda để setup nút cho nhanh (đỡ viết lặp lại)
    auto setupBtn = [&](std::optional<sf::Sprite>& btn, float xPos, float yPos) {
        if (btn.has_value()) {
            sf::FloatRect bounds = btn->getLocalBounds();
            btn->setOrigin({ bounds.size.x / 2.f, bounds.size.y / 2.f }); // Tâm ở giữa
            btn->setPosition({ xPos, yPos });
        }
    };

	setupBtn(m_btnSettings, m_window->getSize().x - 50.f, 50.f);
	setupBtn(m_btnPlay, centerX, startY + gapY - 30);
    setupBtn(m_btnResume, centerX, startY + gapY * 2);
	setupBtn(m_btnCustomize, centerX, startY + gapY * 3);
	setupBtn(m_btnExit, centerX, startY + gapY * 4);
}

void MainMenuState::updateButtons() {
    sf::Vector2f mousePos = m_window->mapPixelToCoords(sf::Mouse::getPosition(*m_window));
    bool isMousePressed = sf::Mouse::isButtonPressed(sf::Mouse::Button::Left);
    static bool isHandled = false; // Chặn click liên tục

    if (m_isWaitingForMouseRelease) {
        // Trong lúc chờ nhả chuột, ta ép các nút về màu trắng (không cho tối đi)
        if (m_btnPlay.has_value()) m_btnPlay->setColor(sf::Color::White);
        if (m_btnCustomize.has_value()) m_btnCustomize->setColor(sf::Color::White);
        if (m_btnSettings.has_value()) m_btnSettings->setColor(sf::Color::White);
        if (m_btnHowToPlay.has_value()) m_btnHowToPlay->setColor(sf::Color::White);
        if (m_btnExit.has_value()) m_btnExit->setColor(sf::Color::White);
        if (m_btnResume.has_value()) m_btnResume->setColor(sf::Color::White);
        m_isWaitingForMouseRelease = false;
        return; // Thoát hàm ngay, không xử lý hover/click gì cả
    }

    // Danh sách các nút để duyệt vòng lặp (giúp code gọn hơn, dễ xử lý âm thanh)
    // Lưu pair: <Con trỏ tới nút, ID nút>
    std::vector<std::pair<std::optional<sf::Sprite>*, int>> buttons = {
        {&m_btnPlay, 0},
        {&m_btnCustomize, 1},
        {&m_btnSettings, 2},
        {&m_btnHowToPlay, 3},
        {&m_btnExit, 4},
        {&m_btnResume, 5}
    };

    bool anyHover = false;

    // --- LOGIC HIỆU ỨNG (VISUAL) ---
    for (auto& pair : buttons) {
        auto& btnOpt = *pair.first; // Lấy optional sprite
        int id = pair.second;

        if (!btnOpt.has_value()) continue;

        sf::Sprite& btn = *btnOpt; // Lấy sprite thật
        float baseScale = (id == 2) || (id == 3) ? 2.0f : ((id == 0) ? 3.0f : 2.5f); // Nút Play (id 0) to hơn

        // Kiểm tra chuột có nằm trong nút không
        if (btn.getGlobalBounds().contains(mousePos)) {
            anyHover = true;

            // 1. Xử lý Âm thanh Hover (Chỉ kêu 1 lần khi mới lướt vào)
            if (m_lastHoveredButton != id) {
                m_sfxHover->play();
                m_lastHoveredButton = id;
            }

            // 2. Xử lý Hình ảnh
            if (isMousePressed) {
                btn.setScale({ baseScale * 0.9f, baseScale * 0.9f });
                btn.setColor(sf::Color(150, 150, 150));
            }
            else {
                btn.setScale({ baseScale * 1.1f, baseScale * 1.1f });
                btn.setColor(sf::Color::White);
            }
        }
        else {
            // TRẠNG THÁI: BÌNH THƯỜNG (Normal)
            if (id == 0) {
                float breathing = std::sin(m_totalTime * 5.0f) * 0.1f;
                btn.setScale({ baseScale + breathing, baseScale + breathing });
            }
            else {
                btn.setScale({ baseScale, baseScale });
            }
        }
    }

    // Reset biến âm thanh nếu chuột không chạm nút nào
    if (!anyHover) {
        m_lastHoveredButton = -1;
    }

    // --- LOGIC CLICK (ACTION) ---
    if (isMousePressed) {
        if (!isHandled) {
            auto performClickEffect = [&](std::optional<sf::Sprite>& btn, std::function<void()> action) {
                if (btn.has_value()) {
                    // 1. CHƠI ÂM THANH
                    if (m_sfxClick.has_value()) m_sfxClick->play();

                    // 2. ĐỔI HIỆU ỨNG (Thu nhỏ + Tối)
                    float currentScale = btn->getScale().x; // Lấy scale hiện tại (đang hover là 1.1 hoặc 2.2)
                    // Thu nhỏ đi khoảng 20% so với lúc đang hover
                    btn->setScale({ currentScale * 0.9f, currentScale * 0.9f });
                    btn->setColor(sf::Color(150, 150, 150));

                    // 3. FORCE RENDER (ÉP VẼ NGAY LẬP TỨC)
                    // Đây là bước quan trọng nhất để mắt nhìn thấy được
                    m_window->clear();      // Xóa màn hình cũ
                    this->render(*m_window); // Vẽ lại toàn bộ menu (lúc này nút đã bị méo)
                    m_window->display();    // Đẩy lên màn hình ngay

                    // 4. CHỜ XÍU (Để người chơi kịp nhìn thấy nút bị lún xuống)
                    sf::sleep(sf::milliseconds(150));
                    btn->setColor(sf::Color::White);
                    // 5. THỰC HIỆN CHUYỂN MÀN
                    action();
                }
                };

            // --- ÁP DỤNG CHO CÁC NÚT ---

            // 1. PLAY
            if (m_btnPlay.has_value() && m_btnPlay->getGlobalBounds().contains(mousePos)) {
                performClickEffect(m_btnPlay, [&]() {
                    m_states->push(std::make_unique<ModeSelectState>(m_window, m_states));
                    });
            }

            // 2. SETTINGS
            else if (m_btnSettings.has_value() && m_btnSettings->getGlobalBounds().contains(mousePos)) {
                performClickEffect(m_btnSettings, [&]() {
                    m_states->push(std::make_unique<SettingState>(m_window, m_states));
                    });
            }

            // 3. EXIT
            else if (m_btnExit.has_value() && m_btnExit->getGlobalBounds().contains(mousePos)) {
                performClickEffect(m_btnExit, [&]() {
                    m_window->close();
                    });
            }

            // ... Các nút khác tương tự ...

            isHandled = true;
        }
    }
    else {
        isHandled = false;
    }
}

void MainMenuState::update(float dt) {
    m_totalTime += dt;
    updateButtons();
}

void MainMenuState::render(sf::RenderWindow& window) {
    // Vẽ background
    if (m_bgSprite.has_value()) window.draw(*m_bgSprite);

    // Vẽ các nút (Nhớ dùng dấu * để lấy giá trị từ optional)
	if (m_titleText.has_value()) window.draw(*m_titleText);
    if (m_btnPlay.has_value()) window.draw(*m_btnPlay);
    if (m_btnCustomize.has_value()) window.draw(*m_btnCustomize);
    if (m_btnSettings.has_value()) window.draw(*m_btnSettings);
    if (m_btnHowToPlay.has_value()) window.draw(*m_btnHowToPlay);
    if (m_btnExit.has_value()) window.draw(*m_btnExit);
	if (m_btnResume.has_value()) window.draw(*m_btnResume);
}