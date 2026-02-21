#include "core/commands/Command.hpp"
#include "core/commands/IntCommand.hpp"
#include "core/commands/ListCommand.hpp"
#include "game/gta/Stats.hpp"
#include "core/backend/FiberPool.hpp"
#include "game/gta/ScriptGlobal.hpp"
#include "game/gta/ScriptLocal.hpp"
#include "core/backend/ScriptMgr.hpp"
#include "game/backend/Tunables.hpp"

namespace YimMenu::Features
{
	namespace DiamondCasinoHeist
	{
		static IntCommand _DiamondCasinoHeistCut1{"diamondcasinoheistcut1", "玩家1", "玩家1分红", std::nullopt, std::nullopt, 0};
		static IntCommand _DiamondCasinoHeistCut2{"diamondcasinoheistcut2", "玩家2", "玩家2分红", std::nullopt, std::nullopt, 0};
		static IntCommand _DiamondCasinoHeistCut3{"diamondcasinoheistcut3", "玩家3", "玩家3分红", std::nullopt, std::nullopt, 0};
		static IntCommand _DiamondCasinoHeistCut4{"diamondcasinoheistcut4", "玩家4", "玩家4分红", std::nullopt, std::nullopt, 0};

		class SetCuts : public Command
		{
			using Command::Command;

			virtual void OnCall() override
			{
				auto base = ScriptGlobal(1973231).At(1497).At(736).At(92);

				*base.At(0, 1).As<int*>() = _DiamondCasinoHeistCut1.GetState();
				*base.At(1, 1).As<int*>() = _DiamondCasinoHeistCut2.GetState();
				*base.At(2, 1).As<int*>() = _DiamondCasinoHeistCut3.GetState();
				*base.At(3, 1).As<int*>() = _DiamondCasinoHeistCut4.GetState();
			}
		};

		class ForceReady : public Command
		{
			using Command::Command;

			virtual void OnCall() override
			{
				auto base = ScriptGlobal(1977594);

				for (int i = 0; i <= 3; i++)
				{
					*base.At(i, 68).At(7).At(i, 1).As<int*>() = 1;
				}
			}
		};

		static std::vector<std::pair<int, const char*>> diamondCasinoHeistDifficulty = {
			{0, "普通"},
			{1, "困难"}
		};
		static ListCommand _DiamondCasinoHeistDifficulty{"diamondcasinoheistdifficulty", "难度", "抢劫难度", diamondCasinoHeistDifficulty, 0};

		static std::vector<std::pair<int, const char*>> diamondCasinoHeistPrimaryTarget = {
			{3, "钻石"},
			{1, "黄金"},
			{2, "艺术品"},
			{0, "现金"}
		};
		static ListCommand _DiamondCasinoHeistPrimaryTarget{"diamondcasinoheistprimarytarget", "主要目标", "主要目标", diamondCasinoHeistPrimaryTarget, 3};

		static ListCommand* _DiamondCasinoHeistGunmanPtr = nullptr;
		static ListCommand* _DiamondCasinoHeistApproachPtr = nullptr;

		static std::vector<std::vector<std::vector<std::pair<int, const char*>>>> diamondCasinoHeistWeapon = {
			{
				{{0, "MK II Shotgun Loadout"}, {1, "MK II Rifle Loadout"}},
				{{0, "MK II SMG Loadout"}, {1, "MK II Rifle Loadout"}},
				{{0, "MK II Shotgun Loadout"}, {1, "MK II Rifle Loadout"}}
			},
			{
				{{0, "Rifle Loadout"}, {1, "Shotgun Loadout"}},
				{{0, "Rifle Loadout"}, {1, "Shotgun Loadout"}},
				{{0, "Rifle Loadout"}, {1, "Shotgun Loadout"}}
			},
			{
				{{0, "Combat PDW Loadout"}, {1, "Rifle Loadout"}},
				{{0, "Shotgun Loadout"}, {1, "Rifle Loadout"}},
				{{0, "Shotgun Loadout"}, {1, "Combat MG Loadout"}}
			},
			{
				{{0, "SMG Loadout"}, {1, "Shotgun Loadout"}},
				{{0, "Machine Pistol Loadout"}, {1, "Shotgun Loadout"}},
				{{0, "SMG Loadout"}, {1, "Shotgun Loadout"}}
			},
			{
				{{0, "Micro SMG Loadout"}, {1, "Machine Pistol Loadout"}},
				{{0, "Micro SMG Loadout"}, {1, "Shotgun Loadout"}},
				{{0, "Shotgun Loadout"}, {1, "Revolver Loadout"}}},
			{
				{{0, "                                        "}, {1, ""}},
				{{0, ""}, {1, ""}},
				{{0, ""}, {1, ""}}
			}
		};
		static ListCommand _DiamondCasinoHeistWeapon{"diamondcasinoheistweapon", "武器", "武器", diamondCasinoHeistWeapon[5][0], 0};

		static std::vector<std::pair<int, const char*>> diamondCasinoHeistGunman = {
			{0, "Chester McCoy"},
			{1, "Gustavo Mota"},
			{2, "Patrick McReary"},
			{3, "Charlie Reed"},
			{4, "Karl Abolaji"},
			{5, "Remove Gunman"}
		};

		class Gunman : public ListCommand
		{
			using ListCommand::ListCommand;

			virtual void OnChange() override
			{
				_DiamondCasinoHeistWeapon.SetList(diamondCasinoHeistWeapon[this->GetState()][_DiamondCasinoHeistApproachPtr->GetState()]);
				_DiamondCasinoHeistWeapon.SetState(0);
			}
		};
		static Gunman _DiamondCasinoHeistGunman{"diamondcasinoheistgunman", "枪手", "枪手", diamondCasinoHeistGunman, 5};

		static std::vector<std::pair<int, const char*>> diamondCasinoHeistApproach = {
			{0, "Silent & Sneaky"},
			{1, "The Big Con"},
			{2, "Aggressive"}
		};

		class Approach : public ListCommand
		{
			using ListCommand::ListCommand;

			virtual void OnChange() override
			{
				_DiamondCasinoHeistWeapon.SetList(diamondCasinoHeistWeapon[_DiamondCasinoHeistGunmanPtr->GetState()][this->GetState()]);
				_DiamondCasinoHeistWeapon.SetState(0);
			}
		};
		static Approach _DiamondCasinoHeistApproach{"diamondcasinoheistapproach", "方案", "抢劫方案", diamondCasinoHeistApproach, 0};

		static struct InitPtrs
		{
			InitPtrs()
			{
				_DiamondCasinoHeistGunmanPtr = &_DiamondCasinoHeistGunman;
				_DiamondCasinoHeistApproachPtr = &_DiamondCasinoHeistApproach;
			}
		} _initPtrs;

		static std::vector<std::vector<std::pair<int, const char*>>> diamondCasinoHeistVehicle = {
			{{0, "Zhaba"}, {1, "Vagrant"}, {2, "Outlaw"}, {3, "Everon"}},
			{{0, "Sultan Classic"}, {1, "Gauntlet Classic"}, {2, "Ellie"}, {3, "Komoda"}},
			{{0, "Retinue MK II"}, {1, "Drifty Yosemite"}, {2, "Sugoi"}, {3, "Jugular"}},
			{{0, "Manchez"}, {1, "Stryder"}, {2, "Defiler"}, {3, "Lectro"}},
			{{0, "Issi Classic"}, {1, "Asbo"}, {2, "Kanjo"}, {3, "Sentinel Classic"}},
			{{0, "                           "}, {1, ""}, {2, ""}, {3, ""}}
		};
		static ListCommand _DiamondCasinoHeistVehicle{"diamondcasinoheistvehicle", "载具", "载具", diamondCasinoHeistVehicle[5], 0};

		static std::vector<std::pair<int, const char*>> diamondCasinoHeistDriver = {
			{0, "Chester McCoy"},
			{1, "Eddie Toh"},
			{2, "Taliana Martinez"},
			{3, "Zach Nelson"},
			{4, "Karim Denz"},
			{5, "Remove Driver"}
		};

		class Driver : public ListCommand
		{
			using ListCommand::ListCommand;

			virtual void OnChange() override
			{
				_DiamondCasinoHeistVehicle.SetList(diamondCasinoHeistVehicle[this->GetState()]);
				_DiamondCasinoHeistVehicle.SetState(0);
			}
		};
		static Driver _DiamondCasinoHeistDriver{"diamondcasinoheistdriver", "司机", "司机", diamondCasinoHeistDriver, 5};

		static std::vector<std::pair<int, const char*>> diamondCasinoHeistHacker = {
			{4, "Avi Schwartzman"},
			{5, "Paige Harris"},
			{2, "Christian Feltz"},
			{3, "Yohan Blair"},
			{1, "Rickie Lukens"},
			{6, "Remove Hacker"}
		};
		static ListCommand _DiamondCasinoHeistHacker{"diamondcasinoheisthacker", "黑客", "黑客", diamondCasinoHeistHacker, 6};

		class Setup : public Command
		{
			using Command::Command;

			virtual void OnCall() override
			{
				Stats::SetInt("MPX_H3_COMPLETEDPOSIX", -1);
				Stats::SetInt("MPX_H3OPT_MASKS", 4);
				Stats::SetInt("MPX_H3OPT_WEAPS", _DiamondCasinoHeistWeapon.GetState());
				Stats::SetInt("MPX_H3OPT_VEHS", _DiamondCasinoHeistVehicle.GetState());
				Stats::SetInt("MPX_CAS_HEIST_FLOW", -1);
				Stats::SetInt("MPX_H3_LAST_APPROACH", 0);
				Stats::SetInt("MPX_H3OPT_APPROACH", _DiamondCasinoHeistApproach.GetState() + 1); // 1 = Silent & Sneaky, 2 = The Big Con, 3 = Aggressive

				switch (_DiamondCasinoHeistDifficulty.GetState())
				{
				case 0: Stats::SetInt("MPX_H3_HARD_APPROACH", 0); break;
				case 1: Stats::SetInt("MPX_H3_HARD_APPROACH", _DiamondCasinoHeistApproach.GetState() + 1); break;
				default: break;
				}

				Stats::SetInt("MPX_H3OPT_TARGET", _DiamondCasinoHeistPrimaryTarget.GetState());
				Stats::SetInt("MPX_H3OPT_POI", 1023);
				Stats::SetInt("MPX_H3OPT_ACCESSPOINTS", 2047);

				switch (_DiamondCasinoHeistGunman.GetState())
				{
				case 0: Stats::SetInt("MPX_H3OPT_CREWWEAP", 4); break;
				case 1: Stats::SetInt("MPX_H3OPT_CREWWEAP", 2); break;
				case 2: Stats::SetInt("MPX_H3OPT_CREWWEAP", 5); break;
				case 3: Stats::SetInt("MPX_H3OPT_CREWWEAP", 3); break;
				case 4: Stats::SetInt("MPX_H3OPT_CREWWEAP", 1); break;
				case 5: Stats::SetInt("MPX_H3OPT_CREWWEAP", 6); break;
				default: break;
				}

				switch (_DiamondCasinoHeistDriver.GetState())
				{
				case 0: Stats::SetInt("MPX_H3OPT_CREWDRIVER", 5); break;
				case 1: Stats::SetInt("MPX_H3OPT_CREWDRIVER", 3); break;
				case 2: Stats::SetInt("MPX_H3OPT_CREWDRIVER", 2); break;
				case 3: Stats::SetInt("MPX_H3OPT_CREWDRIVER", 4); break;
				case 4: Stats::SetInt("MPX_H3OPT_CREWDRIVER", 1); break;
				case 5: Stats::SetInt("MPX_H3OPT_CREWDRIVER", 6); break;
				default: break;
				}

				Stats::SetInt("MPX_H3OPT_CREWHACKER", _DiamondCasinoHeistHacker.GetState());
				Stats::SetInt("MPX_H3OPT_DISRUPTSHIP", 3); // Security Strength: 1 = Strong, 2 = Medium, 3 = Weak
				Stats::SetInt("MPX_H3OPT_BODYARMORLVL", -1);
				Stats::SetInt("MPX_H3OPT_KEYLEVELS", 2); // 1 = Level1 Security Pass, 2 = Level2 Security Pass

				Stats::SetInt("MPX_H3OPT_BITSET0", rand() % (INT_MAX)); // Refresh board
				Stats::SetInt("MPX_H3OPT_BITSET1", rand() % (INT_MAX)); // Refresh board

				ScriptMgr::Yield(500ms);

				Stats::SetInt("MPX_H3OPT_BITSET0", -1); // Refresh board
				Stats::SetInt("MPX_H3OPT_BITSET1", -1); // Refresh board
			}
		};

		static IntCommand _DiamondCasinoHeistPotentialTake{"diamondcasinoheistpotentialtake", "潜在收益", "更新潜在收益", std::nullopt, std::nullopt, 0};

		class SetPotentialTake : public Command
		{
			using Command::Command;

			virtual void OnCall() override
			{
				int primary_target = Stats::GetInt("MPX_H3OPT_TARGET");

				switch (primary_target)
				{
				case 0:
				{
					static Tunable tunable{static_cast<joaat_t>(-1638885821)};
					if (tunable.IsReady())
						tunable.Set(_DiamondCasinoHeistPotentialTake.GetState());
					break;
				} // "Cash"
				case 1:
				{
					static Tunable tunable{static_cast<joaat_t>(-582734553)};
					if (tunable.IsReady())
						tunable.Set(_DiamondCasinoHeistPotentialTake.GetState());
					break;
				} // "Gold"
				case 2:
				{
					static Tunable tunable{1934398910};
					if (tunable.IsReady())
						tunable.Set(_DiamondCasinoHeistPotentialTake.GetState());
					break;
				} // "Artwork"
				case 3:
				{
					static Tunable tunable{1277889925};
					if (tunable.IsReady())
						tunable.Set(_DiamondCasinoHeistPotentialTake.GetState());
					break;
				} // "Diamond"
				default: break;
				}
			}
		};

		static IntCommand _DiamondCasinoHeistActualTake{"diamondcasinoheistactualtake", "实际收益", "更新实际收益", std::nullopt, std::nullopt, 0};

		class SetActualTake : public Command
		{
			using Command::Command;

			virtual void OnCall() override
			{
				if (auto thread = Scripts::FindScriptThread("fm_mission_controller"_J))
					*ScriptLocal(thread, 20395).At(2686).As<int*>() = _DiamondCasinoHeistActualTake.GetState();
			}
		};

		class SkipHacking : public Command
		{
			using Command::Command;

			virtual void OnCall() override
			{
				if (auto thread = Scripts::FindScriptThread("fm_mission_controller"_J))
				{
					*ScriptLocal(thread, 54042).As<int*>() = 5;
					*ScriptLocal(thread, 55108).As<int*>() = 5;
				}
			}
		};

		class SkipDrilling : public Command
		{
			using Command::Command;

			virtual void OnCall() override
			{
				if (auto thread = Scripts::FindScriptThread("fm_mission_controller"_J))
					*ScriptLocal(thread, 10551).At(7).As<int*>() = *ScriptLocal(thread, 10551).At(37).As<int*>();
			}
		};

		class SoloMantrap : public Command
		{
			using Command::Command;

			virtual void OnCall() override
			{
				if (auto thread = Scripts::FindScriptThread("fm_mission_controller"_J))
				{
					*ScriptLocal(thread, 32616).As<int*>() = 8;
					*ScriptLocal(thread, 63640).As<int*>() = 5;
				}
			}
		};

		class InstantFinish : public Command
		{
			using Command::Command;

			virtual void OnCall() override
			{
				if (auto thread = Scripts::FindScriptThread("fm_mission_controller"_J))
				{
					Scripts::ForceScriptHost(thread);
					ScriptMgr::Yield(500ms);

					*ScriptLocal(thread, 20395).At(1740).At(0, 1).As<int*>() = 80;
					*ScriptLocal(thread, 20395).At(2686).As<int*>() = 4443220;
					*ScriptLocal(thread, 20395).At(1062).As<int*>() = 5;
					*ScriptLocal(thread, 20395).As<int*>() = 12;
					*ScriptLocal(thread, 29016).At(0, 1).As<int*>() = 99999;
					*ScriptLocal(thread, 32472).At(0, 294).At(68).As<int*>() = 99999;
				}
			}
		};

		static SetCuts _DiamondCasinoHeistSetCuts{"diamondcasinoheistsetcuts", "设置分红", "设置抢劫分红"};
		static ForceReady _DiamondCasinoHeistForceReady{"diamondcasinoheistforceready", "强制准备", "强制所有玩家准备就绪"};
		static Setup _DiamondCasinoHeistSetup{"diamondcasinoheistsetup", "准备", "设置钻石赌场抢劫"};
		static SetPotentialTake _DiamondCasinoHeistSetPotentialTake{"diamondcasinoheistsetpotentialtake", "设置潜在收益", "更新潜在收益"};
		static SetActualTake _DiamondCasinoHeistSetActualTake{"diamondcasinoheistsetactualtake", "设置实际收益", "更新实际收益"};
		static SkipHacking _DiamondCasinoHeistSkipHacking{"diamondcasinoheistskiphacking", "跳过破解", "跳过破解过程"};
		static SkipDrilling _DiamondCasinoHeistSkipDrilling{"diamondcasinoheistskipdrilling", "跳过钻探", "跳过钻探过程"};
		static SoloMantrap _DiamondCasinoHeistSoloMantrap{"diamondcasinoheistsolomantrap", "单人通道", "跳过刷卡过程"};
		static InstantFinish _DiamondCasinoHeistInstantFinish{"diamondcasinoheistinstantfinish", "瞬间完成", "立即完成抢劫"};
	}
}