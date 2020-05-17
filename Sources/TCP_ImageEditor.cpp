// ---------------------------------------------------------------
// ソパカ写真編集
// 2020-3-16 Mr.zkR
// ---------------------------------------------------------------

#include "Helpers.hpp"
#include "cheats.hpp"

#define IMAGE_WIDTH		80
#define IMAGE_HEIGHT	118

namespace CTRPluginFramework
{
	Color Image[IMAGE_WIDTH][IMAGE_HEIGHT];
	
	bool Draw(const Screen &);
	void Line(int, int, int, int, Color);
	Point TchXY[2];
	
	void TCP_ImageEditor(MenuEntry *e)
	{
		if ( !e->Hotkeys[0].IsDown() ) return;
		
		if ( *(u16*)(0x95133a) == 0xa55c )
		{
			bool BottomUI  = *(u32*)(0x949d1f) == 0x6 || *(u32*)(0x949d1f) == 0x8a;
			bool ScreenTWQ = *(u16*)(0x946be8) == 0x6d30;
			if ( !BottomUI || !ScreenTWQ )
			{
				(MessageBox("カウント中のみ使えます。"))();
				return;
			}
		}
		else
		{
			if ( (MessageBox("ここは写真機ではありません。\nワープしますか？"))() )
			{
				volatile u32(*pfunction04)(u32 param1, u32 param2, u32 param3, u32 param4);
				Process::Write32( (u32)&pfunction04, 0x304A94 );
				pfunction04(0x5C, 1, 1, 0);
			}
			return;
		}
		
		
		bool DrawPicture = true;
		
		std::vector<std::string> options;
		Color dc = Color::White;
		int dx = 0, dy = 0;
		int x, y;
		
		Process::Pause();
		
		const Screen &top = OSD::GetTopScreen();
		const Screen &bottom = OSD::GetBottomScreen();
		
		// 上画面の写真部分をImage配列に保存
		for ( int sy = 0; sy < IMAGE_HEIGHT; sy++ )
			for ( int sx = 0; sx < IMAGE_WIDTH; sx++ )
			{
				top.ReadPixel(160 + sx, 56 + sy, Image[sx][sy]);
			}
		
		
		while ( true )
		{
			// HID更新
			Controller::Update();
			
			options.clear();
			options =
			{
				"<A / Touch>",
				"  Draw pixel to picture",
				"",
				"<Y> OK",
				"<B> Exit",
				"<X> Change pixel color",
				"<DPad> Move draw point",
				"",
				Utils::Format("Draw Point: %d, %d", dx, dy)
			};
			
			// 背景
			top.DrawRect(0, 0, 400, 240, Color::Black);
			bottom.DrawRect(0, 0, 320, 240, Color::Green);
			//bottom.DrawRect(183, 16, 128, 208, Color::Black);
			
			// 説明文章
			int i = 0;
			for ( const std::string &s: options )
			{
				bottom.Draw(s, 4, 8 + i * 10, Color::Black, Color::Green);
				i++;
			}
			
			// 画像描画
			bottom.DrawPixel( 158 + dx * 2, 2 + dy * 2, Color::Red );
			for ( int sy = 0; sy < IMAGE_HEIGHT; sy++ )
				for ( int sx = 0; sx < IMAGE_WIDTH; sx++ )
				{
					bottom.DrawRect( 158 + sx * 2, 2 + sy * 2, 2, 2, Image[sx][sy] );
				}
			
			// exit
			if ( Controller::IsKeyPressed(B) )
			{
				if ( (MessageBox("編集を破棄しますか？", DialogType::DialogYesNo, ClearScreen::Top))() == true )
				{
					DrawPicture = false;
					break;
				}
			}
			
			// save & exit
			if ( Controller::IsKeyPressed(Y) )
			{
				if ( (MessageBox("編集した内容を適用します。\nよろしいですか？", DialogType::DialogYesNo, ClearScreen::Top))() == true ) break;
			}
			
			// change color
			if ( Controller::IsKeyPressed(X) )
			{ 
				Keyboard k;
				u32 c = dc.ToU32();
				k.Open(c, c);
				dc = Color(c);
			}
			
			// move draw point
			if ( Controller::IsKeyDown(DPad) )
			{
				if ( Controller::IsKeyDown(DPadUp) ) dy--;
				if ( Controller::IsKeyDown(DPadDown) ) dy++;
				if ( Controller::IsKeyDown(DPadLeft) ) dx--;
				if ( Controller::IsKeyDown(DPadRight) ) dx++;
				if ( dx < 0 ) dx = 0;
				if ( dy < 0 ) dy = 0;
				if ( dx > IMAGE_WIDTH - 1 ) dx = IMAGE_WIDTH - 1;
				if ( dy > IMAGE_HEIGHT - 1 ) dy = IMAGE_HEIGHT - 1;
				Sleep(Milliseconds(50));
			}
			
			
			// Aでピクセル描画
			if ( Controller::IsKeyDown(A) )
			{
				Image[dx][dy] = dc;
			}
			
			// タッチ
			if ( Controller::IsKeyPressed(Touchpad) )
			{
				x = Touch::GetPosition().x;
				y = Touch::GetPosition().y;
				if ( x >= 158 && y >= 2 && x <= (158 + IMAGE_WIDTH * 2) && y <= (2 + IMAGE_HEIGHT * 2) )
				{
					x -= 158;
					y -= 2;
					x /= 2;
					y /= 2;
					for ( int i = 0; i < 2; i++ )
					{
						TchXY[i].x = x;
						TchXY[i].y = y;
					}
				}
			}
			else if ( Controller::IsKeyDown(Touchpad) )
			{
				x = Touch::GetPosition().x;
				y = Touch::GetPosition().y;
				if ( x >= 158 && y >= 2 && x <= (158 + IMAGE_WIDTH * 2) && y <= (2 + IMAGE_HEIGHT * 2) )
				{
					x -= 158;
					y -= 2;
					x /= 2;
					y /= 2;
					dx = x;
					dy = y;
					
					TchXY[1] = TchXY[0];
					TchXY[0].x = x;
					TchXY[0].y = y;
					
					Line(TchXY[0].x, TchXY[0].y, TchXY[1].x, TchXY[1].y, dc);
					//Image[y * IMAGE_WIDTH + x] = dc;
				}
			}
			
			
			OSD::SwapBuffers();
			//Sleep(Milliseconds(1));
		}
		
		Process::Play();
		
		if ( !DrawPicture )
		{
			return;
		}
		
		OSD::Run(Draw);
		while ( *(u16*)(0x946be8) != 0x9cc0 )
		{
			Sleep(Seconds(0.1));
		}
		OSD::Stop(Draw);
		
		
		
	}

	bool Draw(const Screen &scr)
	{
		if ( !scr.IsTop ) return false;
		
		for ( int sy = 0; sy < IMAGE_HEIGHT; sy++ )
			for ( int sx = 0; sx < IMAGE_WIDTH; sx++ )
				scr.DrawPixel(160 + sx, 56 + sy, Image[sx][sy]);
		
		scr.Draw("Please wait...", 398 - OSD::GetTextWidth(false, "DOODLE DoooOOO"), 220);
		
		return true;
	}
	
	void Line(int sx, int sy, int dx, int dy, Color c)
	{
		int x = sx, y = sy;
		int wx = (sx >= dx ? sx - dx : dx - sx);
		int wy = (sy >= dy ? sy - dy : dy - sy);
		bool xmode = (wx >= wy);
		int gosa = 0;
		
		while ( x != dx || y != dy )
		{
			Image[x][y] = c;
			if ( xmode )
			{
				if ( sx < dx )
					x++;
				else
					x--;
				
				gosa += ((dy - sy) << 1);
				if ( gosa > wx )
				{
					y++;
					gosa -= (wx << 1);
				}
				else if ( gosa < -wx )
				{
					y--;
					gosa += (wx << 1);
				}
			}
			else
			{
				if ( sy < dy )
					y++;
				else
					y--;
				
				gosa += ((dx - sx) << 1);
				if ( gosa > wy )
				{
					x++;
					gosa -= (wy << 1);
				}
				else if ( gosa < -wy )
				{
					x--;
					gosa += (wy << 1);
				}
			}
		}
		
		
		
	}
	
	
	
	
}



