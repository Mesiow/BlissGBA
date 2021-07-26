#include "DebugUI.h"

MemoryEditor DebugUI::mainMemory;
MemoryEditor DebugUI::biosMemory;
MemoryEditor DebugUI::gamepakMemory;

DebugUI::DebugUI(sf::RenderWindow *window, MemoryBus *mbus, Arm* cpu)
	:window(window), mbus(mbus), cpu(cpu)
{
    showRegisterWindow = true;
    showBiosMemory = false;
    showGamePakMemory = true;
    showCartWindow = true;
    showPPUWindow = false;
    vsync = false;
}

void DebugUI::render()
{
    renderMenuBar();
	renderRegisters();
    renderCartInfo();

    if (showBiosMemory) {
        biosMemory.DrawWindow("Bios Memory", mbus->getBiosMemory(), BIOS_SIZE);
    }
    if (showGamePakMemory) {
        gamepakMemory.DrawWindow("GamePak Memory", mbus->getGamePakMemory(), GAMEPAK_WS_SIZE);
    }
}

void DebugUI::renderRegisters()
{
    if (showRegisterWindow) {
        ImGui::Begin("General");
        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(ImColor(255, 242, 0, 255)));
        ImGui::Text("Registers");
        ImGui::PopStyleColor();

        std::string str;
        for (s32 i = 0; i < NUM_REGISTERS; i++) {
            str = "R" + std::to_string(i) + ": 0x%08X";
            RegisterID id{ i };
            ImGui::Text(str.c_str(), cpu->getRegister(id));
        }
        ImGui::Text("R13(SP): 0x%08X", cpu->getRegister(RegisterID{ R13_ID }));
        ImGui::Text("R14(LR): 0x%08X", cpu->getRegister(RegisterID{ R14_ID }));
        ImGui::Text("R15(PC): 0x%08X", cpu->getRegister(RegisterID{ R15_ID }));

        ImGui::NewLine();
        ImGui::Text("CPSR: 0x%08X", cpu->getPSR().CPSR);
        ImGui::Text("SPSR: 0x%08X", cpu->getPSR().SPSR);

        {
            ImGui::NewLine();
            ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(ImColor(255, 242, 0, 255)));
            ImGui::Text("Flags");
            ImGui::PopStyleColor();

            ImGui::Checkbox("Zero ", &zero);
            ImGui::SameLine();
            ImGui::Checkbox("Sign ", &sign);
            ImGui::SameLine();
            ImGui::Checkbox("Carry", &carry);
            ImGui::SameLine();
            ImGui::Checkbox("Overflow", &overflow);
            ImGui::SameLine();
        }

        ImGui::NewLine();
        renderButtons();

        ImGui::End();
    }
}

void DebugUI::renderButtons()
{
    ImGui::NewLine();
    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(ImColor(255, 242, 0, 255)));
    ImGui::Text("Emulation");
    ImGui::PopStyleColor();
 
    if (ImGui::Button("Run")) {

    }
    ImGui::SameLine();
    if (ImGui::Button("Step")) {

    }
    ImGui::SameLine();
    if (ImGui::Button("Pause")) {

    }
    ImGui::SameLine();
    if (ImGui::Button("Reset")) {

    }
}

void DebugUI::renderMenuBar()
{
    static const char* fileTypes[] = { "*.gba", "*.bin", "*.zip", "*.rar", "*.7z" };

    if (ImGui::BeginMainMenuBar()) {
        if (ImGui::MenuItem("Open ROM")) {
            auto file = tinyfd_openFileDialog(
                "GBA ROM",
                "",
                5,
                fileTypes,
                "GBA ROMs",
                0);

            if (file != nullptr) {  // Check if file dialog was canceled
                auto path = std::filesystem::path(file);
                //Memory::loadROM(path);
                //g_snes.reset();
            }
        }

        if (ImGui::BeginMenu("Emulation")) {
            //bool cartInserted = Memory::cart.mapper != Mappers::NoCart;

            //if (ImGui::MenuItem("Trace", nullptr) && cartInserted) // Make sure not to run without cart
            //    gba.step();
            //if (ImGui::MenuItem("Run", nullptr, &running)) // Same here
            //    running = running ? cartInserted : false;
            //if (ImGui::MenuItem("Pause", nullptr) && running) {
            //    waitEmuThread(); // Wait till the frame finishes
            //    running = false; // Stop running
            //}

            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("Debug")) {
            ImGui::MenuItem("General Debug Info", nullptr, &showRegisterWindow);
            ImGui::MenuItem("Show Cart Info", nullptr, &showCartWindow);
            ImGui::MenuItem("Show PPU registers", nullptr, &showPPUWindow);
            ImGui::MenuItem("Show Bios memory", nullptr, &showBiosMemory);
            ImGui::MenuItem("Show GamePak memory", nullptr, &gamepakMemory);
            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("Configuration")) {
            if (ImGui::MenuItem("Vsync", nullptr, &vsync))
                window->setFramerateLimit(vsync ? 60 : 0);

            ImGui::End();
        }

        ImGui::EndMainMenuBar();
    }
}

void DebugUI::renderCartInfo()
{
    if (showCartWindow) {
        ImGui::Begin("Cartridge Info");

        ImGui::Text("Game Title: %s", mbus->pak.title.c_str());
        ImGui::Text("ROM Size: %s", mbus->pak.romSize.c_str());

        ImGui::End();
    }
}

void DebugUI::update()
{
}

void DebugUI::handleButtonPresses()
{
}

void DebugUI::handleEvents(sf::Event& ev)
{
}
