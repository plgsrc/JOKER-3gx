#include "3ds.h"
#include "CTRPluginFramework.hpp"
#include "Zstd/Chat.hpp"

namespace Zstd
{
	using namespace CTRPluginFramework;
	
	int StringUtil::lenUtf8(std::string &str)
	{
		auto _pstr = str.c_str();
		
		int i = 0;
		int len = 0;
		
		while (_pstr[i] != '\0')
		{
			len++;
			i += lenByte(_pstr[i]);
		}
		
		return len;
	}
	
	int StringUtil::lenByte(unsigned char c)
	{
		if ((c >= 0x00) && (c <= 0x7f))
			return 1;
		else if ((c >= 0xc2) && (c <= 0xdf))
			return 2;
		else if ((c >= 0xe0) && (c <= 0xef))
			return 3;
		else if ((c >= 0xe0) && (c <= 0xef))
			return 4;
		
		return 0;
	}
	
	bool Chat::IsOpened()
	{
		if ( *(u32*)0x958114 == 0 ) return false;
		return true;
	}
	
	void Chat::Write(std::string str)
	{
		if ( !IsOpened() ) return;
		
		Clear();
		_write_length(StringUtil::lenUtf8(str));
		Process::WriteString( _get_text_buffer_addr(), str, StringFormat::Utf16 );
	}
	
	void Chat::Read(std::string &output)
	{
		if ( IsOpened() )
			Process::ReadString( _get_text_buffer_addr(), output, 33, StringFormat::Utf16 );
	}
	
	std::string Chat::Read()
	{
		if ( !IsOpened() ) return std::string("");
		
		std::string str;
		Process::ReadString(_get_text_buffer_addr(), str, 33, StringFormat::Utf16);
		
		return str;
	}
	
	void Chat::Send()
	{
		if ( IsOpened() ) Process::Write32(0x9580e1, 0x1000001);
	}
	
	void Chat::Clear()
	{
		if ( !IsOpened() ) return;
		
		u32 *addr = (u32*)( _get_text_buffer_addr() );
		for (int i = 0; i < 16; i++)
			addr[i] = 0;
	}
	
	u32 Chat::_get_text_buffer_addr()
	{
		return *(u32*)(0x958108);
	}
	
	bool Chat::_isTown()	
	{
		if ( READU32(0x32E2A080) == 0xCBBCBABB )
			return false;
		
		return true;
	}
	
	void Chat::_write_length(u8 len)
	{
		*(u8*)((*(u32*)0x958114) + 0x8) = len;
	}
	
	u8 Chat::_read_length()
	{
		return *(u8*)((*(u32*)0x958114) + 0x8);
	}
	
	u32 Chat::len_addr()
	{
		return (*(u32*)0x958114) + 0x8;
	}
	
	
}


