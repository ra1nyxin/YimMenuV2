#include "core/commands/LoopedCommand.hpp"
#include "core/hooking/DetourHook.hpp"
#include "game/hooks/Hooks.hpp"
#include "game/pointers/Pointers.hpp"

namespace YimMenu::Features
{
	class CheaterPool : public LoopedCommand
	{
		using LoopedCommand::LoopedCommand;

		int m_OldRegion;

		virtual void OnEnable() override
		{
			Pointers.SetJoinRequestPoolTypePatch->Apply();
			Pointers.HandleJoinRequestIgnorePoolPatch->Apply();
			m_OldRegion = *Pointers.RegionCode;
		}

		virtual void OnTick() override
		{
			*Pointers.RegionCode = 0; // put everyone in the same region for easier matchmaking
		}

		virtual void OnDisable() override
		{
			Pointers.SetJoinRequestPoolTypePatch->Restore();
			Pointers.HandleJoinRequestIgnorePoolPatch->Restore();
			*Pointers.RegionCode = m_OldRegion;
		}
	};

	static CheaterPool _CheaterPool{"cheaterpool", "作弊者池", "匹配会将你放入与其他YimMenu用户一起的会话"};
}

namespace YimMenu::Hooks
{
	int Network::GetPoolType()
	{
		if (YimMenu::Features::_CheaterPool.GetState())
			return 1;

		return BaseHook::Get<Network::GetPoolType, DetourHook<decltype(&Network::GetPoolType)>>()->Original()();
	}
}