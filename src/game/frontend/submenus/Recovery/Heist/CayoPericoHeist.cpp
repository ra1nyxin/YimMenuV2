#include "CayoPericoHeist.hpp"

namespace YimMenu::Submenus
{
	std::shared_ptr<TabItem> RenderCayoPericoHeistMenu()
	{
		auto tab = std::make_shared<TabItem>("Cayo Perico Heist");

		auto cuts = std::make_shared<Group>("分红", 2);
		auto setups = std::make_shared<Group>("准备任务");
		auto loots = std::make_shared<Group>("战利品", 2);
		auto misc = std::make_shared<Group>("其他", 1);

		cuts->AddItem(std::make_shared<IntCommandItem>("cayopericoheistcut1"_J));
		cuts->AddItem(std::make_shared<IntCommandItem>("cayopericoheistcut3"_J));
		cuts->AddItem(std::make_shared<IntCommandItem>("cayopericoheistcut2"_J));
		cuts->AddItem(std::make_shared<IntCommandItem>("cayopericoheistcut4"_J));
		cuts->AddItem(std::make_shared<CommandItem>("cayopericoheistforceready"_J));
		cuts->AddItem(std::make_shared<CommandItem>("cayopericoheistsetcuts"_J));

		setups->AddItem(std::make_shared<ListCommandItem>("cayopericoheistdifficulty"_J));
		setups->AddItem(std::make_shared<ListCommandItem>("cayopericoheistprimarytarget"_J));
		setups->AddItem(std::make_shared<ListCommandItem>("cayopericoheistweapon"_J));
		setups->AddItem(std::make_shared<CommandItem>("cayopericoheistsetup"_J));

		loots->AddItem(std::make_shared<IntCommandItem>("cayopericoheistprimarytargetvalue"_J));
		loots->AddItem(std::make_shared<IntCommandItem>("cayopericoheistsecondarytakevalue"_J));
		loots->AddItem(std::make_shared<CommandItem>("cayopericoheistsetprimarytargetvalue"_J, "Set##primarytargetvalue"));
		loots->AddItem(std::make_shared<CommandItem>("cayopericoheistsetsecondarytakevalue"_J, "Set##secondarytakevalue"));

		misc->AddItem(std::make_shared<CommandItem>("cayopericoheistskiphacking"_J));
		misc->AddItem(std::make_shared<CommandItem>("cayopericoheistcutsewer"_J));
		misc->AddItem(std::make_shared<CommandItem>("cayopericoheistcutglass"_J));
		misc->AddItem(std::make_shared<CommandItem>("cayopericoheisttakeprimarytarget"_J));
		misc->AddItem(std::make_shared<CommandItem>("cayopericoheistinstantfinish"_J));

		tab->AddItem(cuts);
		tab->AddItem(setups);
		tab->AddItem(loots);
		tab->AddItem(misc);

		return tab;
	}
}