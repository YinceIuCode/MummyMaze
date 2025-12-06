#pragma once
#include <string>

struct GameData {
    // Biến static để truy cập ở mọi nơi mà không cần khởi tạo
    inline static float masterVolume = 100.0f; // 0.0f -> 100.0f
    inline static bool isSoundOn = true;
    inline static std::string currentTheme = "Playmap"; // Mặc định
};