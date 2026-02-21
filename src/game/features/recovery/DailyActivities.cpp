#include "DailyActivities.hpp"
#include "core/commands/ListCommand.hpp"
#include "core/commands/BoolCommand.hpp"
#include "core/backend/ScriptMgr.hpp"
#include "core/frontend/Notifications.hpp"
#include "game/backend/Self.hpp"
#include "game/backend/ScriptPatches.hpp"
#include "game/backend/Tunables.hpp"
#include "game/gta/Ped.hpp"
#include "game/gta/Natives.hpp"
#include "game/gta/Scripts.hpp"
#include "game/gta/Stats.hpp"
#include "game/gta/Vehicle.hpp"
#include "game/gta/ScriptFunction.hpp"
#include "game/gta/ScriptLocal.hpp"
#include "game/gta/ScriptGlobal.hpp"
#include "game/pointers/Pointers.hpp"
#include "types/blip/BlipSprite.hpp"
#include "types/script/ScriptEvent.hpp"
#include "types/script/MPScriptData.hpp"
#include "types/script/globals/GPBD_Flow.hpp"
#include "types/script/globals/g_SavedMPGlobals.hpp"
#include "types/script/globals/DailyObjectivesState.hpp"
#include "types/script/globals/WeeklyChallenge.hpp"
#include "types/script/globals/GSBD_RandomEvents.hpp"
#include "types/script/globals/FreemodeGeneral.hpp"
//#include "types/script/globals/FreemodeDelivery.hpp"
#include "types/script/locals/StandardTimeTrialData.hpp"
#include "types/script/locals/RCBanditoTimeTrialData.hpp"
#include "types/script/locals/BikeTimeTrialData.hpp"

namespace YimMenu::Features
{
	static ListCommand hiddenCacheIndex = {"hiddencacheindex", "Hidden Cache", "Selected Hidden Cache", {{0, "Hidden Cache 1"}, {1, "Hidden Cache 2"}, {2, "Hidden Cache 3"}, {3, "Hidden Cache 4"}, {4, "Hidden Cache 5"}, {5, "Hidden Cache 6"}, {6, "Hidden Cache 7"}, {7, "Hidden Cache 8"}, {8, "Hidden Cache 9"}, {9, "Hidden Cache 10"}}};

	static ListCommand treasureChestIndex = {"treasurechestindex", "Treasure Chest", "Selected Treasure Chest", {{0, "Treasure Chest 1"}, {1, "Treasure Chest 2"}}};

	static ListCommand buriedStashIndex = {"buriedstashindex", "Buried Stash", "Selected Buried Stash", {{0, "Buried Stash 1"}, {1, "Buried Stash 2"}}};

	static ListCommand skydiveIndex = {"skydiveindex", "Skydive", "Selected Skydive", {{0, "Skydive 1"}, {1, "Skydive 2"}, {2, "Skydive 3"}, {3, "Skydive 4"}, {4, "Skydive 5"}, {5, "Skydive 6"}, {6, "Skydive 7"}, {7, "Skydive 8"}, {8, "Skydive 9"}, {9, "Skydive 10"}}};

	static ListCommand timeTrialIndex = {"timetrialindex", "Time Trial", "Selected Time Trial", {{0, "Standard Time Trial"}, {1, "RC Bandito Time Trial"}, {2, "Junk Energy Bike Time Trial"}}};

	static ListCommand streetDealerIndex = {"streetdealerindex", "Street Dealer", "Selected Street Dealer", {{0, "Street Dealer 1"}, {1, "Street Dealer 2"}, {2, "Street Dealer 3"}}};

	static ListCommand lsTagIndex = {"lstagindex", "LS Tag", "Selected LS Tag", {{0, "LS Tag 1"}, {1, "LS Tag 2"}, {2, "LS Tag 3"}, {3, "LS Tag 4"}, {4, "LS Tag 5"}}};

	static ListCommand animalIndex = {"animalindex", "Animal", "Selected Animal", {{0, "Animal 1"}, {1, "Animal 2"}, {2, "Animal 3"}}};

	static ListCommand productIndex = {"productindex", "Product", "Selected Product", {{0, "Product 1"}, {1, "Product 2"}, {2, "Product 3"}, {3, "Product 4"}, {4, "Product 5"}, {5, "Product 6"}, {6, "Product 7"}, {7, "Product 8"}, {8, "Product 9"}, {9, "Product 10"}}};

	static constexpr auto wildlifePhotographyAnimalHashes = std::to_array({"A_C_Boar"_J, "A_C_Cat_01"_J, "A_C_Cow"_J, "A_C_Coyote"_J, "A_C_Deer"_J, "A_C_Husky"_J, "A_C_MtLion"_J, "A_C_Pig"_J, "A_C_Poodle"_J, "A_C_Pug"_J, "A_C_Rabbit_01"_J, "A_C_Retriever"_J, "A_C_Rottweiler"_J, "A_C_shepherd"_J, "A_C_Westy"_J, "A_C_Chickenhawk"_J, "A_C_Cormorant"_J, "A_C_Crow"_J, "A_C_Hen"_J, "A_C_Seagull"_J});

	static void SetAllDailyActivitiesCompleted(bool completed)
	{
		Stats::SetPackedBool(36628, completed); // G's Cache
		Stats::SetPackedBool(36657, completed); // Stash House
		Stats::SetPackedBool(31734, completed); // Shipwreck
		Stats::SetPackedBool(30297, completed); // Hidden Cache 1
		Stats::SetPackedBool(30298, completed); // Hidden Cache 2
		Stats::SetPackedBool(30299, completed); // Hidden Cache 3
		Stats::SetPackedBool(30300, completed); // Hidden Cache 4
		Stats::SetPackedBool(30301, completed); // Hidden Cache 5
		Stats::SetPackedBool(30302, completed); // Hidden Cache 6
		Stats::SetPackedBool(30303, completed); // Hidden Cache 7
		Stats::SetPackedBool(30304, completed); // Hidden Cache 8
		Stats::SetPackedBool(30305, completed); // Hidden Cache 9
		Stats::SetPackedBool(30306, completed); // Hidden Cache 10
		Stats::SetPackedBool(30307, completed); // Treasure Chest 1
		Stats::SetPackedBool(30308, completed); // Treasure Chest 2
		Stats::SetPackedBool(25522, completed); // Buried Stash 1
		Stats::SetPackedBool(25523, completed); // Buried Stash 2
		Stats::SetPackedBool(42252, completed); // LS Tag 1
		Stats::SetPackedBool(42253, completed); // LS Tag 2
		Stats::SetPackedBool(42254, completed); // LS Tag 3
		Stats::SetPackedBool(42255, completed); // LS Tag 4
		Stats::SetPackedBool(42256, completed); // LS Tag 5
		Stats::SetPackedBool(42269, completed); // Madrazo Hit
		Stats::SetPackedBool(42059, completed); // Shoot Animals Photography 1
		Stats::SetPackedBool(42060, completed); // Shoot Animals Photography 2
		Stats::SetPackedBool(42061, completed); // Shoot Animals Photography 3
		Stats::SetPackedBool(54672, completed); // Smoke on the Water Product 1
		Stats::SetPackedBool(54673, completed); // Smoke on the Water Product 2
		Stats::SetPackedBool(54674, completed); // Smoke on the Water Product 3
		Stats::SetPackedBool(54675, completed); // Smoke on the Water Product 4
		Stats::SetPackedBool(54676, completed); // Smoke on the Water Product 5
		Stats::SetPackedBool(54677, completed); // Smoke on the Water Product 6
		Stats::SetPackedBool(54678, completed); // Smoke on the Water Product 7
		Stats::SetPackedBool(54679, completed); // Smoke on the Water Product 8
		Stats::SetPackedBool(54680, completed); // Smoke on the Water Product 9
		Stats::SetPackedBool(54681, completed); // Smoke on the Water Product 10
		Stats::SetPackedBool(54735, completed); // Golden Clover
		// Street Dealers
		for (int i = 0; i < 3; ++i)
		{
			static Tunable maxCoke{1238316723};
			static Tunable maxMeth{658190943};
			static Tunable maxWeed{803541362};
			static Tunable maxAcid{3123173154};

			int statIndex = 41218 + i * 5;
			int premium = completed ? 0 : static_cast<int>(FreemodeGeneral::Get()->StreetDealers.Dealers[i].PremiumProduct);
			int coke = completed ? 0 : (maxCoke.IsReady() ? maxCoke.Get<int>() : 1);
			int meth = completed ? 0 : (maxMeth.IsReady() ? maxMeth.Get<int>() : 2);
			int weed = completed ? 0 : (maxWeed.IsReady() ? maxWeed.Get<int>() : 10);
			int acid = completed ? 0 : (maxAcid.IsReady() ? maxAcid.Get<int>() : 10);
			Stats::SetPackedInt(statIndex + 0, premium);
			Stats::SetPackedInt(statIndex + 1, coke);
			Stats::SetPackedInt(statIndex + 2, meth);
			Stats::SetPackedInt(statIndex + 3, weed);
			Stats::SetPackedInt(statIndex + 4, acid);
		}
		for (int i = 0; i < 10; i++)
		{
			// see TSE 1916113629
			int location = Stats::GetInt("MPX_DAILYCOLLECT_SKYDIVES" + std::to_string(i));
			Stats::SetPackedInt((34837 + i * 4), completed ? location : -1); // Junk Energy Skydives Checkpoint
			Stats::SetPackedInt((34839 + i * 4), completed ? location : -1); // Junk Energy Skydives Accurate Landing
			Stats::SetPackedInt((34838 + i * 4), completed ? location : -1); // Junk Energy Skydives Partime
			Stats::SetPackedInt((34840 + i * 4), completed ? location : -1); // Junk Energy Skydives Gold
		}
		for (int i = 34252; i <= 34261; i++)
		{
			Stats::SetPackedBool(i, completed); // Trick or Treat
		}
		for (int i = 34512; i <= 34701; i++)
		{
			Stats::SetPackedBool(i, completed); // Trick or Treat
		}
		int ttLocation = 0;
		int rcttLocation = 0;
		int bttLocation = 0;
		if (auto thread = Scripts::FindScriptThread("freemode"_J))
		{
			ttLocation = StandardTimeTrialData::Get(thread)->Location; // gets its value from tunable TIMETRIALVARIATION
			// we can actually manually calculate these using FreemodeGeneral::Get()->DailyReset.Seed % 14
			rcttLocation = RCBanditoTimeTrialData::Get(thread)->Location;
			bttLocation = BikeTimeTrialData::Get(thread)->Location;
		}
		Stats::SetInt("MPPLY_TIMETRIAL_COMPLETED_WEEK", completed ? ttLocation : -1); // Standard Time Trial
		Stats::SetInt("MPPLY_RCTTCOMPLETEDWEEK", completed ? rcttLocation : -1);      // RC Bandito Time Trial
		Stats::SetInt("MPPLY_BTTCOMPLETED", completed ? bttLocation : -1);            // Junk Energy Bike Time Trial
		Stats::SetInt("MPX_CBV_DELIVERED_BS", completed ? 1023 : 0);                  // Exotic Exports
		Stats::SetInt("MPX_CBV_STATE", completed);                                    // Exotic Exports
	}

	static void TeleportToCollectable(SCRIPT_EVENT_COLLECT_COLLECTABLE::eCollectables collectable, int index)
	{
		static ScriptFunction getCollectibleCoords("freemode"_J,
		    ScriptPointer("GetCollectibleCoords", "5D ? ? ? 7D 2C 10").Add(1).Rip());
		if (auto coords = getCollectibleCoords.Call<Vector3>(collectable, index))
		{
			Self::GetPed().TeleportTo(coords);
		}
	}

	static void CollectCollectable(SCRIPT_EVENT_COLLECT_COLLECTABLE::eCollectables collectable, int index)
	{
		SCRIPT_EVENT_COLLECT_COLLECTABLE data;
		data.CollectableType = collectable;
		data.CollectableIndex = index;
		data.SetCollected = TRUE;
		data.DisplayNotification = TRUE;
		data.Send();
	}

	static joaat_t GetNextExoticExportsVehicleHash()
	{
		constexpr ScriptGlobal order(1950369);
		for (int i = 1; i <= 10; i++)
		{
			auto offset = *order.At(i).As<int*>();
			if (!GPBD_Flow::Get()->Entries[Self::GetPlayer().GetId()].TunerCBVDeliveredFlags.IsSet(offset))
			{
				auto hash = *ScriptGlobal(1950358 + (offset + 1)).As<joaat_t*>();
				return hash;
			}
		}
		return 0;
	}

	static bool initialized = false;

	void OpenStreetDealerMenu::RunScriptImpl()
	{
		while (g_Running)
		{
			ScriptMgr::Yield();

			if (!m_ShouldRunScript)
				continue;

			if (!m_Thread)
			{
				int id = Scripts::StartScript("fm_street_dealer"_J);
				m_Thread = Scripts::FindScriptThreadByID(id);
				if (m_Thread)
				{
					m_Thread->m_Context.m_State = rage::scrThread::State::PAUSED;
				}
				else
				{
					m_ShouldRunScript = false;
					continue;
				}
			}

			auto streetDealerData = ScriptLocal(m_Thread, 258).At(12);

			if (!initialized)
			{
				auto selected = streetDealerIndex.GetState();
				FreemodeGeneral::Get()->StreetDealers.ClosestDealerLocation =
				    FreemodeGeneral::Get()->StreetDealers.Dealers[selected].Location;
				FreemodeGeneral::Get()->StreetDealers.ClosetsDealerIndex = selected;

				static ScriptFunction initStreetDealerData("fm_street_dealer"_J, ScriptPointer("InitStreetDealerData", "2D 00 07 00 00 61"));
				initStreetDealerData.Call<void>();
				streetDealerData.At(5).As<SCR_BITSET<uint64_t>*>()->Set(0);
				initialized = true;
			}

			static ScriptFunction runStreetDealerMenu("fm_street_dealer"_J, ScriptPointer("RunStreetDealerMenu", "2D 01 03 00 00 5D ? ? ? 2A"));
			runStreetDealerMenu.Call<void>(streetDealerData.As<int*>());

			if (streetDealerData.At(5).As<SCR_BITSET<uint64_t>*>()->IsSet(2) || !*Pointers.IsSessionStarted)
			{
				// if we don't reset these, freemode won't start the script legitimately
				FreemodeGeneral::Get()->StreetDealers.ClosestDealerLocation = -1;
				FreemodeGeneral::Get()->StreetDealers.ClosetsDealerIndex = -1;

				m_Thread->Kill();
				m_Thread->m_Context.m_State = rage::scrThread::State::KILLED;
				m_Thread = nullptr;
				initialized = false;
				m_ShouldRunScript = false;
			}
		}
	}

	class SetAllActivitiesCompleted : public Command
	{
		using Command::Command;

		virtual void OnCall() override
		{
			if (!Pointers.IsSessionStarted)
				return;

			SetAllDailyActivitiesCompleted(true);
			Notifications::Show("Daily Activities", "Completed all activities, switch session to apply the changes.");
		}
	};

	class ResetAllActivities : public Command
	{
		using Command::Command;

		virtual void OnCall() override
		{
			if (!Pointers.IsSessionStarted)
				return;

			SetAllDailyActivitiesCompleted(false);
			Notifications::Show("Daily Activities", "Reset all activities, switch session to apply the changes.");
		}
	};

	class CompleteAllChallenges : public Command
	{
		using Command::Command;

		virtual void OnCall() override
		{
			if (!Pointers.IsSessionStarted)
				return;

			for (int i = 0; i < 3; i++)
			{
				DailyObjectivesState::Get()->Objective[i] =
				    g_SavedMPGlobals::Get()->Entries[0].GeneralSaved.DOObjectives[i].Objective;
			}
			DailyObjectivesState::Get()->Completed = TRUE;
			WeeklyChallenge::Get()->Entries[0].CurrentObjective = WeeklyChallenge::Get()->Entries[0].ObjectiveCountOverride;
		}
	};

	class TeleportToHiddenCache : public Command
	{
		using Command::Command;

		virtual void OnCall() override
		{
			if (!*Pointers.IsSessionStarted)
				return;

			if (!Stats::GetPackedBool(30297 + hiddenCacheIndex.GetState()))
			{
				TeleportToCollectable(SCRIPT_EVENT_COLLECT_COLLECTABLE::eCollectables::HiddenCaches, hiddenCacheIndex.GetState());
			}
			else
			{
				Notifications::Show("Hidden Caches", "This Hidden Cache has already been collected.", NotificationType::Error);
			}
		}
	};

	class CollectHiddenCache : public Command
	{
		using Command::Command;

		virtual void OnCall() override
		{
			if (!*Pointers.IsSessionStarted)
				return;

			if (!Stats::GetPackedBool(30297 + hiddenCacheIndex.GetState()))
			{
				CollectCollectable(SCRIPT_EVENT_COLLECT_COLLECTABLE::eCollectables::HiddenCaches, hiddenCacheIndex.GetState());
			}
			else
			{
				Notifications::Show("Hidden Caches", "This Hidden Cache has already been collected.", NotificationType::Error);
			}
		}
	};

	class TeleportToTreasureChest : public Command
	{
		using Command::Command;

		virtual void OnCall() override
		{
			if (!*Pointers.IsSessionStarted)
				return;

			if (!Stats::GetPackedBool(30307 + treasureChestIndex.GetState()))
			{
				TeleportToCollectable(SCRIPT_EVENT_COLLECT_COLLECTABLE::eCollectables::TresureChests, treasureChestIndex.GetState());
			}
			else
			{
				Notifications::Show("Treasure Chests", "This Treasure Chest has already been collected.", NotificationType::Error);
			}
		}
	};

	class CollectTreasureChest : public Command
	{
		using Command::Command;

		virtual void OnCall() override
		{
			if (!*Pointers.IsSessionStarted)
				return;

			if (!Stats::GetPackedBool(30307 + treasureChestIndex.GetState()))
			{
				CollectCollectable(SCRIPT_EVENT_COLLECT_COLLECTABLE::eCollectables::TresureChests, treasureChestIndex.GetState());
			}
			else
			{
				Notifications::Show("Treasure Chests", "This Treasure Chest has already been collected.", NotificationType::Error);
			}
		}
	};

	class EnableTreasureChestInLS : public BoolCommand
	{
		using BoolCommand::BoolCommand;

		ScriptPatch m_EnableTreasureChestInLSPatch{};

		virtual void OnEnable() override
		{
			if (!m_EnableTreasureChestInLSPatch)
			{
				m_EnableTreasureChestInLSPatch = ScriptPatches::AddPatch("freemode"_J, ScriptPointer("EnableTreasureChestInLSPatch", "1F 56 ? ? 38 00 47 ? ? 73"), {0x72});
			}
			m_EnableTreasureChestInLSPatch->Enable();
		}

		virtual void OnDisable() override
		{
			if (m_EnableTreasureChestInLSPatch)
			{
				m_EnableTreasureChestInLSPatch->Disable();
			}
		}
	};

	class TeleportToShipwrecked : public Command
	{
		using Command::Command;

		virtual void OnCall() override
		{
			if (!*Pointers.IsSessionStarted)
				return;

			if (!Stats::GetPackedBool(31734))
			{
				TeleportToCollectable(SCRIPT_EVENT_COLLECT_COLLECTABLE::eCollectables::Shipwrecked, 0);
			}
			else
			{
				Notifications::Show("Shipwreck", "Shipwreck has already been collected.", NotificationType::Error);
			}
		}
	};

	class CollectShipwrecked : public Command
	{
		using Command::Command;

		virtual void OnCall() override
		{
			if (!*Pointers.IsSessionStarted)
				return;

			if (!Stats::GetPackedBool(31734))
			{
				CollectCollectable(SCRIPT_EVENT_COLLECT_COLLECTABLE::eCollectables::Shipwrecked, 0);
			}
			else
			{
				Notifications::Show("Shipwreck", "Shipwreck has already been collected.", NotificationType::Error);
			}
		}
	};

	class TeleportToBuriedStash : public Command
	{
		using Command::Command;

		virtual void OnCall() override
		{
			if (!*Pointers.IsSessionStarted)
				return;

			if (!Stats::GetPackedBool(25522 + buriedStashIndex.GetState()))
			{
				TeleportToCollectable(SCRIPT_EVENT_COLLECT_COLLECTABLE::eCollectables::BuriedStash, buriedStashIndex.GetState());
			}
			else
			{
				Notifications::Show("Buried Stashes", "This Buried Stash has already been collected.", NotificationType::Error);
			}
		}
	};

	class CollectBuriedStash : public Command
	{
		using Command::Command;

		virtual void OnCall() override
		{
			if (!*Pointers.IsSessionStarted)
				return;

			if (!Stats::GetPackedBool(25522 + buriedStashIndex.GetState()))
			{
				CollectCollectable(SCRIPT_EVENT_COLLECT_COLLECTABLE::eCollectables::BuriedStash, buriedStashIndex.GetState());
			}
			else
			{
				Notifications::Show("Buried Stashes", "This Buried Stash has already been collected.", NotificationType::Error);
			}
		}
	};

	class EnableBuriedStashInLS : public BoolCommand
	{
		using BoolCommand::BoolCommand;

		ScriptPatch m_EnableBuriedStashInLSPatch{};

		virtual void OnEnable() override
		{
			if (!m_EnableBuriedStashInLSPatch)
			{
				m_EnableBuriedStashInLSPatch = ScriptPatches::AddPatch("freemode"_J, ScriptPointer("EnableBuriedStashInLSPatch", "71 39 02 38 02 06 56 ? ? 2C"), {0x72});
			}
			m_EnableBuriedStashInLSPatch->Enable();
		}

		virtual void OnDisable() override
		{
			if (m_EnableBuriedStashInLSPatch)
			{
				m_EnableBuriedStashInLSPatch->Disable();
			}
		}
	};

	class TeleportToSkydive : public Command
	{
		using Command::Command;

		virtual void OnCall() override
		{
			if (!*Pointers.IsSessionStarted)
				return;

			TeleportToCollectable(SCRIPT_EVENT_COLLECT_COLLECTABLE::eCollectables::Skydives, skydiveIndex.GetState()); // no stat check as you can complete a skydive multiple times
		}
	};

	class CompleteSkydive : public Command
	{
		using Command::Command;

		virtual void OnCall() override
		{
			if (!*Pointers.IsSessionStarted)
				return;

			static Tunable checkpointReward{"SKYDIVING_CHALLENGE_CASH_REWARD_ALL_CHECKPOINTS_COLLECTED"_J};
			static Tunable parTimeReward{"SKYDIVING_CHALLENGE_CASH_REWARD_PAR_TIME"_J};
			static Tunable landingReward{"SKYDIVING_CHALLENGE_CASH_REWARD_ACCURATE_LANDING"_J};

			if (!checkpointReward.IsReady() || !parTimeReward.IsReady() || !landingReward.IsReady())
				return;

			int index = skydiveIndex.GetState();
			int location = Stats::GetInt("MPX_DAILYCOLLECT_SKYDIVES" + std::to_string(index));
			bool checkpoints = Stats::GetPackedInt(34837 + (index * 4)) == location;
			bool partime = Stats::GetPackedInt(34838 + (index * 4)) == location;
			bool landing = Stats::GetPackedInt(34839 + (index * 4)) == location;

			*ScriptGlobal(1979961).As<int*>() = 1;
			*ScriptGlobal(1979961).At(1).As<int*>() = 1;
			*ScriptGlobal(1979961).At(3).As<int*>() = 5;
			*ScriptGlobal(1979961).At(4).As<int*>() = location + 1;
			*ScriptGlobal(1979961).At(8).At(1).As<int*>() = checkpointReward.Get<int>() / (1 + (checkpoints * 9));
			*ScriptGlobal(1979961).At(8).At(2).As<int*>() = parTimeReward.Get<int>() / (1 + (partime * 9));
			*ScriptGlobal(1979961).At(8).At(3).As<int*>() = landingReward.Get<int>() / (1 + (landing * 9));

			SET_SKYDIVE_COMPLETED data;
			data.SkydiveIndex = index;
			data.SkydiveLocation = location;
			data.AllCheckpointsHit = TRUE;
			data.ParTimeBeaten = TRUE;
			data.AccurateLanding = TRUE;
			data.Send();

			ScriptGlobal(1983930).At(4).As<SCR_BITSET<uint64_t>*>()->Clear(3);
		}
	};

	class TeleportToTimeTrial : public Command
	{
		using Command::Command;

		virtual void OnCall() override
		{
			if (!*Pointers.IsSessionStarted)
				return;

			std::string stat = "";
			switch (timeTrialIndex.GetState())
			{
			case 0: stat = "MPPLY_TIMETRIAL_COMPLETED_WEEK"; break;
			case 1: stat = "MPPLY_RCTTCOMPLETEDWEEK"; break;
			case 2: stat = "MPPLY_BTTCOMPLETED"; break;
			}

			if (stat.empty() || Stats::GetInt(stat) != -1)
			{
				Notifications::Show("Time Trials", "This Time Trial has already been beaten.", NotificationType::Error);
				return;
			}

			int sprite = -1;
			switch (timeTrialIndex.GetState())
			{
			case 0: sprite = static_cast<int>(BlipSprite::RADAR_TEMP_2); break;
			case 1: sprite = static_cast<int>(BlipSprite::RADAR_RC_TIME_TRIALS); break;
			case 2: sprite = static_cast<int>(BlipSprite::RADAR_BICYCLE_TRIAL); break;
			}

			auto blip = HUD::GET_FIRST_BLIP_INFO_ID(sprite);
			if (HUD::DOES_BLIP_EXIST(blip))
			{
				if (auto coords = HUD::GET_BLIP_COORDS(blip))
				{
					Self::GetPed().TeleportTo(coords);
				}
			}
			else
			{
				Notifications::Show("Time Trials", "Please enable the blip from the Interaction Menu.", NotificationType::Error);
			}
		}
	};

	class BeatTimeTrial : public Command
	{
		using Command::Command;

		virtual void OnCall() override
		{
			if (!*Pointers.IsSessionStarted)
				return;

			std::string stat = "";
			switch (timeTrialIndex.GetState())
			{
			case 0: stat = "MPPLY_TIMETRIAL_COMPLETED_WEEK"; break;
			case 1: stat = "MPPLY_RCTTCOMPLETEDWEEK"; break;
			case 2: stat = "MPPLY_BTTCOMPLETED"; break;
			}

			if (stat.empty() || Stats::GetInt(stat) != -1)
			{
				Notifications::Show("Time Trials", "This Time Trial has already been beaten.", NotificationType::Error);
				return;
			}

			rage::scrThread* thread = nullptr;
			if (timeTrialIndex.GetState() == 2)
			{
				if (Scripts::IsScriptActive("fm_content_bicycle_time_trial"_J))
					return;

				MP_SCRIPT_DATA garbage{};
				thread = Scripts::FindScriptThreadByID(Scripts::StartScript("fm_content_bicycle_time_trial"_J, eStackSizes::MULTIPLAYER_FREEMODE, &garbage, SCR_SIZEOF(garbage)));
			}
			else
			{
				thread = Scripts::FindScriptThread("freemode"_J);
			}

			if (!thread)
				return;

			if (timeTrialIndex.GetState() == 0)
			{
				if (auto ttData = StandardTimeTrialData::Get(thread))
				{
					ttData->Timer1.Time = *Pointers.NetworkTime;
					static ScriptFunction beatStandardTimeTrial("freemode"_J, ScriptPointer("BeatStandardTimeTrial", "2D 01 19 00 00 38"));
					beatStandardTimeTrial.Call<void>(ttData);
				}
			}
			else if (timeTrialIndex.GetState() == 1)
			{
				if (auto rcttData = RCBanditoTimeTrialData::Get(thread))
				{
					rcttData->Timer1.Time = *Pointers.NetworkTime;
					static ScriptFunction beatRCTimeTrial("freemode"_J, ScriptPointer("BeatRCTimeTrial", "2D 01 17 00 00 38 00 40"));
					beatRCTimeTrial.Call<void>(rcttData);
				}
			}
			else if (timeTrialIndex.GetState() == 2)
			{
				thread->m_Context.m_State = rage::scrThread::State::PAUSED;

				*ScriptLocal(thread, 3103).At(131).At(1).As<int*>() = FreemodeGeneral::Get()->DailyReset.Seed % 14; // if we don't init this, the par time duration function will return 0 and the COMPLETED stat will be set to 0, which is bad
				*ScriptLocal(thread, 146).At(4).As<int*>() = 0;
				static ScriptFunction onBTTEnd("fm_content_bicycle_time_trial"_J,
				    ScriptPointer("OnBTTEnd", "64 ? ? ? 5D ? ? ? 75 77").Add(1).Rip());
				onBTTEnd.Call<void>();

				thread->m_Context.m_State = rage::scrThread::State::KILLED;
			}
		}
	};

	class TeleportToExoticExportsVehicle : public Command
	{
		using Command::Command;

		virtual void OnCall() override
		{
			if (!*Pointers.IsSessionStarted)
				return;

			if (GPBD_Flow::Get()->Entries[Self::GetPlayer().GetId()].TunerCBVDeliveredFlags.Value == 1023)
			{
				Notifications::Show("Exotic Exports", "You have already delivered all the vehicles.", NotificationType::Error);
				return;
			}

			if (GSBD_RandomEvents::Get()->EventData[3].State != eRandomEventState::INACTIVE)
			{
				if (auto coords = GSBD_RandomEvents::Get()->EventData[3].TriggerPosition)
					Self::GetPed().TeleportTo(coords);
			}
		}
	};

	class DeliverNextExoticExportsVehicle : public Command
	{
		using Command::Command;

		virtual void OnCall() override
		{
			if (!*Pointers.IsSessionStarted)
				return;

			if (GPBD_Flow::Get()->Entries[Self::GetPlayer().GetId()].TunerCBVDeliveredFlags.Value == 1023)
			{
				Notifications::Show("Exotic Exports", "You have already delivered all the vehicles.", NotificationType::Error);
				return;
			}

			/*if (!FreemodeDelivery::Get()->DeliveryCooldown1.HasTimePassed(30000))
			{
				Notifications::Show("Exotic Exports", "You have just delivered a vehicle, wait a moment.", NotificationType::Error);
				return;
			}*/

			auto hash = GetNextExoticExportsVehicleHash();
			static ScriptFunction onFreemodeDeliveryEnd("freemode"_J, ScriptPointer("OnFreemodeDeliveryEnd", "2D 0C 2A 00 00"));
			onFreemodeDeliveryEnd.Call<void>(0, Self::GetPlayer().GetId(), 0, 0, 0, 0, 0, 0, 0, 0, hash, 273);
			//FreemodeDelivery::Get()->DeliveryCooldown1.Reset();
		}
	};

	class TeleportToDeadDrop : public Command
	{
		using Command::Command;

		virtual void OnCall() override
		{
			if (!*Pointers.IsSessionStarted)
				return;

			if (!Stats::GetPackedBool(36628))
			{
				TeleportToCollectable(SCRIPT_EVENT_COLLECT_COLLECTABLE::eCollectables::DeadDrop, 0);
			}
			else
			{
				Notifications::Show("G's Cache", "G's Cache has already been collected.", NotificationType::Error);
			}
		}
	};

	class CollectDeadDrop : public Command
	{
		using Command::Command;

		virtual void OnCall() override
		{
			if (!*Pointers.IsSessionStarted)
				return;

			if (!Stats::GetPackedBool(36628))
			{
				CollectCollectable(SCRIPT_EVENT_COLLECT_COLLECTABLE::eCollectables::DeadDrop, 0);
			}
			else
			{
				Notifications::Show("G's Cache", "G's Cache has already been collected.", NotificationType::Error);
			}
		}
	};

	class TeleportToStashHouse : public Command
	{
		using Command::Command;

		virtual void OnCall() override
		{
			if (!*Pointers.IsSessionStarted)
				return;

			if (!Stats::GetPackedBool(36657))
			{
				auto blip = HUD::GET_FIRST_BLIP_INFO_ID(static_cast<int>(BlipSprite::RADAR_STASH_HOUSE));
				if (HUD::DOES_BLIP_EXIST(blip))
				{
					if (auto coords = HUD::GET_BLIP_COORDS(blip))
						Self::GetPed().TeleportTo(coords);
				}
			}
			else
			{
				Notifications::Show("Stash House", "Stash House has already been raided.", NotificationType::Error);
			}
		}
	};

	class EnterStashHouseSafeCode : public Command
	{
		using Command::Command;

		virtual void OnCall() override
		{
			if (!*Pointers.IsSessionStarted)
				return;

			if (auto thread = Scripts::FindScriptThread("fm_content_stash_house"_J))
			{
				for (int i = 0; i < 3; i++)
				{
					int combination = *ScriptLocal(thread, 146).At(22).At(i, 2).At(1).As<int*>();
					*ScriptLocal(thread, 146).At(22).At(i, 2).As<float*>() = combination;
				}
			}
		}
	};

	class TeleportToStreetDealer : public Command
	{
		using Command::Command;

		virtual void OnCall() override
		{
			if (!*Pointers.IsSessionStarted)
				return;

			static ScriptFunction getStreetDealerCoords("freemode"_J,
			    ScriptPointer("GetStreetDealerCoords", "5D ? ? ? 5D ? ? ? 5D ? ? ? 18 1F").Add(1).Rip());
			if (auto coords = getStreetDealerCoords.Call<Vector3>(
			        FreemodeGeneral::Get()->StreetDealers.Dealers[streetDealerIndex.GetState()].Location))
				Self::GetPed().TeleportTo(coords);
		}
	};

	class _OpenStreetDealerMenu : public Command
	{
		using Command::Command;

		virtual void OnCall() override
		{
			if (!*Pointers.IsSessionStarted || Scripts::IsScriptActive("fm_street_dealer"_J))
				return;

			OpenStreetDealerMenu::SetShouldRunScript(true);
		}
	};

	class TeleportToLSTag : public Command
	{
		using Command::Command;

		virtual void OnCall() override
		{
			if (!*Pointers.IsSessionStarted)
				return;

			if (!Stats::GetPackedBool(42252 + lsTagIndex.GetState()))
			{
				TeleportToCollectable(SCRIPT_EVENT_COLLECT_COLLECTABLE::eCollectables::Tagging, lsTagIndex.GetState());
			}
			else
			{
				Notifications::Show("LS Tags", "This LS Tag has already been sprayed.", NotificationType::Error);
			}
		}
	};

	class SprayLSTag : public Command
	{
		using Command::Command;

		virtual void OnCall() override
		{
			if (!*Pointers.IsSessionStarted)
				return;

			if (!Stats::GetPackedBool(42252 + lsTagIndex.GetState()))
			{
				CollectCollectable(SCRIPT_EVENT_COLLECT_COLLECTABLE::eCollectables::Tagging, lsTagIndex.GetState());
			}
			else
			{
				Notifications::Show("LS Tags", "This LS Tag has already been sprayed.", NotificationType::Error);
			}
		}
	};

	class TeleportToMadrazoHit : public Command
	{
		using Command::Command;

		virtual void OnCall() override
		{
			if (!*Pointers.IsSessionStarted)
				return;

			if (Stats::GetInt("MPX_BAIL_OFFICE_OWNED") == 0)
			{
				Notifications::Show("Madrazo Hits", "You must own a Bail Office.", NotificationType::Error);
				return;
			}

			if (!Stats::GetPackedBool(42269))
			{
				auto blip = HUD::GET_FIRST_BLIP_INFO_ID(static_cast<int>(BlipSprite::RADAR_DAILY_BOUNTY));
				if (HUD::DOES_BLIP_EXIST(blip))
				{
					if (auto coords = HUD::GET_BLIP_COORDS(blip))
						Self::GetPed().TeleportTo(coords);
				}
			}
			else
			{
				Notifications::Show("Madrazo Hits", "Madrazo Hit has already been completed.", NotificationType::Error);
			}
		}
	};

	class TeleportToMadrazoHitTarget : public Command
	{
		using Command::Command;

		virtual void OnCall() override
		{
			if (!*Pointers.IsSessionStarted)
				return;

			if (auto thread = Scripts::FindScriptThread("fm_content_daily_bounty"_J))
			{
				if (auto coords = *ScriptLocal(thread, 243).At(434).At(1).At(0, 4).As<Vector3*>())
				{
					Self::GetPed().TeleportTo(coords);
				}
			}
		}
	};

	class SpawnAnimal : public Command
	{
		using Command::Command;

		virtual void OnCall() override
		{
			if (!*Pointers.IsSessionStarted)
				return;

			if (!Stats::GetPackedBool(42059 + animalIndex.GetState()))
			{
				int index = Stats::GetPackedInt(28091 + animalIndex.GetState());
				if (index < 0 || index >= wildlifePhotographyAnimalHashes.size())
					return;

				Ped::Create(wildlifePhotographyAnimalHashes[index], Self::GetPed().GetPosition());
			}
			else
			{
				Notifications::Show("Shoot Animals Photography", "This animal has already been photographed.", NotificationType::Error);
			}
		}
	};

	class PhotographAnimal : public Command
	{
		using Command::Command;

		virtual void OnCall() override
		{
			if (!*Pointers.IsSessionStarted)
				return;

			if (!Stats::GetPackedBool(42059 + animalIndex.GetState()))
			{
				int index = Stats::GetPackedInt(28091 + animalIndex.GetState());
				if (index < 0 || index >= wildlifePhotographyAnimalHashes.size())
					return;

				ScriptGlobal(2709409).At(545).As<SCR_BITSET<uint64_t>*>()->Set(6);
				*ScriptGlobal(2709409).At(549).As<joaat_t*>() = wildlifePhotographyAnimalHashes[index];
				*ScriptGlobal(2709409).At(550).As<int*>() = *Pointers.GameTimer - 1; // bypass 2 sec delay
			}
			else
			{
				Notifications::Show("Shoot Animals Photography", "This animal has already been photographed.", NotificationType::Error);
			}
		}
	};

	class TeleportToProduct : public Command
	{
		using Command::Command;

		virtual void OnCall() override
		{
			if (!*Pointers.IsSessionStarted)
				return;

			if (Stats::GetInt("MPX_SB_WEED_SHOP_OWNED") == 0)
			{
				Notifications::Show("Smoke on the Water", "You must own a Smoke on the Water property.", NotificationType::Error);
				return;
			}

			if (!Stats::GetPackedBool(54672 + productIndex.GetState()))
			{
				TeleportToCollectable(SCRIPT_EVENT_COLLECT_COLLECTABLE::eCollectables::SmokeOnTheWater, productIndex.GetState());
			}
			else
			{
				Notifications::Show("Smoke on the Water", "This product has already been collected.", NotificationType::Error);
			}
		}
	};

	class CollectProduct : public Command
	{
		using Command::Command;

		virtual void OnCall() override
		{
			if (!*Pointers.IsSessionStarted)
				return;

			if (Stats::GetInt("MPX_SB_WEED_SHOP_OWNED") == 0)
			{
				Notifications::Show("Smoke on the Water", "You must own a Smoke on the Water property.", NotificationType::Error);
				return;
			}

			if (!Stats::GetPackedBool(54672 + productIndex.GetState()))
			{
				CollectCollectable(SCRIPT_EVENT_COLLECT_COLLECTABLE::eCollectables::SmokeOnTheWater, productIndex.GetState());
			}
			else
			{
				Notifications::Show("Smoke on the Water", "This product has already been collected.", NotificationType::Error);
			}
		}
	};

	class TeleportToGoldenClover : public Command
	{
		using Command::Command;

		virtual void OnCall() override
		{
			if (!*Pointers.IsSessionStarted)
				return;

			if (!Stats::GetPackedBool(54735))
			{
				TeleportToCollectable(SCRIPT_EVENT_COLLECT_COLLECTABLE::eCollectables::GoldenClover, 0);
			}
			else
			{
				Notifications::Show("Golden Clover", "Golden Clover has already been collected.", NotificationType::Error);
			}
		}
	};

	class CollectGoldenClover : public Command
	{
		using Command::Command;

		virtual void OnCall() override
		{
			if (!*Pointers.IsSessionStarted)
				return;

			if (!Stats::GetPackedBool(54735))
			{
				CollectCollectable(SCRIPT_EVENT_COLLECT_COLLECTABLE::eCollectables::GoldenClover, 0);
			}
			else
			{
				Notifications::Show("Golden Clover", "Golden Clover has already been collected.", NotificationType::Error);
			}
		}
	};

	static SetAllActivitiesCompleted _SetAllActivitiesCompleted{"setallactivitiescompleted", "设置所有活动已完成", "切换战局以应用更改。"};
	static ResetAllActivities _ResetAllActivities{"resetallactivities", "重置所有活动", "切换战局以应用更改。"};

	static CompleteAllChallenges _CompleteAllChallenges{"completeallchallenges", "完成所有挑战", "完成所有每日目标和每周挑战。"};

	static TeleportToHiddenCache _TeleportToHiddenCache{"tptohiddencache", "传送到隐藏宝箱", "传送到选定的隐藏宝箱。"};
	static CollectHiddenCache _CollectHiddenCache{"collecthiddencache", "收集隐藏宝箱", "收集选定的隐藏宝箱。"};

	static TeleportToTreasureChest _TeleportToTreasureChest{"tptotreasurechest", "传送到宝箱", "传送到选定的宝箱。"};
	static CollectTreasureChest _CollectTreasureChest{"collecttreasurechest", "收集宝箱", "收集选定的宝箱。"};
	static EnableTreasureChestInLS _EnableTreasureChestInLS{"enabletreasurechestinls", "在洛圣都启用宝箱", "在洛圣都启用宝箱，这样你就不必去佩里科岛。"};

	static TeleportToShipwrecked _TeleportToShipwrecked{"tptoshipwrecked", "传送到沉船", "传送到沉船。"};
	static CollectShipwrecked _CollectShipwrecked{"collectshipwrecked", "收集沉船", "收集沉船。"};

	static TeleportToBuriedStash _TeleportToBuriedStash{"tptoburiedstash", "传送到埋藏藏品", "传送到选定的埋藏藏品。"};
	static CollectBuriedStash _CollectBuriedStash{"collectburiedstash", "收集埋藏藏品", "收集选定的埋藏藏品。"};
	static EnableBuriedStashInLS _EnableBuriedStashInLS{"enableburiedstashinls", "在洛圣都启用埋藏藏品", "在洛圣都启用埋藏藏品，这样你就不必去佩里科岛。"};

	static TeleportToSkydive _TeleportToSkydive{"tptoskydive", "传送到跳伞点", "传送到选定的跳伞点。"};
	static CompleteSkydive _CompleteSkydive{"completeskydive", "完成跳伞", "完成选定的跳伞。"};

	static TeleportToTimeTrial _TeleportToTimeTrial{"tptotimetrial", "传送到时间挑战赛", "传送到选定的时间挑战赛。"};
	static BeatTimeTrial _BeatTimeTrial{"beattimetrial", "完成时间挑战赛", "完成选定的时间挑战赛。"};

	static TeleportToExoticExportsVehicle _TeleportToExoticExportsVehicle{"tptoexoticexportsvehicle", "传送到进口载具", "传送到下一个进口载具。"};
	static DeliverNextExoticExportsVehicle _DeliverNextExoticExportsVehicle{"delivernextexoticexportsvehicle", "交付下一个进口载具", "直接交付下一个进口载具。"};

	static TeleportToDeadDrop _TeleportToDeadDrop{"tptodeaddrop", "传送到G的缓存", "传送到G的缓存。"};
	static CollectDeadDrop _CollectDeadDrop{"collectdeaddrop", "收集G的缓存", "收集G的缓存。"};

	static TeleportToStashHouse _TeleportToStashHouse{"tptostashhouse", "传送到藏匿处", "传送到藏匿处。"};
	static EnterStashHouseSafeCode _EnterStashHouseSafeCode{"enterstashhousesafecode", "输入藏匿处保险箱密码", "输入藏匿处保险箱密码。"};

	static TeleportToStreetDealer _TeleportToStreetDealer{"tptostreetdealer", "传送到街头毒贩", "传送到选定的街头毒贩。"};
	static _OpenStreetDealerMenu __OpenStreetDealerMenu{"openstreetdealermenu", "打开街头毒贩菜单", "允许你远程访问选定的街头毒贩。"};

	static TeleportToLSTag _TeleportToLSTag{"tptolstag", "传送到洛圣都涂鸦", "传送到选定的洛圣都涂鸦。"};
	static SprayLSTag _SprayLSTag{"spraylstag", "喷涂洛圣都涂鸦", "喷涂选定的洛圣都涂鸦。"};

	static TeleportToMadrazoHit _TeleportToMadrazoHit{"tptomadrazohit", "传送到玛德拉索任务", "传送到玛德拉索任务。"};
	static TeleportToMadrazoHitTarget _TeleportToMadrazoHitTarget{"tptomadrazohittarget", "传送到玛德拉索任务目标", "传送到玛德拉索任务目标。"};

	static SpawnAnimal _SpawnAnimal{"spawnanimal", "生成动物", "生成选定的动物。"};
	static PhotographAnimal _PhotographAnimal{"photographanimal", "拍摄动物", "拍摄选定的动物。"};

	static TeleportToProduct _TeleportToProduct{"tptoproduct", "传送到产品", "传送到选定的产品。"};
	static CollectProduct _CollectProduct{"collectproduct", "收集产品", "收集选定的产品。"};

	static TeleportToGoldenClover _TeleportToGoldenClover{"tptogoldenclover", "传送到金色三叶草", "传送到金色三叶草。"};
	static CollectGoldenClover _CollectGoldenClover{"collectgoldenclover", "收集金色三叶草", "收集金色三叶草。"};
}