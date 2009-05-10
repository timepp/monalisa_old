#pragma once

#include "log.h"
#include <time.h>

namespace tp
{
	enum context_id
	{
		LCID_TIME = 11,
		LCID_TEXT = 12,
		LCID_TYPE = 13,
	};

class lc_time : public log_context
{
public:
	lc_time(const wchar_t * time_fmt = NULL, bool show_millisec = false) : log_context(LCID_TIME)
	{
		if (!time_fmt) time_fmt = L"%H:%M:%S";

		m_time_fmt = time_fmt;
		m_show_millisec = show_millisec;
	}

	std::wstring value(unsigned int type) const
	{
		wchar_t time_str[64] = {0};
		time_t ct = time(NULL);
		struct tm otm;
		localtime_s(&otm, &ct);
		size_t time_len = aw::strftime(time_str, sizeof(time_str)/sizeof(time_str[0]) - 1, m_time_fmt.c_str(), &otm);
		if (m_show_millisec)
		{
			SYSTEMTIME st;
			GetSystemTime(&st);
			aw::strncpy_s(time_str + time_len, 64 - time_len, cfmt<wchar_t>(L".%03d", st.wMilliseconds), _TRUNCATE);
			time_str[63] = '\0';
		}
		return time_str;
	}

private:
	std::wstring m_time_fmt;
	bool m_show_millisec;
};

class lc_text : public log_context
{
public:
	lc_text(const wchar_t * text) : log_context(LCID_TEXT)
	{
		if (text) m_text = text;
	}
	std::wstring value(unsigned int type) const
	{
		return m_text;
	}
private:
	std::wstring m_text;
};

class lc_type : public log_context
{
public:
	lc_type(const wchar_t * type_str) : log_context(LCID_TYPE)
	{
		if (type_str) m_type_str = type_str;
	}
	std::wstring value(unsigned int type) const
	{
		if (type < m_type_str.size())
		{
			return m_type_str.substr(type, 1) + L"|";
		}
		return L"|";
	}
private:
	std::wstring m_type_str;
};

}