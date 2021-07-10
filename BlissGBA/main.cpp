#include "imgui-SFML.h"

#include <SFML\Graphics.hpp>
#include <SFML/System/Clock.hpp>
#include <SFML/Window/Event.hpp>

#include "Memory\MemoryBus.h"
#include "Debugger\DebugUI.h"

int main(int arc, char* argv[]) {
    sf::RenderWindow window(sf::VideoMode(240 * 4, 160 * 4), "BlissGBA");

    sf::Image icon;
    icon.loadFromFile("icon.png");
    window.setIcon(icon.getSize().x, icon.getSize().y, icon.getPixelsPtr());
    window.setVerticalSyncEnabled(true);

    ImGui::SFML::Init(window);

    MemoryBus mbus;
    Arm cpu(&mbus);
    DebugUI debug(&cpu);

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

        window.clear(sf::Color(179, 216, 229, 255));
        
        debug.render();
        ImGui::SFML::Render(window);

        window.display();
    }

    ImGui::SFML::Shutdown();

    return 0;
}