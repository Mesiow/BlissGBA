#include "imgui-SFML.h"

#include <SFML\Graphics.hpp>
#include <SFML/System/Clock.hpp>
#include <SFML/Window/Event.hpp>

#include "Core\Emulator.h"

int main(int arc, char* argv[]) {
    sf::RenderWindow window(sf::VideoMode(240 * 4, 160 * 4), "BlissGBA");

    sf::Image icon;
    icon.loadFromFile("icon.png");
    window.setIcon(icon.getSize().x, icon.getSize().y, icon.getPixelsPtr());

    ImGui::SFML::Init(window);

    Emulator emu(&window);

    sf::Clock deltaClock;
    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            ImGui::SFML::ProcessEvent(event);
     
            if (event.type == sf::Event::Closed) {
                window.close();
            }
        }

        ImGui::SFML::Update(window, deltaClock.restart());

        emu.run();

        window.clear(sf::Color(179, 216, 229, 255));

        emu.render();

        ImGui::SFML::Render(window);

        window.display();
    }

    ImGui::SFML::Shutdown();

    return 0;
}