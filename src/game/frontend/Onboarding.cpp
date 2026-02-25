#include "Onboarding.hpp"
#include "GUI.hpp"
#include "core/commands/Commands.hpp"
#include "core/commands/BoolCommand.hpp"
#include "game/backend/AnticheatBypass.hpp"
#include "game/pointers/Pointers.hpp"
#include <shellapi.h>

namespace YimMenu
{
	static BoolCommand _OnboardingComplete{"$onboardingcomplete", "", ""};

	void ProcessOnboarding()
	{
		if (_OnboardingComplete.GetState())
			return;

		static bool ensure_popup_open = [] {
			ImGui::OpenPopup("重要提示！请仔细阅读！");
			GUI::SetOnboarding(true);
			return true;
		}();

		const auto window_size = ImVec2{700, 500};
		const auto window_position = ImVec2{(*Pointers.ScreenResX - window_size.x) / 2, (*Pointers.ScreenResY - window_size.y) / 2};

		ImGui::SetNextWindowSize(window_size, ImGuiCond_Once);
		ImGui::SetNextWindowPos(window_position, ImGuiCond_Once);

		if (ImGui::BeginPopupModal("重要提示！请仔细阅读！", nullptr, ImGuiWindowFlags_NoSavedSettings))
		{
			ImGui::TextWrapped("%s",
			    "欢迎使用 YimMenuV2！你可以按 INSERT 或 Ctrl+\\ 打开菜单。随着 BattlEye 的引入，加入并停留在公共战局的能力被大幅限制。"
			    "你可以选择只与其他 YimMenu 用户一起游玩，或者选择连接到常规的 BattlEye 保护战局。"
			    "在常规战局中，你会在三分钟内被自动踢出，即使重新启用 BattlEye，在长达两天内也可能被暂时加入黑名单，无法再次加入。");
			static int value = 0;
			ImGui::RadioButton("仅与 YimMenu 用户游玩", &value, 0);
			ImGui::SameLine();
			ImGui::RadioButton("与所有人游玩（当前不稳定）", &value, 1);
			ImGui::TextWrapped("%s",
			    "你可以随时通过切换 网络 > 伪装 > 仅加入 YimMenu 战局 来更改此选择。我们的官方仓库在 "
			    "https://github.com/YimMenu/YimMenuV2，请务必只从 GitHub 下载菜单以避免恶意软件。"
			    "你可以通过该仓库报告 Bug、提交功能建议，并通过 Pull Request 参与贡献。我们还提供一个 Matrix 服务器 "
			    "https://matrix.to/#/#yimmenu:matrix.org，方便你与开发者及其他用户快速交流。"
			    "Matrix 是一个开源、免费的 Discord 替代品，注册账号安全且简单。");
			if (ImGui::Button("打开 GitHub"))
			{
				ShellExecuteA(NULL, "open", "https://github.com/YimMenu/YimMenuV2", NULL, NULL, SW_SHOWNORMAL);
			}
			ImGui::SameLine();
			if (ImGui::Button("打开 Matrix 服务器"))
			{
				ShellExecuteA(NULL, "open", "https://matrix.to/#/#yimmenu:matrix.org", NULL, NULL, SW_SHOWNORMAL);
			}
			ImGui::TextWrapped("%s",
			    "请经常检查更新；我们几乎每晚都会发布新构建。最重要的是，尽情玩耍，享受使用 YimMenu 的乐趣！");
			if (ImGui::Button("关闭"))
			{
				Commands::GetCommand<BoolCommand>("cheaterpool"_J)->SetState(!value);
				_OnboardingComplete.SetState(true);
				GUI::SetOnboarding(false);
				ImGui::CloseCurrentPopup();
			}
			ImGui::EndPopup();
		}
	}
}