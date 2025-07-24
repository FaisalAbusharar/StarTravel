#pragma once
#include "SFML/System/Vector2.hpp"


namespace conf {
    //Window Configuration
    constexpr sf::Vector2u window_size = {1920, 1080};
    constexpr auto window_size_f = static_cast<sf::Vector2f>(window_size);
    constexpr uint32_t max_framerate = 144;
    constexpr float dt = 1.0f / static_cast<float>(max_framerate);

    //star configuration
    constexpr uint32_t count = 10000;
    constexpr float radius = 20.0f;
    constexpr float far = 10.0f;
    constexpr float near = 0.1f;
    constexpr float speed = 1.0f;

}
