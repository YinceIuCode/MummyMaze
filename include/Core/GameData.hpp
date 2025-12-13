#pragma once
#include <SFML/Audio.hpp>

struct GameData {
    // --- SETTINGS (Âm thanh) ---
    static float musicVolume; // 0.0f -> 100.0f
    static bool isSfxMuted;
    static bool isMuted;      // Tắt tiếng hoàn toàn

    // --- CUSTOMIZE (Giao diện) ---
    static int currentTheme; 

    // --- OBJECTS (Đối tượng sống dai nhất game) ---
    static sf::Music bgMusic; // Nhạc nền (để đây để Settings chỉnh được volume)
};