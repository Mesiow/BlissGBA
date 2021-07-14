#include "DebugUI.h"

MemoryEditor DebugUI::mainMemory;
MemoryEditor DebugUI::biosMemory;

DebugUI::DebugUI(sf::RenderWindow *window, MemoryBus *mbus, Arm* cpu)
	:window(window), mbus(mbus), cpu(cpu)
{
    showRegisterWindow = true;
    showBiosMemory = true;
    vsync = false;
}

void DebugUI::render()
{
    renderMenuBar();
	renderRegisters();

    if (showBiosMemory) {
        biosMemory.DrawWindow("Bios Memory", mbus->getBiosMemory(), BIOS_SIZE);
    }
}

void DebugUI::renderRegisters()
{
    if (showRegisterWindow) {
        ImGui::Begin("Register Window");
        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(ImColor(255, 242, 0, 255)));
        ImGui::Text("Registers");
        ImGui::PopStyleColor();

        std::string str;
        for (s32 i = 0; i < NUM_REGISTERS; i++) {
            str = "R" + std::to_string(i) + ": 0x%08X";
            ImGui::Text(str.c_str(), cpu->getRegister(i));
        }
        ImGui::Text("R13(SP): 0x%08X", cpu->getRegister(R13_ID));
        ImGui::Text("R14(LR): 0x%08X", cpu->getRegister(R14_ID));
        ImGui::Text("R15: 0x%08X", cpu->getRegister(R15_ID));

        ImGui::NewLine();
        ImGui::Text("PC: 0x%08X", cpu->getPC());
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

        ImGui::End();
    }
}

void DebugUI::renderMenuBar()
{
    static const char* fileTypes[] = { "*.sfc", "*.smc", "*.snes", "*.zip", "*.rar", "*.7z" };

    if (ImGui::BeginMainMenuBar()) {
        if (ImGui::MenuItem("Open ROM")) {
            //auto file = tinyfd_openFileDialog(
            //    "Gib SNES ROM",
            //    "",
            //    6,
            //    fileTypes,
            //    "SNES ROMs",
            //    0);

            //if (file != nullptr) {  // Check if file dialog was canceled
            //    auto path = std::filesystem::path(file);
            //    Memory::loadROM(path);
            //    g_snes.reset();
            //}
        }

        if (ImGui::BeginMenu("Emulation")) {
            //bool cartInserted = Memory::cart.mapper != Mappers::NoCart;

            //if (ImGui::MenuItem("Trace", nullptr) && cartInserted) // Make sure not to run without cart
            //    g_snes.step();
            //if (ImGui::MenuItem("Run", nullptr, &running)) // Same here
            //    running = running ? cartInserted : false;
            //if (ImGui::MenuItem("Pause", nullptr) && running) {
            //    waitEmuThread(); // Wait till the frame finishes
            //    running = false; // Stop running
            //}

            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("Debug")) {
            ImGui::MenuItem("Show registers", nullptr, &showRegisterWindow);
            /*ImGui::MenuItem("Show SPC info", nullptr, &showSPCWindow);
            ImGui::MenuItem("Show cart info", nullptr, &showCartWindow);
            ImGui::MenuItem("Show DMA info", nullptr, &showDMAWindow);
            ImGui::MenuItem("Show PPU registers", nullptr, &showPPUWindow);
            ImGui::MenuItem("Show VRAM editor", nullptr, &showVramEditor);*/
            ImGui::MenuItem("Show bios memory", nullptr, &showBiosMemory);
            //ImGui::MenuItem("Show memory", nullptr, &showMemoryEditor);
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

void DebugUI::update()
{
}

void DebugUI::handleButtonPresses()
{
}

void DebugUI::handleEvents(sf::Event& ev)
{
}
