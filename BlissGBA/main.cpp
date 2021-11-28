#include "imgui-SFML.h"

#include <SFML\Graphics.hpp>
#include <SFML/System/Clock.hpp>
#include <SFML/Window/Event.hpp>

#include "Core\Emulator.h"


int main(int arc, char* argv[]) {
    float displayScaleFactor = 6;
    std::string title = "BlissGBA";
    sf::RenderWindow window(sf::VideoMode(240 * displayScaleFactor, 160 * displayScaleFactor), title);

    sf::Image icon;
    icon.loadFromFile("icon.png");
    window.setIcon(icon.getSize().x, icon.getSize().y, icon.getPixelsPtr());

    ImGui::SFML::Init(window);

    Emulator *emu = new Emulator(&window, displayScaleFactor);

    sf::Clock deltaClock;
    sf::Time prevTime = deltaClock.getElapsedTime();
    sf::Time currentTime;
    float lastTime = 0;
    float fps;
    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            ImGui::SFML::ProcessEvent(event);
     
            emu->handleEvents(event);
            if (event.type == sf::Event::Closed) {
                window.close();
            }
        }

        currentTime = deltaClock.getElapsedTime();
        fps = 1.0f / (currentTime.asSeconds() - prevTime.asSeconds());
        window.setTitle(title + " FPS: " + std::to_string(floor(fps)));
        
        ImGui::SFML::Update(window, currentTime);

        emu->handleInput();
        emu->run();

        window.clear(sf::Color(179, 216, 229, 255));

        emu->render(window);

        ImGui::SFML::Render(window);

        window.display();

        prevTime = currentTime;
    }

    delete emu;
    ImGui::SFML::Shutdown();
    
    return 0;
}