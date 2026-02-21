#include "core/commands/Command.hpp"
#include "game/gta/Pools.hpp"

namespace YimMenu::Features
{
	class KillAll : public Command
	{
		using Command::Command;

		virtual void OnCall() override
		{
			for (auto ped : Pools::GetPeds())
			{
				if (!ped.IsPlayer())
					ped.Kill();
			}
		}
	};

	class KillAllEnemies : public Command
	{
		using Command::Command;

		virtual void OnCall() override
		{
			for (auto ped : Pools::GetPeds())
			{
				if (!ped.IsPlayer() && ped.IsEnemy())
					ped.Kill();
			}
		}
	};

	static KillAll _KillAll{"killallpeds", "击杀所有NPC", "击杀游戏世界中的所有NPC"};
	static KillAllEnemies _KillAllEnemies{"killallenemies", "击杀所有敌人", "击杀游戏世界中的所有敌人"};
}