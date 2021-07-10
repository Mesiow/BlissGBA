#include "DebugUI.h"

DebugUI::DebugUI(Arm* cpu)
	:cpu(cpu)
{
}

void DebugUI::render()
{
	ImGui::Begin("Registers");
	ImGui::NewLine();
	
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

	ImGui::End();
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
