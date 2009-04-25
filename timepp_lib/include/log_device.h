#pragma once

#include "log.h"
#include "tstring.h"
#include <windows.h>
#include <stdio.h>
#include <string>

namespace tp
{

class ld_console : public log_device
{
public:
	ld_console()
		: m_free_console_on_close(false)
		, m_handle(NULL)
	{
	}

	virtual bool open()
	{
		if (::AllocConsole())
		{
			m_free_console_on_close = true;
		}
		m_handle = ::GetStdHandle(STD_OUTPUT_HANDLE);
		return is_handle_valid(m_handle);
	}
	virtual bool close()
	{
		if (m_free_console_on_close)
		{
			::FreeConsole();
			m_handle = NULL;
		}
		return true;
	}
	virtual bool flush()
	{
		return true;
	}

	virtual size_t write(const char * buf, size_t len, text_class tc)
	{
		if (!is_handle_valid(m_handle)) return 0;

		set_text_attr(m_handle, tc);
		DWORD writed;
		::WriteConsoleA(m_handle, buf, static_cast<DWORD>(len), &writed, NULL);
		return static_cast<size_t>(writed);
	}
	virtual size_t write(const wchar_t * buf, size_t len, text_class tc)
	{
		if (!is_handle_valid(m_handle)) return 0;

		set_text_attr(m_handle, tc);
		DWORD writed;
		::WriteConsoleW(m_handle, buf, static_cast<DWORD>(len), &writed, NULL);
		return static_cast<size_t>(writed);
	}

protected:
	HANDLE m_handle;
	bool m_free_console_on_close;

private:
	static void set_text_attr(HANDLE handle, text_class tc)
	{
		WORD attr = 0;
		switch (tc)
		{
		case tc_normal: 
		case tc_blank:  attr = FOREGROUND_RED | FOREGROUND_BLUE | FOREGROUND_GREEN; break;
		case tc_header: attr = FOREGROUND_GREEN | FOREGROUND_INTENSITY; break;
		case tc_type:   attr = FOREGROUND_RED | FOREGROUND_INTENSITY; break;
		case tc_thread: attr = FOREGROUND_BLUE | FOREGROUND_INTENSITY; break;
		case tc_other: break;
		}

		::SetConsoleTextAttribute(handle, attr);
	}
	static bool is_handle_valid(HANDLE handle)
	{
		return (handle != NULL && handle != INVALID_HANDLE_VALUE);
	}
};

class ld_mem_log : public log_device
{
public:
	ld_mem_log()
	{
		m_log.reserve(1000 * 1000);
	}

	virtual bool open() { return true; }
	virtual bool close() { return true; }
	virtual size_t write(const wchar_t * buf, size_t len, text_class)
	{
		m_log += std::wstring(buf, len);
		return len;
	}
	virtual size_t write(const char *, size_t, text_class)
	{
		// 当前暂时不支持
		return 0;
	}
	virtual bool flush()
	{
		return true;
	}

	void get_log(std::wstring& str)
	{
		str = m_log;
	}

private:
	std::wstring m_log;
};

class ld_debug_output : public log_device
{
public:
	ld_debug_output()
	{
	}

	virtual bool open()
	{
		return true;
	}

	virtual bool close()
	{
		return true;
	}

	virtual size_t write(const wchar_t * buf, size_t len, text_class)
	{
		std::wstring str(buf, len);
		::OutputDebugStringW(str.c_str());
		return len;
	}

	virtual size_t write(const char *buf, size_t len, text_class)
	{
		std::string str(buf, len);
		::OutputDebugStringA(str.c_str());
		return len;
	}

	virtual bool flush()
	{
		return true;
	}
};

class ld_file : public log_device
{
public:
	ld_file(const wchar_t * filename)
	{
		m_filename = filename;
		m_fp = NULL;
	}
	
	virtual bool open()
	{
		errno_t ret = _wfopen_s(&m_fp, m_filename.c_str(), L"a");
		return (ret == 0);
	}

	virtual bool close()
	{
		fclose(m_fp);
		m_fp = NULL;
		return true;
	}

	virtual size_t write(const char *buf, size_t len, text_class)
	{
		if (m_fp) return static_cast<size_t>(fprintf(m_fp, "%.*s", len, buf));
		return 0;
	}

	virtual size_t write(const wchar_t *buf, size_t len, text_class)
	{
		if (m_fp) return static_cast<size_t>(fwprintf(m_fp, L"%.*s", len, buf));
		return 0;
	}

protected:
	FILE * m_fp;
	std::wstring m_filename;
};
/*
class ld_xml_file : public ld_file
{
};
*/

} // namespace tp