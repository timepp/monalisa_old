#include "compilerconf.h"
#include <log.h>
#include <log_device.h>
//#include "cfmt.h"
#include <stdio.h>
#include <auto_release.h>
#include <format_shim.h>
#include <sys_error.h>
#include <base64_codec.h>
#include <iostream>
#include <algorithm>
#include <cstddef>
#include <string>
#include <helper.h>

using namespace std;

#pragma message(__LOC__ "可以跳转的编译提示举例")

void main(int argc, char *[])
{
	setlocale(LC_ALL, "");
	AUTO_RELEASE("main exited\n", printf);
	
	// 配合其它模块使用
	tp::log_add_device(new tp::ld_console, 0xFF, "%H:%M:%S");
	tp::log_add_device(new tp::ld_file(L"timepp_lib.log"), 0xFF, "%Y-%m-%d %H:%M:%S");
	tp::log_set_type_names("IVDE");

	tp::log(1, tp::cz(L"program started with %d args.", argc - 1));
	
	tp::log("\ncheck main");
	{
		tp::log_indenter li(4);
		unsigned char tmp[] = "ABCDE\xcc\xdd\xee\x00""acc";
		tp::log(2, tp::cfmt<char, 10240>("the content of array(%d bytes)\n%s", sizeof(tmp), &tp::hex_dumpA(tmp, sizeof(tmp))));
		tp::log(2, tp::cfmt<wchar_t, 10240>(L"the content of array(%d bytes)\n%s", sizeof(tmp), &tp::hex_dump(tmp, sizeof(tmp))));
	}

	tp::log(3, base64_codec::encode_a(&main, 200).c_str());

}
