#include "compilerconf.h"
#include <tplib.h>

using namespace std;

int wmain(int argc, wchar_t *[])
{
	setlocale(LC_ALL, "");

	ON_LEAVE(tp::log(L"�����Զ��ͷź꣬����־Ӧ���ڳ����˳�ʱ���\n"));
	
	// �������ģ��ʹ��
	tp::sc::log_default_console_config();
//	tp::log_add_device(new tp::ld_file(L"timepp_lib.log"), 0xFF, L"%Y-%m-%d %H:%M:%S");
//	tp::log_set_type_names(L"IVDE");

	tp::log(1, tp::cz(L"program started with %d args.", argc - 1));
	
	tp::log(L"\ncheck main");
	{
//		tp::log_indenter li(4);
		unsigned char tmp[] = "ABCDE\xcc\xdd\xee\x00""acc";
		tp::log(2, tp::cz(L"the content of array(%d bytes)\n%s", sizeof(tmp), &tp::hex_dump(tmp, sizeof(tmp))));
	}

	return 0;
}
