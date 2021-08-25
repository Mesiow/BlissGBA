#include "imgui-SFML.h"

#include <SFML\Graphics.hpp>
#include <SFML/System/Clock.hpp>
#include <SFML/Window/Event.hpp>

#include "Core\Emulator.h"

int main(int arc, char* argv[]) {

    float displayScaleFactor = 4;
    sf::RenderWindow window(sf::VideoMode(240 * displayScaleFactor, 160 * displayScaleFactor), "BlissGBA");

    sf::Image icon;
    icon.loadFromFile("icon.png");
    window.setIcon(icon.getSize().x, icon.getSize().y, icon.getPixelsPtr());

    ImGui::SFML::Init(window);

    Emulator emu(&window, displayScaleFactor);

    sf::Clock deltaClock;
    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            ImGui::SFML::ProcessEvent(event);
     
            emu.handleEvents(event);
            if (event.type == sf::Event::Closed) {
                window.close();
            }
        }

        ImGui::SFML::Update(window, deltaClock.restart());

        emu.run();

        window.clear(sf::Color(179, 216, 229, 255));

        emu.render(window);

        ImGui::SFML::Render(window);

        window.display();
    }

    ImGui::SFML::Shutdown();

    return 0;
}