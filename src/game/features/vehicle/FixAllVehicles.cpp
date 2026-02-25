#include "core/commands/Command.hpp"
#include "core/frontend/Notifications.hpp"
#include "game/backend/PersonalVehicles.hpp"
#include "game/pointers/Pointers.hpp"

namespace YimMenu::Features
{
	class FixAllVehicles : public Command
	{
		using Command::Command;

		virtual void OnCall() override
		{
			if (!*Pointers.IsSessionStarted)
				return;

			int count = 0;
			for (const auto& it : PersonalVehicles::GetPersonalVehicles())
			{
				const auto& personalVeh = it.second;
				if (personalVeh->Repair())
					count++;
			}

			if (count > 0)
				Notifications::Show("修复所有载具", std::format("已修复 {} 辆载具。", count), NotificationType::Success);
			else
				Notifications::Show("修复所有载具", "没有需要修复的载具。");
		}
	};

	static FixAllVehicles _FixAllVehicles{"fixallvehicles", "修复所有载具", "修复你所有被摧毁的个人载具。"};
}