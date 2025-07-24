#include "events.hpp"
#include <SFML/Window/Event.hpp>
#include <SFML/Graphics.hpp>

void processEvents(sf::Window& window)
{
    while (const std::optional<sf::Event> event = window.pollEvent())
    {
        if (event->is<sf::Event::Closed>())
        {
            window.close();
        }
        else if (event->is<sf::Event::KeyPressed>())
        {
            if (event->getIf<sf::Event::KeyPressed>()->code == sf::Keyboard::Key::Escape)
            {
                window.close();
            }
        }
    }
}
