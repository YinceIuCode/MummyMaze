#include "../../include/Core/GameData.hpp"

// Khởi tạo giá trị mặc định
float GameData::musicVolume = 50.0f; // Nhạc nền 50%
bool GameData::isSfxMuted = false; // Mặc định là bật (false)
bool GameData::isMuted = false;

int GameData::currentTheme = 0; // Mặc định là Theme 0 (Ai Cập)

sf::Music GameData::bgMusic; // Biến nhạc nền