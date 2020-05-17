#include "cheats.hpp"
#include "Item.h"

void abort(void);

namespace CTRPluginFramework
{
	using StringVector = std::vector<std::string>;
	
	/*
		失敗作
	*/
	void k2h(std::string &s)
	{
		u16 moji[200] = { 0 };
		Process::WriteString( *moji, s, StringFormat::Utf16 );
		
		int i = 0;
		while ( moji[i] != '\0' )
		{
			if ( moji[i] >= 0x30a1 && moji[i] <= 0x30f3 ) moji[i] -= 0x60;
			i++;
		}
		
		Process::ReadString( *moji, s, 200, StringFormat::Utf16 );
	}
	
	
	// ---------------------------------------------------------------
	// アイテム検索
	// ～アイテム一覧の中から検索して、一致した場合はoutputに追加します。
	// ・str	: 探す文字列
	// ・output	: 追加されるベクター配列(ITEM_MAP)
	// ---------------------------------------------------------------
	void ItemSearch(const std::string &str, std::vector<ITEM_MAP> &output)
	{
		std::string tmp;
		for ( const ITEM_MAP &i: AllItems )
		{
			tmp = i.name2;
			k2h(tmp);
			
			if ( tmp.find(str) != std::string::npos )
				output.push_back(i);
		}
	}
	
	
	// ---------------------------------------------------------------
	// コマンドでチート実行
	// ---------------------------------------------------------------
	void OpenCatalog();
	void TextToCheats()
	{
		std::string com;
		
		if ( !Chat::IsOpened() ) return;
		
		if ( Controller::IsKeyDown(B) && Controller::IsKeyPressed(R) )
		{
			com = Chat::Read();
			
			if ( com == "drop on" )
			{
				Chat::Write("ドロップを有効化しました");
				Process::Write32(0x5a0e50, 0xaffff84);
			}
			else if ( com == "drop off" )
			{
				Chat::Write("ドロップを無効化しました");
				Process::Write32(0x5a0e50, 0xeaffff84);
			}
			else if ( com == "icc" )
			{
				
				volatile u32(*pfunction04)(u32 param1, u32 param2, u32 param3, u32 param4);
				Process::Write32( (u32)&pfunction04, 0x304A94 );
				pfunction04(0x5C, 1, 0, 0);
				
				
				
				
			}
			else if ( com.length() > 0 )// アイテム検索
			{
				std::vector<ITEM_MAP> Hits;
				ItemSearch(com, Hits);
				
				if ( !Hits.empty() )
				{
					if ( Hits.size() > 40 )
					{
						MessageBox("エラー", "一致するアイテム数が多すぎます。\nもう少し詳細に記述してください。")();
					}
					else
					{
						StringVector Names;
						Names.clear();
						
						for ( const ITEM_MAP &i: Hits )
							Names.push_back(i.name2);
						
						Keyboard key("", Names);
						int result = key.Open();
						
						if ( result >= 0 )
						{
							OSD::SwapBuffers();
							Sleep(Seconds(0.05));
							
							Player p;
							p.SetItem(Hits[result].code, 0);
							
							MessageBox("スロット1: " + Names[result])();
						}
					}
				}
			}
			
			
			
			
		}
	}
	
	
	void OpenCatalog()
	{
		
		
		
	}
	
	
}






