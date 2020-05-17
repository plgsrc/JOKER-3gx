#include "cheats.hpp"

namespace CTRPluginFramework
{
	u16 ItemID = 0x2159;
	u16 DropID = 0x10a;

	// ---------------------------------------------------------------
	// ドロップ関数
	// 成功するとtrueを返し、失敗すればfalseを返します。
	// ---------------------------------------------------------------
	bool Drop(u16 item, u16 dcode, u8 x, u8 y, bool check)
	{
		if ( check && Game::GetWorldItem(x, y) != 0x7ffe ) return false;
		
		Process::Write32(0x9AE160, item);
		Process::Write32(0x9AE164, item);
		
		u16 location = y * 0x100 + x;
		u16 IsDropping;
		u32 DropAddress;
		u32 ItemAddress;
		
		for ( int i = 0; i < 32; i++ )
		{
			IsDropping = READU16(0x9ae254 + i * 0x16);
			DropAddress = 0x9ae252 + i * 0x16;
			if ( READU16(DropAddress) == location && IsDropping != 0 ) return false;
		}
		
		for ( int i = 0; i < 32; i++ )
		{
			IsDropping = READU16(0x9ae254 + i * 0x16);
			ItemAddress = 0x9ae248 + i * 0x16;
			DropAddress = 0x9ae252 + i * 0x16;
			if ( IsDropping == 0 )
			{
				Process::Write32(ItemAddress, item);
				Process::Write32(ItemAddress + 0x4, item);
				Process::Write32(DropAddress, dcode * 0x10000 + location);
				return true;
			}
		}
		
		return false;
	}

	// ドロップ無効化
	void DropStopper(MenuEntry* e) {
		if (Controller::IsKeysPressed(e->Hotkeys[0].GetKeys()))
			Process::Write32(0x5a0e50, 0xeaffff84);

		if (Controller::IsKeysPressed(e->Hotkeys[1].GetKeys()))
			Process::Write32(0x5a0e50, 0xaffff84);
	}

	// 連続スライドドロップ
	void DD_Spam(MenuEntry* e) {
		static bool mode;

		u32* infItemAddr1 = (u32*)0x19C4D0;
		u32* infItemAddr2 = (u32*)0x19C42C;
		const u32 nop = 0xE1A00000;

		if (*infItemAddr1 != 0)
		{
			*infItemAddr1 = 0;
			*infItemAddr2 = 0;
		}

		if (Controller::IsKeysPressed(e->Hotkeys[0].GetKeys()))
		{
			if (!mode)
			{
				mode = true;
				OSD::Notify("DragDropSpam: ON");
				Process::Write32(0x19c548, nop);
				Process::Write32(0x19dde4, nop);
				Process::Write32(0x26f000, nop);
			}
			else
			{
				mode = false;
				OSD::Notify("DragDropSpam: OFF");
				Process::Write32(0x19c548, 0xeb03fb85);
				Process::Write32(0x19dde4, 0xeb03f55e);
				Process::Write32(0x26f000, 0xeb00b0d7);
			}
		}
	}

	// アイテム変更
	void ChangeItemID(MenuEntry* e) {
		if (e->Hotkeys[0].IsDown())
		{
			Keyboard key("アイテムID");
			key.Open(ItemID);
		}
	}

	// ドロップ変更
	void ChangeDropID(MenuEntry* e) {
		if (e->Hotkeys[0].IsDown())
		{
			Keyboard key("", { "通常", "花を植える", "マイデザイン", "埋める" });
			u16 DropCodes[4] = { 0x10a, 0x10c, 0x10d, 0x10b };

			int r0 = key.Open();
			if (r0 >= 0) DropID = DropCodes[r0];
		}
	}

	// タッチドロップ
	void TouchDrop(MenuEntry* e) {
		touchPosition tp;
		hidTouchRead(&tp);

		int x = tp.px;
		int y = tp.py;

		if (x < 70 || y < 54) return;
		if (x > 249 || y > 206) return;
		if (Chat::IsOpened()) return;

		x -= 70;
		x *= 13.7;
		x /= 32;

		y -= 54;
		y *= 14.8;
		y /= 32;

		if (Controller::IsKeysDown(Touchpad))
		{
			Drop(ItemID, DropID, x + 0x10, y + 0x10, true);
		}
	}

	// 自動ドロップ
	void AutoDrop(MenuEntry* e) {
		static bool mode;

		u8 sx = *(u8*)0x3309a2b8;
		u8 sy = *(u8*)0x3309a2bc;

		sx -= 1;
		sy -= 1;

		if (Controller::IsKeysPressed(e->Hotkeys[0].GetKeys())) {
			if (!mode) {
				mode = true;
				OSD::Notify("AutoDrop: ON");
			}
			else {
				mode = false;
				OSD::Notify("AutoDrop: OFF");
			}
		}

		if (mode) {
			for (int x = 0; x < 3; x++) {
				for (int y = 0; y < 3; y++) {
					u32 px = sx + x;
					u32 py = sy + y;
					if (px >= 0x10 && px <= 0x60 && py >= 0x10 && py <= 0x60)
						Drop(ItemID, DropID, px, py, true);
				}
			}
		}
	}



}

