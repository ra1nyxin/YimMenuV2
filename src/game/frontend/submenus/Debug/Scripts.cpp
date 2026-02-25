#include "Scripts.hpp"
#include "core/backend/FiberPool.hpp"
#include "core/backend/ScriptMgr.hpp"
#include "core/frontend/Notifications.hpp"
#include "game/gta/data/ScriptNames.hpp"
#include "game/gta/data/StackSizes.hpp"
#include "game/gta/Scripts.hpp"
#include "game/gta/Natives.hpp"
#include "game/pointers/Pointers.hpp"
#include "types/rage/atArray.hpp"
#include "types/script/GtaThread.hpp"
#include "types/script/scrProgram.hpp"
#include "types/script/CGameScriptHandlerNetComponent.hpp"
#include "types/network/CNetGamePlayer.hpp"
#include "misc/cpp/imgui_stdlib.h"

namespace YimMenu::Submenus
{
	static constexpr std::pair<const char*, int> stackSizes[] = {
	    {"MICRO", static_cast<int>(eStackSizes::MICRO)},
	    {"MINI", static_cast<int>(eStackSizes::MINI)},
	    {"DEFAULT", static_cast<int>(eStackSizes::DEFAULT)},
	    {"SPECIAL_ABILITY", static_cast<int>(eStackSizes::SPECIAL_ABILITY)},
	    {"FRIEND", static_cast<int>(eStackSizes::FRIEND)},
	    {"SHOP", static_cast<int>(eStackSizes::SHOP)},
	    {"CELLPHONE", static_cast<int>(eStackSizes::CELLPHONE)},
	    {"VEHICLE_SPAWN", static_cast<int>(eStackSizes::VEHICLE_SPAWN)},
	    {"CAR_MOD_SHOP", static_cast<int>(eStackSizes::CAR_MOD_SHOP)},
	    {"PAUSE_MENU_SCRIPT", static_cast<int>(eStackSizes::PAUSE_MENU_SCRIPT)},
	    {"APP_INTERNET", static_cast<int>(eStackSizes::APP_INTERNET)},
	    {"MULTIPLAYER_MISSION", static_cast<int>(eStackSizes::MULTIPLAYER_MISSION)},
	    {"CONTACTS_APP", static_cast<int>(eStackSizes::CONTACTS_APP)},
	    {"INTERACTION_MENU", static_cast<int>(eStackSizes::INTERACTION_MENU)},
	    {"SCRIPT_XML", static_cast<int>(eStackSizes::SCRIPT_XML)},
	    {"PROPERTY_INT", static_cast<int>(eStackSizes::PROPERTY_INT)},
	    {"ACTIVITY_CREATOR_INT", static_cast<int>(eStackSizes::ACTIVITY_CREATOR_INT)},
	    {"SMPL_INTERIOR", static_cast<int>(eStackSizes::SMPL_INTERIOR)},
	    {"WAREHOUSE", static_cast<int>(eStackSizes::WAREHOUSE)},
	    {"IE_DELIVERY", static_cast<int>(eStackSizes::IE_DELIVERY)},
	    {"SHOP_CONTROLLER", static_cast<int>(eStackSizes::SHOP_CONTROLLER)},
	    {"AM_MP_YACHT", static_cast<int>(eStackSizes::AM_MP_YACHT)},
	    {"INGAMEHUD", static_cast<int>(eStackSizes::INGAMEHUD)},
	    {"TRANSITION", static_cast<int>(eStackSizes::TRANSITION)},
	    {"FMMC_LAUNCHER", static_cast<int>(eStackSizes::FMMC_LAUNCHER)},
	    {"MULTIPLAYER_FREEMODE", static_cast<int>(eStackSizes::MULTIPLAYER_FREEMODE)},
	    {"MISSION", static_cast<int>(eStackSizes::MISSION)},
	    {"MP_LAUNCH_SCRIPT", static_cast<int>(eStackSizes::MP_LAUNCH_SCRIPT)}};

	static void RenderBytecode(rage::scrProgram* program)
	{
		constexpr int bytesPerRow = 16;
		const std::uint32_t codeSize = program->GetFullCodeSize();
		const std::uint32_t totalRows = (codeSize + bytesPerRow - 1) / bytesPerRow;

		static bool shouldJump = false;
		static float targetScroll = -1.0f;
		static char offsetInput[9] = "";

		ImGui::SetNextItemWidth(150);
		ImGui::InputText("##jumpoffset", offsetInput, IM_ARRAYSIZE(offsetInput));
		ImGui::SameLine();
		if (ImGui::Button("跳转到偏移"))
		{
			char* end = nullptr;
			std::uint32_t offset = strtoul(offsetInput, &end, 0);
			if (end != offsetInput && offset < codeSize)
			{
				std::uint32_t row = offset / bytesPerRow;
				targetScroll = row * ImGui::GetFrameHeightWithSpacing();
				shouldJump = true;
			}
		}

		ImGui::BeginChild("##bytecode", ImVec2(610, 400), ImGuiChildFlags_Borders, ImGuiWindowFlags_HorizontalScrollbar);

		if (shouldJump && targetScroll >= 0.0f)
		{
			ImGui::SetScrollY(targetScroll);
			shouldJump = false;
		}

		ImGuiListClipper clipper;
		clipper.Begin(totalRows);
		while (clipper.Step())
		{
			for (int row = clipper.DisplayStart; row < clipper.DisplayEnd; ++row)
			{
				std::uint32_t offset = row * bytesPerRow;

				ImGui::Text("%08X: ", offset);
				ImGui::SameLine(80);

				for (int i = 0; i < bytesPerRow; ++i)
				{
					std::uint32_t index = offset + i;
					if (index >= codeSize)
						break;

					if (auto byte = program->GetCodeAddress(index))
					{
						char hexStr[3];
						snprintf(hexStr, sizeof(hexStr), "%02X", *byte);

						ImGui::SetNextItemWidth(24);
						ImGui::PushID(index);
						if (ImGui::InputText("##byte", hexStr, sizeof(hexStr), ImGuiInputTextFlags_CharsHexadecimal | ImGuiInputTextFlags_EnterReturnsTrue))
						{
							char* end = nullptr;
							std::uint8_t newVal = static_cast<std::uint8_t>(strtoul(hexStr, &end, 16));
							if (end != hexStr)
								*byte = newVal; // this will conflict with the ScriptPatches class, but still useful to keep for quick testings
						}
						ImGui::PopID();
						if (ImGui::IsItemActive() && ImGui::IsItemHovered())
							ImGui::SetTooltip("按回车键写入。");

						if (i < bytesPerRow - 1)
							ImGui::SameLine();
					}
				}
			}
		}

		clipper.End();
		ImGui::EndChild();
	}

	std::shared_ptr<Category> BuildScriptsMenu()
	{
		auto menu = std::make_unique<Category>("脚本");

		auto tabBar = std::make_unique<TabBarItem>("脚本");
		auto threads = std::make_unique<TabItem>("线程");
		auto script = std::make_unique<TabItem>("启动脚本");

		threads->AddItem(std::make_unique<ImGuiItem>([] {
			static rage::scrThread* curThread = nullptr;
			static rage::scrProgram* curProgram = nullptr;

			if (!Pointers.ScriptThreads || Pointers.ScriptThreads->size() == 0)
			{
				curThread = nullptr;
				curProgram = nullptr;
				return ImGui::TextDisabled("无");
			}

			if (ImGui::BeginCombo("线程", curThread ? curThread->m_ScriptName : "(选择)"))
			{
				for (auto thread : *Pointers.ScriptThreads)
				{
					if (thread)
					{
						if (thread->m_Context.m_State == rage::scrThread::State::KILLED || thread->m_Context.m_StackSize == 0)
							continue;

						ImGui::PushID(thread->m_Context.m_ThreadId);

						if (ImGui::Selectable(thread->m_ScriptName, curThread == thread))
						{
							curThread = thread;
							curProgram = Scripts::FindScriptProgram(thread->m_ScriptHash);
						}

						if (curThread == thread)
							ImGui::SetItemDefaultFocus();

						ImGui::PopID();
					}
				}

				ImGui::EndCombo();
			}

			if (!curThread || !curProgram || !curProgram->IsValid() || curProgram->m_RefCount == 0)
			{
				curThread = nullptr;
				curProgram = nullptr;
				return;
			}

			ImGui::Combo("状态", (int*)&curThread->m_Context.m_State, "Idle\0Running\0Killed\0Paused\0Unk4\0");

			if (curThread->m_Context.m_State == rage::scrThread::State::KILLED)
			{
				ImGui::Text("退出原因: %s", curThread->m_ErrorMessage);
			}
			else
			{
				if (ImGui::Button("终止"))
				{
					curThread->Kill();
					curThread->m_Context.m_State = rage::scrThread::State::KILLED;
				}
				ImGui::SameLine();
				if (ImGui::Button("记录文本标签"))
				{
					FiberPool::Push([] {
						for (int i = 0; i < curProgram->m_StringsCount; i++)
						{
							if (auto str = curProgram->GetString(i))
							{
								if (HUD::DOES_TEXT_LABEL_EXIST(str))
								{
									LOGF(INFO, "{} - {} (0x{:X}): {}", i, str, Joaat(str), HUD::GET_FILENAME_FOR_AUDIO_CONVERSATION(str));
								}
							}
						}
					});
				}

				if (ImGui::TreeNode("Info"))
				{
					if (auto netComponent = reinterpret_cast<GtaThread*>(curThread)->m_NetComponent)
					{
						if (auto host = netComponent->GetHost())
						{
							ImGui::Text("主机: %s", host->GetName());
						}
						ImGui::SameLine();
						ImGui::BeginDisabled(netComponent->IsLocalPlayerHost());
						if (ImGui::SmallButton("获取主机权"))
						{
							FiberPool::Push([] {
								Scripts::ForceScriptHost(curThread);
							});
						}
						ImGui::EndDisabled();
					}
					ImGui::BeginGroup();
					ImGui::Text("线程 ID: %d", curThread->m_Context.m_ThreadId);
					ImGui::Text("栈大小: %d", curThread->m_Context.m_StackSize);
					ImGui::Text("栈指针: 0x%X", curThread->m_Context.m_StackPointer);
					ImGui::Text("程序计数器: 0x%X", curThread->m_Context.m_ProgramCounter); // This is not really accurate (always points to the WAIT)
					ImGui::Text("代码大小: %d", curProgram->m_CodeSize);
					ImGui::EndGroup();
					ImGui::SameLine();
					ImGui::BeginGroup();
					ImGui::Text("参数数量: %d", curProgram->m_ArgCount);
					ImGui::Text("局部变量数量: %d", curProgram->m_LocalCount);
					ImGui::Text("全局变量数量: %d", curProgram->m_GlobalCount);
					ImGui::Text("原生函数数量: %d", curProgram->m_NativeCount);
					ImGui::Text("字符串数量: %d", curProgram->m_StringsCount);
					ImGui::EndGroup();
					ImGui::TreePop();
				}
				if (ImGui::TreeNode("Bytecode"))
				{
					RenderBytecode(curProgram);
					ImGui::TreePop();
				}
			}
		}));

		script->AddItem(std::make_unique<ImGuiItem>([] {
			static std::string scriptSearch = "";
			static std::string stackSizeName = stackSizes[0].first;
			static std::optional<int> launcherIndex = std::nullopt;
			static int stackSize = 0;
			static std::int64_t* args = nullptr;
			static int argCount = 0;
			static int previousArgCount = 0;
			static bool pauseAfterStarting = false;

			bool modified = ImGui::InputTextWithHint("脚本名称", "搜索", &scriptSearch);

			if (ImGui::BeginCombo("栈大小", stackSizeName.c_str()))
			{
				for (auto& p : stackSizes)
				{
					if (ImGui::Selectable(std::format("{} ({})", p.first, (int)p.second).data(), stackSize == p.second))
					{
						stackSizeName = p.first;
						stackSize = p.second;
					}

					if (p.second == stackSize)
						ImGui::SetItemDefaultFocus();
				}

				ImGui::EndCombo();
			}

			if (ImGui::InputInt("参数数量", &argCount))
			{
				if (argCount < 0) // should clamp this to a max value?
					argCount = 0;
			}
			if (argCount != previousArgCount)
			{
				std::int64_t* temp = nullptr;
				if (argCount > 0)
				{
					temp = new std::int64_t[argCount]{0};
					if (args)
						std::memcpy(temp, args, sizeof(int64_t) * std::min(argCount, previousArgCount));
				}
				delete[] args;
				args = temp;
				previousArgCount = argCount;
			}
			for (int i = 0; i < argCount; i++)
			{
				ImGui::Text("Arg[%d]", i);
				ImGui::SameLine();
				ImGui::PushID(i);
				ImGui::InputScalar("##arg", ImGuiDataType_S64, &args[i]);
				ImGui::PopID();
			}

			if (ImGui::BeginListBox("##scripts"))
			{
				std::string lowerSearch = scriptSearch;
				std::transform(lowerSearch.begin(), lowerSearch.end(), lowerSearch.begin(), ::tolower);
				for (auto& script : scriptNames)
				{
					std::string lowerScript = script;
					std::transform(lowerScript.begin(), lowerScript.end(), lowerScript.begin(), ::tolower);
					if (lowerScript.find(lowerSearch) != std::string::npos)
					{
						if (ImGui::Selectable(script, scriptSearch == script))
						{
							scriptSearch = script;
							modified = true;
						}
					}
				}

				ImGui::EndListBox();
			}

			if (modified)
			{
				launcherIndex = Scripts::GetLauncherIndexFromScript(Joaat(scriptSearch));
			}

			ImGui::Checkbox("启动后暂停", &pauseAfterStarting);

			if (ImGui::Button("启动脚本"))
			{
				FiberPool::Push([] {
					auto hash = Joaat(scriptSearch);

					if (!SCRIPT::DOES_SCRIPT_WITH_NAME_HASH_EXIST(hash))
					{
						Notifications::Show("启动脚本", "脚本不存在。", NotificationType::Error);
						return;
					}

					if (SCRIPT::GET_NUMBER_OF_THREADS_RUNNING_THE_SCRIPT_WITH_THIS_HASH(hash) > 0)
					{
						Notifications::Show("启动脚本", "脚本已在运行。", NotificationType::Error);
						return;
					}

					if (MISC::GET_NUMBER_OF_FREE_STACKS_OF_THIS_SIZE(stackSize) == 0)
					{
						Notifications::Show("启动脚本", "没有可用的该大小脚本栈。", NotificationType::Error);
						return;
					}

					while (!SCRIPT::HAS_SCRIPT_WITH_NAME_HASH_LOADED(hash))
					{
						SCRIPT::REQUEST_SCRIPT_WITH_NAME_HASH(hash);
						ScriptMgr::Yield();
					}

					int id = 0;
					if (args && argCount > 0)
					{
						id = BUILTIN::START_NEW_SCRIPT_WITH_NAME_HASH_AND_ARGS(hash, args, argCount, stackSize);
					}
					else
					{
						id = BUILTIN::START_NEW_SCRIPT_WITH_NAME_HASH(hash, stackSize);
					}

					if (pauseAfterStarting)
					{
						if (auto thread = Scripts::FindScriptThreadByID(id))
							thread->m_Context.m_State = rage::scrThread::State::PAUSED;
					}

					SCRIPT::SET_SCRIPT_WITH_NAME_HASH_AS_NO_LONGER_NEEDED(hash);
					Notifications::Show("启动脚本", std::format("已启动脚本，ID 为 {}。", id), NotificationType::Success);
				});
			}

			if (launcherIndex && *Pointers.IsSessionStarted)
			{
				ImGui::SameLine();
				if (ImGui::Button("启动战局脚本"))
				{
					FiberPool::Push([] {
						Scripts::StartLauncherScript(Joaat(scriptSearch));
					});
				}
				ImGui::SameLine();
				if (ImGui::Button("以事件方式启动脚本"))
				{
					FiberPool::Push([] {
						Scripts::ForceScriptOnPlayer(Joaat(scriptSearch), -1);
					});
				}
			}
		}));

		tabBar->AddItem(std::move(threads));
		tabBar->AddItem(std::move(script));

		menu->AddItem(std::move(tabBar));

		return menu;
	}
}