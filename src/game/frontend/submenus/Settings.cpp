#include "Settings.hpp"

#include "core/commands/Commands.hpp"
#include "core/commands/HotkeySystem.hpp"
#include "core/commands/LoopedCommand.hpp"
#include "game/backend/Self.hpp"
#include "game/frontend/items/Items.hpp"
#include "game/frontend/items/DrawHotkey.hpp"
#include "game/frontend/submenus/Settings/LuaScripts.hpp"
#include "game/frontend/submenus/Settings/GUISettings.hpp"

namespace YimMenu::Submenus
{
	// TODO: refactor this
	static void Hotkeys()
	{
		ImGui::BulletText("按住命令名称按钮并输入按键来更改热键");
		ImGui::BulletText("如果命令已有热键，点击按钮将移除它");

		ImGui::Spacing();
		ImGui::Separator();
		ImGui::Spacing();
		
		// this assumes we can't add new commands in runtime, but a lot of other subsystems assume that too
		static std::map<std::string, CommandLink*> sortedCommands;
		static bool commandsSorted = []() {
			for (auto& [hash, command] : Commands::GetCommands())
			{
				if (auto it = g_HotkeySystem.m_CommandHotkeys.find(hash); it != g_HotkeySystem.m_CommandHotkeys.end())
					sortedCommands.emplace(command->GetLabel(), &it->second);
			}
			return true;
		}();

		HotkeySystem::SetBeingModifed(false);

		for (auto& [name, link] : sortedCommands)
		{
			if (name.empty())
				continue;
			DrawHotkey(link, name);
		}
	};

	Settings::Settings() :
	#define ICON_FA_GEARS "\xef\x80\x93"
	    Submenu::Submenu("设置", ICON_FA_GEARS)
	{
		auto hotkeys = std::make_shared<Category>("热键");
		auto gui = std::make_shared<Category>("界面");
		auto game = std::make_shared<Category>("游戏");

		auto uiStyle = std::make_shared<Group>("界面样式");
		auto playerEsp = std::make_shared<Group>("玩家透视", 10);
		auto pedEsp = std::make_shared<Group>("NPC透视", 10);
		auto objectEsp = std::make_shared<Group>("物体透视");
		auto overlay = std::make_shared<Group>("覆盖层");
		auto chat = std::make_shared<Group>("聊天");

		hotkeys->AddItem(std::make_shared<ImGuiItem>(Hotkeys));

		// Players
		uiStyle->AddItem(std::make_shared<ListCommandItem>("styleselector"_J));

		playerEsp->AddItem(std::make_shared<BoolCommandItem>("espdrawplayers"_J));
		playerEsp->AddItem(std::make_shared<ConditionalItem>("espdrawplayers"_J, std::make_shared<BoolCommandItem>("espdrawdeadplayers"_J)));

		playerEsp->AddItem(std::make_shared<ConditionalItem>("espdrawplayers"_J, std::make_shared<BoolCommandItem>("espnameplayers"_J, "玩家名称")));
		playerEsp->AddItem(std::make_shared<ConditionalItem>("espdrawplayers"_J, std::make_shared<ColorCommandItem>("namecolorplayers"_J)));

		playerEsp->AddItem(std::make_shared<ConditionalItem>("espdrawplayers"_J, std::make_shared<BoolCommandItem>("espdistanceplayers"_J, "玩家距离")));

		playerEsp->AddItem(std::make_shared<ConditionalItem>("espdrawplayers"_J, std::make_shared<BoolCommandItem>("espskeletonplayers"_J, "玩家骨架")));
		playerEsp->AddItem(std::make_shared<ConditionalItem>("espdrawplayers"_J, std::make_shared<ColorCommandItem>("skeletoncolorplayers"_J)));

		// Peds
		pedEsp->AddItem(std::make_shared<BoolCommandItem>("espdrawpeds"_J));
		pedEsp->AddItem(std::make_shared<ConditionalItem>("espdrawpeds"_J, std::make_shared<BoolCommandItem>("espdrawdeadpeds"_J)));

		pedEsp->AddItem(std::make_shared<ConditionalItem>("espdrawpeds"_J, std::make_shared<BoolCommandItem>("espmodelspeds"_J, "NPC哈希")));
		pedEsp->AddItem(std::make_shared<ConditionalItem>("espdrawpeds"_J, std::make_shared<ColorCommandItem>("hashcolorpeds"_J)));

		pedEsp->AddItem(std::make_shared<ConditionalItem>("espdrawpeds"_J, std::make_shared<BoolCommandItem>("espnetinfopeds"_J, "NPC网络信息")));
		pedEsp->AddItem(std::make_shared<ConditionalItem>("espdrawpeds"_J, std::make_shared<BoolCommandItem>("espscriptinfopeds"_J, "NPC脚本信息")));

		pedEsp->AddItem(std::make_shared<ConditionalItem>("espdrawpeds"_J, std::make_shared<BoolCommandItem>("espdistancepeds"_J, "NPC距离")));

		pedEsp->AddItem(std::make_shared<ConditionalItem>("espdrawpeds"_J, std::make_shared<BoolCommandItem>("espskeletonpeds"_J, "NPC骨架")));
		pedEsp->AddItem(std::make_shared<ConditionalItem>("espdrawpeds"_J, std::make_shared<ColorCommandItem>("skeletoncolorpeds"_J)));

		objectEsp->AddItem(std::make_shared<BoolCommandItem>("espdrawobjects"_J));
		objectEsp->AddItem(std::make_shared<ConditionalItem>("espdrawobjects"_J, std::make_shared<ColorCommandItem>("hashcolorobjects"_J)));
		objectEsp->AddItem(std::make_shared<ConditionalItem>("espdrawobjects"_J, std::make_shared<BoolCommandItem>("espnetinfoobjects"_J, "物体网络信息")));
		objectEsp->AddItem(std::make_shared<ConditionalItem>("espdrawobjects"_J, std::make_shared<BoolCommandItem>("espscriptinfoobjects"_J, "物体脚本信息")));

		objectEsp->AddItem(std::make_shared<ConditionalItem>("espdrawobjects"_J, std::make_shared<BoolCommandItem>("espdistanceobjects"_J, "物体距离")));


		overlay->AddItem(std::make_shared<BoolCommandItem>("overlay"_J));
		overlay->AddItem(std::make_shared<ConditionalItem>("overlay"_J, std::make_shared<BoolCommandItem>("overlayfps"_J)));

		chat->AddItem(std::make_shared<BoolCommandItem>("clearchat"_J));

		game->AddItem(playerEsp);
		game->AddItem(pedEsp);
		game->AddItem(objectEsp);

		gui->AddItem(uiStyle);
		gui->AddItem(overlay);
		gui->AddItem(chat);

		AddCategory(std::move(hotkeys));
		AddCategory(std::move(gui));
		AddCategory(std::move(game));
		AddCategory(DrawGUISettingsMenu());
		AddCategory(BuildLuaScriptsMenu());
	}
}