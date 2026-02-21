#include "core/commands/Command.hpp"
#include "game/backend/Self.hpp"
#include "game/gta/Pools.hpp"

namespace YimMenu::Features
{
	class BringAllObjs : public Command
	{
		using Command::Command;

		virtual void OnCall() override
		{
			auto pos = Self::GetPed().GetPosition();
			for (auto obj : Pools::GetObjects())
			{
				if (obj)
				{
					obj.ForceControl();
					obj.SetPosition(pos);
				}
			}
		}
	};

	class BringAllPeds : public Command
	{
		using Command::Command;

		virtual void OnCall() override
		{
			auto pos = Self::GetPed().GetPosition();
			for (auto ped : Pools::GetPeds())
			{
				if (!ped.IsPlayer())
				{
					ped.ForceControl();
					ped.SetPosition(pos);
				}
			}
		}
	};

	class BringAllVehs : public Command
	{
		using Command::Command;

		virtual void OnCall() override
		{
			auto pos = Self::GetPed().GetPosition();
			for (auto veh : Pools::GetVehicles())
			{
				veh.ForceControl();
				veh.SetPosition(pos);
			}
		}
	};

	static BringAllObjs _BringAllObjs{"bringobjs", "召唤所有物体", "将所有游戏物体传送到你身边"};
static BringAllPeds _BringAllPeds{"bringpeds", "召唤所有NPC", "将所有游戏NPC传送到你身边"};
	static BringAllVehs _BringAllVehs{"bringvehs", "召唤所有载具", "将所有游戏载具传送到你身边"};
}