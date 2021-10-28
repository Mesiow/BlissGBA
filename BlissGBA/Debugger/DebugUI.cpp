#include "DebugUI.h"
#include "../Core/Emulator.h"


MemoryEditor DebugUI::biosMemory;
MemoryEditor DebugUI::vramEditor;
MemoryEditor DebugUI::ioEditor;
MemoryEditor DebugUI::obwramEditor;
MemoryEditor DebugUI::gamepakMemory;

DebugUI::DebugUI(sf::RenderWindow *window, Emulator *emu)
	:window(window), emu(emu), logger(emu->cpu)
{
    mbus = &emu->mbus;
    cpu = &emu->cpu;
    ppu = &emu->ppu;

    runToAddr = false;
    runToOpcode = false;
    showRegisterWindow = true;
    showBiosMemory = false;
    showVRAM = true;
    showIO = true;
    showOBWRAM = true;
    showGamePakMemory = false;
    showCartWindow = true;
    showPPUWindow = false;
    showPipeline = true;
    showDisplay = true;
    vsync = false;
    showLoggerSetup = false;

    //shift and h
    showKeys[0] = false; showKeys[1] = false;
    memset(addressBufferText, 0, sizeof(addressBufferText));
    memset(opcodeBufferText, 0, sizeof(opcodeBufferText));
}

void DebugUI::render()
{
    renderMenuBar();
    renderRegisters();
    renderCartInfo();
    renderPipeline();
    renderEmuButtons();
    renderLogSetup();
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
    if (showOBWRAM) {
        obwramEditor.DrawWindow("OBWRAM", mbus->getOBWRAM(), OB_WRAM_SIZE);
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
            bool state = cpu->getFlag(T);
            bool fiq = cpu->getFlag(F);
            bool irq = cpu->getFlag(I);

            ImGui::Checkbox("Zero ", &zero);
            ImGui::SameLine();
            ImGui::Checkbox("Sign ", &sign);
            ImGui::SameLine();
            ImGui::Checkbox("Carry", &carry);
            ImGui::SameLine();
            ImGui::Checkbox("Overflow", &overflow);
            ImGui::SameLine();
            ImGui::NewLine();
            ImGui::Checkbox("State(T)", &state);
            ImGui::SameLine();
            ImGui::Checkbox("FIQ", &fiq);
            ImGui::SameLine();
            ImGui::Checkbox("IRQ", &irq);
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

        ImGui::End();
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
                emu->reset();
            }
        }

        if (ImGui::BeginMenu("Emulation")) {
            //bool cartInserted = Memory::cart.mapper != Mappers::NoCart;
            if (ImGui::MenuItem("Show Debug Display (shift + h)", nullptr, showDebugger)) {
                onDebugUIToggle();
            }
            ImGui::MenuItem("Logging", nullptr, &showLoggerSetup);
              
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
            ImGui::MenuItem("Show IO registers", nullptr, &showIO);
            ImGui::MenuItem("Show VRAM", nullptr, &showVRAM);
            ImGui::MenuItem("Show OB WRAM", nullptr, &showOBWRAM);
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
        if (ImGui::Begin("Cartridge Info")) {
            ImGui::SetWindowFontScale(1.2);

            ImGui::Text("Game Title: %s", mbus->pak.title.c_str());
            ImGui::Text("ROM Size: %s", mbus->pak.romSize.c_str());

            ImGui::End();
        }
    }
}

void DebugUI::renderPipeline()
{
    if (showPipeline) {
      
       u32 first_ins = cpu->armpipeline[0];
       u32 second_ins = cpu->armpipeline[1];

       if (ImGui::Begin("Arm Pipeline")) {
           ImGui::SetWindowFontScale(1.2);

           ImGui::Text("0 -> 0x%08X", first_ins);
           ImGui::Text("1 -> 0x%08X", second_ins);

           ImGui::End();
       }
        
       {
           u16 first_ins = cpu->thumbpipeline[0];
           u16 second_ins = cpu->thumbpipeline[1];

           if (ImGui::Begin("Thumb Pipeline")) {
               ImGui::SetWindowFontScale(1.2);

               ImGui::Text("0 -> 0x%04X", first_ins);
               ImGui::Text("1 -> 0x%04X", second_ins);

               ImGui::End();
           }
       }
    }
}

void DebugUI::renderDisplay()
{
    if (showDisplay) {
        if (ImGui::Begin("Display")) {

            if (ppu->mode == BGMode::THREE)
                ImGui::Image(ppu->mode3.frame);
            else if (ppu->mode == BGMode::FOUR)
                ImGui::Image(ppu->mode4.frame);

            ImGui::End();
        }
    }
}

void DebugUI::renderEmuButtons()
{
    ImGui::Begin("Emulation");

    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(ImColor(255, 242, 0, 255)));
    ImGui::Text("Emulation");
    ImGui::PopStyleColor();

    if (ImGui::Button("Run")) {
        *running = true;
    }
    ImGui::SameLine();
    if (ImGui::Button("Step")) {
        cpu->clock();

        if (logger.isActive()) {
            logger.writeFile();
        }
    }
    ImGui::SameLine();
    if (ImGui::Button("Pause")) {
        *running = false;
    }
    ImGui::SameLine();
    if (ImGui::Button("Reset")) {
        emu->reset();
    }
    
    //ImGui::NewLine();

    //ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(ImColor(255, 242, 0, 255)));
    //ImGui::Text("Run until address");
    //ImGui::PopStyleColor();

    //ImGui::InputText("##Label", addressBufferText, sizeof(addressBufferText));
    //ImGui::SameLine();
    //if (ImGui::Button("Run until")) {
    //    std::string parse(addressBufferText);
    //    parse.erase(0, 2);

    //    //convert hex string to int
    //    std::stringstream ss;
    //    ss << std::hex << parse;
    //    ss >> addressToRunTo;

    //    printf("Running to address: 0x%08X\n", addressToRunTo);
    //    *running = true;
    //    runToAddr = true;
    //}

    ImGui::NewLine();

    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(ImColor(255, 242, 0, 255)));
    ImGui::Text("Run until opcode");
    ImGui::PopStyleColor();

    ImGui::InputText("##Label", opcodeBufferText, sizeof(opcodeBufferText));
    ImGui::SameLine();
    if (ImGui::Button("Run until")) {
        std::string parse(opcodeBufferText);

        if (cpu->getState() == State::ARM) {
            //convert hex string to int
            std::stringstream ss;
            ss << std::hex << parse;
            ss >> armOpcodeToRunTo;

            printf("Running to opcode: 0x%08X\n", armOpcodeToRunTo);
            *running = true;
            runToOpcode = true;
        }
        else {
            std::stringstream ss;
            ss << std::hex << parse;
            ss >> thumbOpcodeToRunTo;

            printf("Running to opcode: 0x%04X\n", thumbOpcodeToRunTo);
            *running = true;
            runToOpcode = true;
        }
    }

    ImGui::End();
}

void DebugUI::renderLogSetup()
{
    if (showLoggerSetup) {
        ImGui::Begin("Logger");

        ImGui::InputText("File Name", logFileName, sizeof(logFileName));
        if (ImGui::Button("Enable")) {
            logger.createAndOpenFile(std::string(logFileName));
        }
        ImGui::SameLine();
        if (ImGui::Button("Disable")) {
            logFileName[0] = '\0';
            logger.closeFile();
        }

        ImGui::End();
    }
}

void DebugUI::update()
{
    //Running emulation until specific address is hit
    if (runToAddr && *running) {
        if ((emu->cpu.R15 - 8) == addressToRunTo) {
            printf("Hit address!\n");
            *running = false;
            runToAddr = false;
        }
    }
    
    if (runToOpcode && *running) {

        if (cpu->getState() == State::ARM) {
            if (emu->cpu.currentExecutingArmOpcode == armOpcodeToRunTo) {
                printf("Hit arm opcode!\n");
                *running = false;
                runToOpcode = false;
            }
        }
        else {
            if (emu->cpu.currentExecutingThumbOpcode == thumbOpcodeToRunTo) {
                printf("Hit thumb opcode!\n");
                *running = false;
                runToOpcode = false;
            }
        }
    }

    if (logger.isActive() && *running) {
        logger.writeFile();
    }
}

void DebugUI::handleButtonPresses()
{
    
}

void DebugUI::handleEvents(sf::Event& ev)
{
    if (ev.type == sf::Event::KeyPressed) {
        if (ev.key.code == sf::Keyboard::LShift) showKeys[0] = true;
        if (ev.key.code == sf::Keyboard::H) showKeys[1] = true;

        bool shift, h;
        shift = showKeys[0];
        h = showKeys[1];

        if (shift && h) {
            *showDebugger = !*showDebugger;
            onDebugUIToggle();
        }
    }
    else if (ev.type == sf::Event::KeyReleased) {
        if (ev.key.code == sf::Keyboard::LShift) showKeys[0] = false;
        if (ev.key.code == sf::Keyboard::H) showKeys[1] = false;
    }
}

void DebugUI::onDebugUIToggle()
{
    if (*showDebugger == false)
        ppu->setScaleFactor(emu->displayScaleFactor);
    else
        ppu->setScaleFactor(emu->displayScaleFactor / 2.0f);
}
