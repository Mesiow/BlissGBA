#include "DebugUI.h"

MemoryEditor DebugUI::biosMemory;
MemoryEditor DebugUI::vramEditor;
MemoryEditor DebugUI::ioEditor;
MemoryEditor DebugUI::gamepakMemory;

DebugUI::DebugUI(sf::RenderWindow *window, MemoryBus *mbus, Arm* cpu)
	:window(window), mbus(mbus), cpu(cpu)
{
    showRegisterWindow = true;
    showBiosMemory = false;
    showVRAM = true;
    showIO = true;
    showGamePakMemory = false;
    showCartWindow = true;
    showPPUWindow = false;
    showPipeline = true;
    showDisplay = true;
    vsync = false;
}

void DebugUI::render()
{
    renderMenuBar();
	renderRegisters();
    renderCartInfo();
    renderPipeline();
    renderDisplay();

    if (showBiosMemory) {
        biosMemory.DrawWindow("Bios Memory", mbus->getBiosMemory(), BIOS_SIZE);
    }
    if (showVRAM) {
        vramEditor.DrawWindow("VRAM", mbus->getVRAM(), VRAM_SIZE);
    }
    if (showIO) {
        ioEditor.DrawWindow("IO Registers", mbus->getIO(), IO_SIZE);
    }
    if (showGamePakMemory) {
        if (mbus->getGamePakMemory() != nullptr) {
            gamepakMemory.DrawWindow("GamePak Memory", mbus->getGamePakMemory(), GAMEPAK_WS_SIZE);
        }
    }
}

void DebugUI::renderRegisters()
{
    if (showRegisterWindow) {
        ImGui::Begin("General");
        ImGui::SetWindowFontScale(1.2);

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

            bool zero = cpu->getFlag(Z);
            bool sign = cpu->getFlag(N);
            bool carry = cpu->getFlag(C);
            bool overflow = cpu->getFlag(V);

            ImGui::Checkbox("Zero ", &zero);
            ImGui::SameLine();
            ImGui::Checkbox("Sign ", &sign);
            ImGui::SameLine();
            ImGui::Checkbox("Carry", &carry);
            ImGui::SameLine();
            ImGui::Checkbox("Overflow", &overflow);
            ImGui::SameLine();
            ImGui::NewLine();
        }

        {
            ImGui::NewLine();
            ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(ImColor(255, 242, 0, 255)));
            ImGui::Text("State");
            ImGui::PopStyleColor();
            
            bool arm = (cpu->getState() == State::ARM);
            if (arm) {
                ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(ImColor(0, 242, 0, 255)));
                ImGui::Text("ARM");
                ImGui::PopStyleColor();
            }
            else {
                ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(ImColor(128, 128, 136, 255)));
                ImGui::Text("ARM");
                ImGui::PopStyleColor();
            }
            
            if (!arm) {
                ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(ImColor(0, 242, 0, 255)));
                ImGui::Text("THUMB");
                ImGui::PopStyleColor();
            }
            else {
                ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(ImColor(128, 128, 136, 255)));
                ImGui::Text("THUMB");
                ImGui::PopStyleColor();
            }
        }
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
        cpu->clock();
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
                std::string path = std::filesystem::path(file).string();
                mbus->loadGamePak(path);
                //gba/core.reset();
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
            ImGui::MenuItem("Show Pipeline", nullptr, &showPipeline);
            ImGui::MenuItem("Show PPU registers", nullptr, &showPPUWindow);
            ImGui::MenuItem("Show Bios memory", nullptr, &showBiosMemory);
            ImGui::MenuItem("Show GamePak memory", nullptr, &showGamePakMemory);
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
        ImGui::SetWindowFontScale(1.2);

        ImGui::Text("Game Title: %s", mbus->pak.title.c_str());
        ImGui::Text("ROM Size: %s", mbus->pak.romSize.c_str());

        ImGui::End();
    }
}

void DebugUI::renderPipeline()
{
    if (showPipeline) {
        u32 first_ins = cpu->armpipeline[0];
        u32 second_ins = cpu->armpipeline[1];

        ImGui::Begin("Pipeline");
        ImGui::SetWindowFontScale(1.2);
        
        ImGui::Text("0 -> 0x%08X", first_ins);
        ImGui::Text("1 -> 0x%08X", second_ins);

        ImGui::End();
    }
}

void DebugUI::renderDisplay()
{
    if (showDisplay) {
        ImGui::Begin("Display");

        sf::Sprite sprite;
        sprite.setColor(sf::Color::White);
        ImGui::Image(0, ImGui::GetContentRegionAvail());
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