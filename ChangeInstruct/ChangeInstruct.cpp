#include <iostream>
#include <windows.h>
#include <fstream>
int main(void)
{
	char buffer_prologue[77] = {};
	char buffer_xored_prologue[77] = {};
	char key = '\x02'; // Ключ для ксора
	const char* buffer_jump_xor = ("\xEB\x69\xEB\x6B\xEB\xAD"); // Джамп через зашифрованный пролог
	const char* buffer_jump_prologue = ("\xEB\x4D"); // Джамп на расшифровшик
	const char* buffer_nop_porlogue = ("\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90");
	const char* buffer_xor = ("\xEB\x25\xEB\x95\x4C\x8D\x0D\x90\xFF\xFF\xFF\x4C\x89\xCE\x4C\x8D\x15\x33\xFF\xFF\xFF\x4C\x89\xD7\xB9\x4D\x00\x00\x00\xAC\x34\x02\xAA\xFF\xC9\x75\xF8\xEB\xDB");
	const char* buffer_xoredd_prologue = ("\x42\x57\x55\x4A\x83\xEE\xEA\x02\x02\x02\x4A\x8F\x6E\x26\x22\x4A\x89\xFE\xBB\x38\x02\x02\x02\xBA\xCE\xCE\xCE\xCE\xF1\xA9\x4A\x8F\x0F\x63\xED\xFD\xFD\xEA\x0C\xD1\xFC\xFD\x4A\x8F\x17\x45\x69\xFD\xFD\x4A\x89\xca\xfd\x17\x61\xd2\xfd\xfd");
	const char* buffer_selfdestruct = ("\x4C\x8D\x0D\x1A\x00\x00\x00\x4C\x89\xCE\x4C\x8D\x15\x10\xFF\xFF\xFF\x4C\x89\xD7\xB9\x4D\x00\x00\x00\xAC\xAA\xFF\xC9\x75\xFA\xEB\x7F");
	std::fstream fs;
	fs.open("Test_ASM.exe", std::fstream::in | std::fstream::out | std::ios::binary | std::ios::beg);

	/* Адреса */
	int addr_start_func = 0xFCB0; // Адресс начала функции
	int addr_xor_func = 0xFD03; // Адресс начана зашифрованного пролога
	int addr_jump_xor = 0xFCFD; // Адресс для  Джамп через зашифрованный пролог
	int addr_xor = 0xFD68; // Адресс функции расшифровки
	int addr_jump_unxor = 0xFD68; // Джампы после зашифрованного пролога
	int addr_selfdestruct = 0xFD8F; // Адресс сегмента кода, котоырй отвечает за уничтожение расшифрованного кода
	int addr_selfdestruck_jump = 0xFDB0; // Адресс для восстановления jump
	
	/* Чтение пролога функции */
	fs.seekg(addr_start_func, fs.beg);
	fs.read(buffer_prologue, sizeof(buffer_prologue));

	/* Ксор пролога функции */
	for (int x = 0; x < sizeof(buffer_prologue); x++)
	{
		buffer_xored_prologue[x] = buffer_prologue[x] ^ key;
	};

	/* Вставка зашифрованного пролога*/
	fs.seekp(addr_xor_func, fs.beg);
	fs.write(buffer_xored_prologue, sizeof(buffer_xored_prologue));
	
	/* Джамп через зашифрованный пролог*/
	fs.seekp(addr_jump_xor, fs.beg);
	fs.write((static_cast<const char*>(buffer_jump_xor)), 6);

	/* Обнуление пролога */
	fs.seekp(addr_start_func, fs.beg);
	fs.write(buffer_nop_porlogue, 77);

	/* Джамп в начало функции */
	fs.seekp(addr_start_func, fs.beg);
	fs.write((static_cast<const char*>(buffer_jump_prologue)), 2);

	/* Вставка расшишифровки */
	fs.seekp(addr_xor, fs.beg);
	fs.write(buffer_xor, 39);

	/* Джамп в сегмент */
	fs.seekp(addr_selfdestruck_jump, fs.beg);
	fs.write((static_cast<const char*>(buffer_jump_prologue)), 2);

	/* Вставка  selfdestruct */
	fs.seekp(addr_selfdestruct, fs.beg);
	fs.write(buffer_selfdestruct, 33);

	/*Вывод буфера зашифрованного пролога и конст чара зашифрованного пролога */
	std::cout << "Xor\tMyXor" << std::endl;
	for (int i = 0; i < 77; i++)
	{
		std::cout << std::hex << (0xFF & static_cast<short>(buffer_xored_prologue[i])) << '\t' 
			<< std::hex << (0xFF & static_cast<short>(buffer_prologue[i])) << std::endl;
	}
	fs.close();
	std::system("pause"); 
}
