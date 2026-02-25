#include "core/commands/StringCommand.hpp"
#include "core/frontend/Notifications.hpp"
#include "game/backend/Self.hpp"
#include "game/gta/Object.hpp"
#include "game/gta/Natives.hpp"

namespace YimMenu::Features
{
	static StringCommand _PedModelName{"pedmodelname", "NPC模型", "你想要生成的NPC的模型名称。"};
	static StringCommand _ObjectModelName{"objectmodelname", "物体模型", "你想要生成的物体的模型名称。"};


	class SpawnPed : public Command
	{
		using Command::Command;

		virtual void OnCall() override
		{
			auto model = _PedModelName.GetString();
			if (!model.length())
			{
				Notifications::Show("生成 NPC", "未提供模型名称。", NotificationType::Error);
				return;
			}

			auto modelHash = Joaat(model);
			if (STREAMING::IS_MODEL_IN_CDIMAGE(modelHash))
			{
				Ped::Create(modelHash, Self::GetPed().GetPosition(), Self::GetPed().GetHeading());
			}
			else
			{
				Notifications::Show("生成 NPC", "提供的模型名称无效。", NotificationType::Error);
			}
		}
	};

	class SpawnObject : public Command
	{
		using Command::Command;

		virtual void OnCall() override
		{
			auto model = _ObjectModelName.GetString();
			if (!model.length())
			{
				Notifications::Show("生成物体", "未提供模型名称。", NotificationType::Error);
				return;
			}

			auto modelHash = Joaat(model);
			if (STREAMING::IS_MODEL_IN_CDIMAGE(modelHash))
			{
				Object::Create(modelHash, Self::GetPed().GetPosition());
			}
			else
			{
				Notifications::Show("生成物体", "提供的模型名称无效。", NotificationType::Error);
			}
		}
	};

static SpawnPed _SpawnPed{"spawnped", "生成NPC", "在你当前位置生成一个NPC。"};
	static SpawnObject _SpawnObject{"spawnobject", "生成物体", "在你当前位置生成一个物体。"};
}