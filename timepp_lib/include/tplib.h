#pragma once

#include "format_shim.h"
#include "log.h"
#include "log_device.h"
#include "auto_release.h"

namespace tp
{
	namespace sc
	{
		inline void log_default_config()
		{
			tp::log_set_type_names(L"IVDE");
			tp::log_add_device(new tp::ld_console, 0xFF, L"%H:%M:%S");
		}
		inline void log_win_error(const wchar_t * prefix)
		{
			tp::log(tp::cz(L"%s: %s", prefix, &tp::edwin()));
		}
	}
}
