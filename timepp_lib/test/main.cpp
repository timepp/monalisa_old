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
	tp::log_set_type_names("IVDE");

	tp::log(1, tp::cz(L"program started with %d args.", argc - 1));
	
	tp::log("\ncheck main");
	{
		tp::log_indenter li(4);
		tp::log(2, tp::cfmt<char, 10240>("the content of main(the first %d bytes)\n%s", 200, &tp::hex_dumpA(&main, 200)));
	}

	tp::log(3, base64_codec::encode_a(&main, 200).c_str());

}
