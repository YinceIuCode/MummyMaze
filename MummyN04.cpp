#include <Core/Game.hpp>

int main() {
	// Khởi tạo
	Game game;

	// Game Loop
	while (game.isRunning()) {
		game.update();
		game.render();
	}

	return 0;
}