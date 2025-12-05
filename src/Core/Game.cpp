#include <Core/Game.hpp>
#include <Entities/Player.hpp>

void Game::initVariables() {
	m_window.setFramerateLimit(60); // Giới hạn 60 FPS

	m_map.loadMap("assets/mazes/maze1.txt");
}

void Game::initWindow() {
	// Nhận kích thước Window
	m_videoMode = sf::VideoMode({ 1290, 960 });

	// Tạo cửa sổ
	m_window.create(m_videoMode, "Mummy Game");

	initVariables();
}

Game::Game() {
	this->initWindow();
}

Game::~Game() {

}

const bool Game::isRunning() const {
	return m_window.isOpen();
}

void Game::updateDt() {
	m_dt = m_dtClock.restart().asSeconds();
}

void Game::update() {
	this->updateDt();

	// Xử lý sự kiện
	while (const std::optional event = m_window.pollEvent()) {
		// Kiểm tra nút đóng cửa sổ
		if (event->is<sf::Event::Closed>()) m_window.close();

		// Kiểm tra phím
		if (const auto* keyEvent = event->getIf<sf::Event::KeyPressed>()) {
			if (keyEvent->code == sf::Keyboard::Key::Escape) m_window.close();
		}
	}

	// Update logic game

	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Num1)) {
		// Theme 1: Playmap
		m_map.loadTheme("Playmap");
		m_player.loadTheme("Playmap");
		sf::sleep(sf::milliseconds(200));
	}

	m_player.update(m_dt);
}

void Game::render() {
	m_window.clear(sf::Color::Black); // Xóa frame cũ

	// Vẽ mọi thứ tại đây
	m_map.draw(m_window, m_player);

	m_window.display(); // Hiển thị frame mới
}