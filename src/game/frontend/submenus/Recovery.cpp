#include "Recovery.hpp"
#include "Recovery/HeistModifier.hpp"
#include "Recovery/StatEditor.hpp"
#include "Recovery/Transactions.hpp"
#include "Recovery/DailyActivities.hpp"
#include "game/frontend/items/Items.hpp"

namespace YimMenu::Submenus
{
	Recovery::Recovery() :
		#define ICON_FA_SACK_DOLLAR "\xef\x93\x80"
	    Submenu::Submenu("刷钱", ICON_FA_SACK_DOLLAR)
	{
		auto main = std::make_shared<Category>("主要");
		auto businesses = std::make_shared<Category>("产业");
		auto casino = std::make_shared<Category>("赌场");

		auto generalGroup = std::make_shared<Group>("通用");
		auto businessGroup = std::make_shared<Group>("通用");
		auto casinoSlots = std::make_shared<Group>("老虎机");
		//auto casinoWheel = std::make_shared<Group>("Lucky Wheel");
		//auto casinoBlackJack = std::make_shared<Group>("Blackjack");
		//auto casinoRoulette = std::make_shared<Group>("Roulette");

		generalGroup->AddItem(std::make_shared<BoolCommandItem>("playallmissionssolo"_J));
		generalGroup->AddItem(std::make_shared<BoolCommandItem>("unlockgtaplus"_J));
		generalGroup->AddItem(std::make_shared<BoolCommandItem>("overriderpmultiplier"_J));
		generalGroup->AddItem(std::make_shared<ConditionalItem>("overriderpmultiplier"_J, std::make_shared<FloatCommandItem>("rpmultiplierinput"_J)));
		generalGroup->AddItem(std::make_shared<BoolCommandItem>("freechangeappearance"_J));
		generalGroup->AddItem(std::make_shared<BoolCommandItem>("nochangeappearancecooldown"_J));
		generalGroup->AddItem(std::make_shared<BoolCommandItem>("allowgenderchange"_J));

		businessGroup->AddItem(std::make_shared<ListCommandItem>("businesssafe"_J));
		businessGroup->AddItem(std::make_shared<CommandItem>("claimsafeearnings"_J));

		casinoSlots->AddItem(std::make_shared<BoolCommandItem>("casinomanipulaterigslotmachines"_J));
		

		main->AddItem(generalGroup);
		businesses->AddItem(businessGroup);
		casino->AddItem(casinoSlots);

		AddCategory(std::move(main));
		AddCategory(std::move(businesses));
		AddCategory(std::move(casino));
		AddCategory(BuildStatEditorMenu());
		AddCategory(BuildTransactionsMenu());
		AddCategory(BuildHeistModifierMenu());
		AddCategory(BuildDailyActivitiesMenu());
	}
}