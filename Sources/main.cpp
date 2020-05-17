#include "cheats.hpp"
#include "csvc.h"

void abort(void);

namespace CTRPluginFramework
{
	void PatchProcess(FwkSettings &settings)
	{
		settings.AllowSearchEngine = false;
	}
	
	void InitEntrys(PluginMenu &);
	
	int main(void)
	{
		Boot();
		
		PluginMenu* menu = new PluginMenu(Color::ForestGreen << "JOKER  4.00");
		menu->SynchronizeWithFrame(true);
		
		InitEntrys(*menu);
		
		menu->Run();

		delete menu;
		return 0;
	}
}




