#include "compilerconf.h"
#include <tplib.h>
#include <cmdlineparser.h>

#include <conio.h>
#include <vector>

using namespace std;

int wmain(int argc, wchar_t *argv[])
{
	setlocale(LC_ALL, "");
	tp::sc::log_default_console_config();

	ON_LEAVE(tp::log(L"测试自动释放宏，此日志应该在程序退出时输出\n"));

	tp::cmdline_parser parser;
	parser.parse(argc, argv);
	tp::log(tp::cz(L"命令行:%s", GetCommandLineW()));
	for (int i = 0; i < argc; i++)
	{
		tp::log_indenter li(2);
		tp::log(tp::cz(L"%02d %s", i, argv[i]));
	}

	tp::log(L"check main");
	{
//		tp::log_indenter li(4);
		unsigned char tmp[] = "ABCDE\xcc\xdd\xee\x00""acc";
		tp::log(2, tp::cz(L"the content of array(%d bytes)\n%s", sizeof(tmp), &tp::hex_dump(tmp, sizeof(tmp))));
	}


	_getch();
	return 0;
}
