#include <algorithm>
#include <SFML/Graphics.hpp>
#include <SFML/Window/Window.hpp>
#include <random>
#include "events.hpp"
#include "configuration.hpp"
#include "star.hpp"

std::vector<Star> createStars(uint32_t count, float scale) {
    std::vector<Star> stars;
    stars.reserve(count);

    // Random nums generator
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<float> distribution(0.0f,   1.0f);

    // Define a star free zone
    sf::Vector2f const window_world_size = conf::window_size_f * conf::near;
    sf::FloatRect const star_free_zone = {-window_world_size * 0.5f, window_world_size};

    // Create random stars distributed on the screen
    for (uint32_t i{count}; i--;)
    {
        float const x = (distribution(gen) - 0.5f) * conf::window_size_f.x * scale;
        float const y = (distribution(gen) - 0.5f) * conf::window_size_f.y * scale;
        float const z = distribution(gen) * (conf::far - conf::near) + conf::near;

        // Discord any star that falls in the zone
        if (star_free_zone.contains({ x, y })) {
            ++i;
            continue;
        }
        // Otherwise add it in the vector
        stars.push_back({{x,y}, z});

    }


    // Prevent stars from overlapping
    std::sort(stars.begin(), stars.end(), [](Star const& s_1, Star const& s_2) {
        return s_1.z > s_2.z;
    });

    return stars;
}

void updateGeometry(uint32_t idx, Star const& s, sf::VertexArray& va)
{
    float const scale = 1.0f / s.z;
    float const depth_ratio = (s.z - conf::near) / (conf::far - conf::near);
    float const color_ratio = 1.0f - depth_ratio;
    auto const c = static_cast<uint8_t>(color_ratio * 255);

    sf::Vector2f const position = s.position * scale;
    float const r = conf::radius * scale;

    sf::Vector2f p0 = {position.x - r, position.y - r};
    sf::Vector2f p1 = {position.x + r, position.y - r};
    sf::Vector2f p2 = {position.x + r, position.y + r};
    sf::Vector2f p3 = {position.x - r, position.y + r};

    uint32_t base = 6 * idx;

    sf::Color const color{c, c, c};

    // Triangle 1
    va[base + 0].position = p0;
    va[base + 1].position = p1;
    va[base + 2].position = p2;

    // Triangle 2
    va[base + 3].position = p2;
    va[base + 4].position = p3;
    va[base + 5].position = p0;

    for (int i = 0; i < 6; ++i)
        va[base + i].color = color;
}




int main()
{
    auto window = sf::RenderWindow(sf::VideoMode({conf::window_size.x, conf::window_size.y}), "Stary Night", sf::Style::None);
    window.setFramerateLimit(conf::max_framerate);
    window.setMouseCursorVisible(false);

    sf::Texture texture;
    texture.loadFromFile("res/star.png");
    texture.setSmooth(true);
    texture.generateMipmap();

    std::vector<Star> stars = createStars(conf::count, conf::far);

    sf::VertexArray va{sf::PrimitiveType::Triangles, 6 * conf::count};
    auto const texture_size_f = static_cast<sf::Vector2f>(texture.getSize());

    for (uint32_t idx{conf::count}; idx--;)
    {
        uint32_t base = 6 * idx;

        // Quad corners tex coords
        sf::Vector2f t0 = {0.0f, 0.0f};
        sf::Vector2f t1 = {texture_size_f.x, 0.0f};
        sf::Vector2f t2 = {texture_size_f.x, texture_size_f.y};
        sf::Vector2f t3 = {0.0f, texture_size_f.y};

        // Triangle 1
        va[base + 0].texCoords = t0;
        va[base + 1].texCoords = t1;
        va[base + 2].texCoords = t2;

        // Triangle 2
        va[base + 3].texCoords = t2;
        va[base + 4].texCoords = t3;
        va[base + 5].texCoords = t0;
    }


    uint32_t first = 0;

    while (window.isOpen())
    {
        processEvents(window);

        // Fake travel toward increasing Z
        for (uint32_t i{conf::count}; i--;) {

            Star& s = stars[i];
            s.z -= conf::speed * conf::dt;
            if (s.z < conf::near)
            {
                // Offset the star by the excess travel it made behind near to keep spacing constant
                // This basically resets the star to behind the scene, instead of generating new stars
                s.z = conf::far - (conf::near - s.z);

                // The star is now the first we need to draw, since it's the furthest away.
                first = i;
            }
        }

        //Render Section
        window.clear();

        sf::CircleShape shape{conf::radius};
        shape.setOrigin(sf::Vector2f(conf::radius, conf::radius));
        for (uint32_t i{0}; i < conf::count; ++i)  {

            uint32_t const idx = (i + first) % conf::count;
            Star& s = stars[idx];
            updateGeometry(i, s, va);

        }

        // sf::CircleShape c{100.0f};
        // c.setFillColor(sf::Color::Green);
        // window.draw(c);

        sf::RenderStates states;
        states.transform.translate(conf::window_size_f * 0.5f);
        states.texture = &texture;
        window.draw(va, states);

        window.display();
    }
}
