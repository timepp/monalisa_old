#pragma once

#include <set>
#include <string>
#include <time.h>
#include <stdlib.h>
#include <map>

#include "api_wrapper.h"
#include "tstring.h"
#include "lock.h"

#define TP_LOG_MULTI_THREAD

namespace tp
{
	// 日志中的文字类型
	enum text_class
	{
		tc_blank,         // 空白字符
		tc_normal,        // 普通
		tc_header,        // 时间戳
		tc_type,          // 类别
		tc_thread,        // 线程标识
		tc_other          // 其它
	};

	// 日志设备
	class log_device
	{
	public:
		virtual bool open() = 0;
		virtual bool close() = 0;
		virtual size_t write(const char * buf, size_t len, text_class tc) = 0;
		virtual size_t write(const wchar_t * buf, size_t len, text_class tc) = 0;
		virtual bool flush() = 0;
		virtual ~log_device(){}
	};

	// 注册设备，mask表示设备能接受的日志类型组合
	void log_add_device(log_device * ld, unsigned int mask, const char * time_fmt, bool show_millisec = true, bool auto_delete = true);
	void log_add_device(log_device * ld, unsigned int mask, const wchar_t * time_fmt, bool show_millisec = true, bool auto_delete = true);
	void log_unadd_device(log_device * ld);

	// 设置类别名字
	void log_set_type_names(const char * names);
	void log_set_type_names(const wchar_t * names);
#ifdef TP_LOG_MULTI_THREAD
	void log_set_thread_name(const char * name);
	void log_set_thread_name(const wchar_t * name);
#endif

	// 记录日志，配合tp::cfmt使用
	void log(size_t log_type, const char * text, bool flush = true);
	void log(size_t log_type, const wchar_t * text, bool flush = true);
	void log(const char * text, bool flush = true);
	void log(const wchar_t * text, bool flush = true);

	namespace _inner
	{
		// singleton
		class logger
		{
		private:
			logger()
			{
				m_type_name.assign(32, 0);
#ifdef TP_LOG_MULTI_THREAD
				::InitializeCriticalSection(&m_cs);
				m_main_thread_id = ::GetCurrentThreadId();
#endif
			}
			~logger()
			{
				for (ld_set_t::const_iterator ci = m_ld_set.begin(); ci != m_ld_set.end(); ++ci)
				{
					ci->ld->close();
					if (ci->auto_delete)
						delete ci->ld;
				}
			}
			logger(const logger&);
			logger& operator = (const logger&);

			struct device_info
			{
				log_device * ld;
				unsigned int mask;
				tstring time_fmt;
				bool auto_delete;

				bool operator < (const device_info & di) const
				{
					return ld < di.ld;
				}
			};

			typedef std::set<device_info> ld_set_t;
			ld_set_t m_ld_set;
			std::string m_type_name;
			bool m_show_millisec;
			int m_indent;

#ifdef TP_LOG_MULTI_THREAD
			CRITICAL_SECTION m_cs;
			DWORD m_main_thread_id;
			struct thread_info
			{
				size_t index;
				tstring name;
			};
			typedef std::map<DWORD, thread_info> ti_map_t;
			ti_map_t m_ti_map;
#endif

		public:
			static logger * instance()
			{
				static logger _l;
				return &_l;
			}

			template <typename T>
			void add_device(log_device * ld, unsigned int mask, T * time_fmt, bool show_millisec, bool auto_delete)
			{
				device_info di = {ld, mask, time_fmt, auto_delete};
				m_show_millisec = show_millisec;
				ld_set_t::iterator it = m_ld_set.find(di);
				if (it != m_ld_set.end())
				{
					*it = di;
				}
				else
				{
					ld->open();
					m_ld_set.insert(di);
				}
			}

			void unadd_device(log_device * ld)
			{
				device_info di = {ld};
				ld->close();
				m_ld_set.erase(di);
			}

			template <typename T>
			void set_type_names(const T * names)
			{
				size_t i;
				for (i = 0; names[i]; i++)
				{
					m_type_name[i] = static_cast<char>(names[i]);
				}
				for (; i < 32; i++)
				{
					m_type_name[i] = 0;
				}
			}
#ifdef TP_LOG_MULTI_THREAD
			template <typename T>
			void set_thread_name(const T * name)
			{
				critical_lock lock(m_cs);
				DWORD thread_id = ::GetCurrentThreadId();
				ti_map_t::iterator it = m_ti_map.find(thread_id);
				if (it == m_ti_map.end())
				{
					thread_info ti = {m_ti_map.size()};
					it = m_ti_map.insert(ti_map_t::value_type(thread_id, ti)).first;
				}
				it->second.name.assign(name);
			}
#endif
			void inc_indent(int indent)
			{
				m_indent += indent;
			}
			
			template <typename T>
			void log(size_t log_type, const T * text, bool flush = false)
			{
#ifdef TP_LOG_MULTI_THREAD
				thread_info ti = {};
				critical_lock lock(m_cs);
				T idfmt1[] = {'(', '%', '0', '4', 'X', ')', '\0'};
				T idfmt2[] = {'(', '%', 's', ')', '\0'};
				T thread_str[32];
				int thread_str_len;
				DWORD thread_id = ::GetCurrentThreadId();
				if (thread_id != m_main_thread_id)
				{
					ti_map_t::const_iterator it = m_ti_map.find(thread_id);
					if (it == m_ti_map.end())
					{
						ti.index = m_ti_map.size();
						m_ti_map.insert(ti_map_t::value_type(thread_id, ti));
					}
					else
					{
						ti = it->second;
					}

					if (*((const T *)ti.name))
					{
						thread_str_len = tp::aw::snprintf_s(thread_str, 32, idfmt2, (const T*)ti.name);
					}
					else
					{
						thread_str_len = tp::aw::snprintf_s(thread_str, 32, idfmt1, thread_id);
					}
				}
#endif

				T time_str[64];
				T indent_str[80];
				T blank[] = {' ', 0};
				T cr[]    = {'\n', 0};
				T msfmt[] = {'.','%','0','3','d','\0'};
				T type_str[] = {static_cast<T>(m_type_name[log_type]), '|', 0};
				size_t type_len = type_str[0] ? 2U : 0U;

				if (m_indent > sizeof(indent_str)/sizeof(T)) m_indent = sizeof(indent_str)/sizeof(T);
				for (int i = 0; i < m_indent; i++) indent_str[i] = ' ';

				time_t ct = time(NULL);
				for (ld_set_t::const_iterator ci = m_ld_set.begin(); ci != m_ld_set.end(); ++ci)
				{
					if (ci->mask & (1 << log_type))
					{
						struct tm otm;
						localtime_s(&otm, &ct);
						size_t time_len = aw::strftime(time_str, sizeof(time_str)/sizeof(time_str[0]) - 1, ci->time_fmt, &otm);
						if (m_show_millisec)
						{
							SYSTEMTIME st;
							GetSystemTime(&st);
							aw::strncpy_s(time_str + time_len, 64 - time_len, cfmt<T>(msfmt, st.wMilliseconds), _TRUNCATE);
							time_str[63] = '\0';
							time_len += 4;
						}
						
						const T * p = text;
						const T * q = text;
						do
						{
							if (*q == '\n' || (*q == 0 && q > p))
							{
								ci->ld->write(time_str, time_len, tc_header);
								ci->ld->write(blank, 1, tc_blank);
#ifdef TP_LOG_MULTI_THREAD
								if (thread_id != m_main_thread_id)
								{
									ci->ld->write(thread_str, static_cast<size_t>(thread_str_len), tc_thread);
									ci->ld->write(blank, 1, tc_blank);
								}
#endif
								ci->ld->write(type_str, type_len, tc_type);
								ci->ld->write(indent_str, static_cast<size_t>(m_indent), tc_blank);
								ci->ld->write(p, static_cast<size_t>(q - p), tc_normal);
								ci->ld->write(cr, 1, tc_blank);
								p = q + 1;
							}
						} while (*q++);
						
						if (flush) 
						{
							ci->ld->flush();
						}
					}
				}
			}

		}; // class logger

	} // namespace _inner

	inline void log_add_device(log_device * ld, unsigned int mask, const char * time_fmt, bool show_millisec, bool auto_delete)
	{
		_inner::logger::instance()->add_device(ld, mask, time_fmt, show_millisec, auto_delete);
	}

	inline void log_add_device(log_device * ld, unsigned int mask, const wchar_t * time_fmt, bool show_millisec, bool auto_delete)
	{
		_inner::logger::instance()->add_device(ld, mask, time_fmt, show_millisec, auto_delete);
	}

	inline void log_unadd_device(log_device * ld)
	{
		_inner::logger::instance()->unadd_device(ld);
	}

	inline void log(size_t log_type, const char * text, bool flush)
	{
		_inner::logger::instance()->log(log_type, text, flush);
	}
	inline void log(size_t log_type, const wchar_t * text, bool flush)
	{
		_inner::logger::instance()->log(log_type, text, flush);
	}
	inline void log(const char * text, bool flush)
	{
		_inner::logger::instance()->log(0, text, flush);
	}
	inline void log(const wchar_t * text, bool flush)
	{
		_inner::logger::instance()->log(0, text, flush);
	}

	inline void log_set_type_names(const char * names)
	{
		_inner::logger::instance()->set_type_names(names);
	}


	inline void log_set_type_names(const wchar_t * names)
	{
		_inner::logger::instance()->set_type_names(names);
	}
#ifdef TP_LOG_MULTI_THREAD
	inline void log_set_thread_name(const char * name)
	{
		_inner::logger::instance()->set_thread_name(name);
	}
	inline void log_set_thread_name(const wchar_t * name)
	{
		_inner::logger::instance()->set_thread_name(name);
	}
#endif
	inline void log_inc_indent(int indent)
	{
		_inner::logger::instance()->inc_indent(indent);
	}

	class log_indenter
	{
	public:
		explicit log_indenter(int indent) : m_indent(indent)
		{
			log_inc_indent(m_indent);
		}
		~log_indenter()
		{
			log_inc_indent(-m_indent);
		}
	private:
		int m_indent;
	};
};
