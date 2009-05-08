#include "compilerconf.h"
#include <tplib.h>

using namespace std;

void main(int argc, char *[])
{
	setlocale(LC_ALL, "");

	AUTO_RELEASE(L"测试自动释放宏，此日志应该在程序退出时输出\n", wprintf);
	
	// 配合其它模块使用
	tp::log_add_device(new tp::ld_console, 0xFF, L"%H:%M:%S");
	tp::log_add_device(new tp::ld_file(L"timepp_lib.log"), 0xFF, L"%Y-%m-%d %H:%M:%S");
	tp::log_set_type_names(L"IVDE");

	tp::log(1, tp::cz(L"program started with %d args.", argc - 1));
	
	tp::log(L"\ncheck main");
	{
		tp::log_indenter li(4);
		unsigned char tmp[] = "ABCDE\xcc\xdd\xee\x00""acc";
		tp::log(2, tp::cz(L"the content of array(%d bytes)\n%s", sizeof(tmp), &tp::hex_dump(tmp, sizeof(tmp))));
	}

}
