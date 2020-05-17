#pragma once

namespace Zstd
{
	class StringUtil
	{
	public:
		static int lenUtf8(std::string &str);
		static int lenByte(unsigned char c);
	};
	
	class Chat
	{
	public:
		static bool IsOpened();
		
		static void Write(std::string);
		static void Read(std::string &);
		static std::string Read();
		
		static void Send();
		static void Clear();
		
	private:
		static u32 _get_text_buffer_addr();
		static bool _isTown();
		static void _write_length(u8);
		static u8 _read_length();
		static u32 len_addr();
		
	};
	
	
}


