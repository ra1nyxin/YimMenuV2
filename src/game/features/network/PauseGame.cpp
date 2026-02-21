#include "core/commands/BoolCommand.hpp"
#include "game/pointers/Pointers.hpp"

namespace YimMenu::Features
{
	class PauseGame : public BoolCommand
	{
		using BoolCommand::BoolCommand;

		virtual void OnEnable() override
		{
			Pointers.AllowPausingInSessionPatch->Apply();
		}

		virtual void OnDisable() override
		{
			Pointers.AllowPausingInSessionPatch->Restore();
		}
	};

	static PauseGame _PauseGame{"pausegame", "暂停游戏", "允许你在线上模式中暂停游戏（仅限本地）"};
}