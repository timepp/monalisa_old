#pragma once

#include "format_shim_base.h"
#include "api_wrapper.h"
#include <stdio.h>
#include <stdarg.h>

namespace tp
{

// cfmt ��printf�﷨��ʽ���ַ���
template <typename T, size_t buf_size = 1024>
class cfmt : public format_shim<T, buf_size>
{
public:
	explicit cfmt(const T * fmt, ...)
	{
		va_list args;
		va_start(args, fmt);

		resize(static_cast<size_t>(aw::_vscprintf(fmt, args) + 1));
		aw::vsnprintf_s(m_buf, m_buf_size, fmt, args);
	}
};

// hex_dumper ���ڴ�����dump�ɿɶ��汾
template <typename T, size_t buf_size = 1024>
class hex_dumper : public format_shim<T, buf_size>
{
public:
	hex_dumper(const void * data, size_t len, size_t indent = 0, size_t bytes_per_line = 16) : m_gap(0)
	{
		const size_t line_size = get_line_size(bytes_per_line, indent);
		size_t line_count = (len + bytes_per_line - 1) / bytes_per_line;
		resize(line_size * line_count + 1);
		_hex_dump(m_buf, data, len, indent, bytes_per_line);
	}

private:
	size_t m_gap;

	size_t get_line_size(size_t bytes_per_line, size_t indent)
	{
		return bytes_per_line * 4 + indent + m_gap + 1;
	}
	void _hex_dump(T * buf, const void * data, size_t len, size_t indent, size_t bytes_per_line)
	{
		const char cmap[] = "0123456789ABCDEF";
		const size_t ascii_pos = bytes_per_line * 3 + indent + m_gap;
		const size_t line_size = get_line_size(bytes_per_line, indent);

		const T * p = (const T *)data;
		const T * q = p + len;
		T * line = buf;
		for (; p < q; p += bytes_per_line, line += line_size)
		{
			for (size_t i = 0; i < line_size; i++) line[i] = ' ';
			for (size_t j = 0; j < bytes_per_line && p + j < q; j++)
			{
				int v = (p[j] + 256) % 256;
				line[indent + j*3] = cmap[v / 16];
				line[indent + j*3+1] = cmap[v % 16];
				line[indent + ascii_pos + j] = (v >= 0x20 && v <= 0x80)? static_cast<char>(v) : '.';
			}
			line[line_size - 1] = '\n';
		}
		*line = 0;
		if (line > buf) line[-1] = 0;
	}
};

/** err_desc��ȡϵͳ�������� 
 */
template <typename T, size_t buf_size = 1024>
class ed_win : public tp::format_shim<T, buf_size>
{
public:
	ed_win()
	{
		build_desc(::GetLastError());
	}
	explicit ed_win(DWORD err_code)
	{
		build_desc(err_code);
	}
private:
	void build_desc(DWORD err_code)
	{
		T* msg;
		DWORD len = tp::aw::FormatMessage(
			FORMAT_MESSAGE_IGNORE_INSERTS | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_ALLOCATE_BUFFER,
			NULL,
			err_code,
			0,
			reinterpret_cast<T*>(&msg),
			0,
			NULL);
		resize(len + 1);
		if (len > 0)
		{
			for (T* p = msg + len; p > msg && (*p == '\0' || *p == '\r' || *p == '\n'); p--)
			{
				*p = '\0';
			}
			tp::aw::strncpy_s(m_buf, m_buf_size, msg, _TRUNCATE);
		}
		else
		{
			m_buf[0] = 0;
		}
		::LocalFree(msg);
	}
};

template <typename T, size_t buf_size = 1024>
class ed_com : public ed_win<T, buf_size>
{
public:
	explicit ed_com(HRESULT hr) : ed_win(static_cast<DWORD>(hr))
	{
	}
};

template <typename T, size_t buf_size = 1024>
class ed_std : public tp::format_shim<T, buf_size>
{
public:
	ed_std()
	{
		build_desc(errno);
	}
	ed_std(int err_code)
	{
		build_desc(err_code);
	}

private:
	void build_desc(int err_code)
	{
		// �޷�����õ���ʽ����������Ļ�������С����������������128
		resize(128);
		tp::aw::strerror_s(m_buf, m_buf_size, err_code);
	}
};

typedef cfmt<char>             czA;
typedef cfmt<wchar_t>          cz;

typedef hex_dumper<char>       hex_dumpA;
typedef hex_dumper<wchar_t>    hex_dump;

typedef ed_win<char>           edwinA;
typedef ed_win<wchar_t>        edwin;
typedef ed_com<char>           edcomA;
typedef ed_com<wchar_t>        edcom;
typedef ed_std<char>           edstdA;
typedef ed_std<wchar_t>        edstd;

}