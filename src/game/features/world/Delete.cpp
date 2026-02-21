#include "core/commands/Command.hpp"
#include "game/gta/Pools.hpp"
#include "game/gta/Object.hpp"

namespace YimMenu::Features
{
	class DeleteAllObjects : public Command
	{
		using Command::Command;

		virtual void OnCall() override
		{
			for (auto obj : Pools::GetObjects())
			{
				if (obj)
					obj.Delete();
			}
		}
	};

	class DeleteAllCameras : public Command
	{
		using Command::Command;

		virtual void OnCall() override
		{
			for (auto obj : Pools::GetObjects())
			{
				if (obj && obj.As<Object>().IsCamera())
					obj.Delete();
			}
		}
	};

	class DeleteAllPeds : public Command
	{
		using Command::Command;

		virtual void OnCall() override
		{
			for (auto ped : Pools::GetPeds())
			{
				if (!ped.IsPlayer())
					ped.Delete();
			}
		}
	};

	class DeleteAllVehs : public Command
	{
		using Command::Command;

		virtual void OnCall() override
		{
			for (auto veh : Pools::GetVehicles())
			{
				veh.Delete();
			}
		}
	};

	static DeleteAllCameras _DeleteAllCameras("delcams", "删除所有摄像头", "删除所有摄像头");
static DeleteAllObjects _DeleteAllObjects{"delobjs", "删除所有物体", "删除游戏世界中的所有物体，包括任务关键物体"};
	static DeleteAllPeds _DeleteAllPeds{"delpeds", "删除所有NPC", "删除游戏世界中的所有NPC，包括任务关键NPC"};
	static DeleteAllVehs _DeleteAllVehs{"delvehs", "删除所有载具", "删除游戏世界中的所有载具，包括任务关键载具"};
}