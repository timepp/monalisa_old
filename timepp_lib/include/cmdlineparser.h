#pragma once

#include <string>
#include <list>
#include <map>

namespace tp
{

class cmdline_parser
{
public:
	typedef bool cvt_func(const wchar_t * str, void* val);

private:
	struct parse_option
	{
		wchar_t short_name;
		std::wstring long_name;
		void * addr;
		bool has_param;
		cvt_func * cf;
		std::wstring value;
	};
	typedef std::list<parse_option> polist_t;
	typedef std::map<std::wstring, std::wstring> ssmap_t;

	polist_t m_po;
	ssmap_t m_om;
	std::list<std::wstring> m_targets;

	parse_option* lookup(const wchar_t* opt)
	{
		if (!opt) return NULL;

		for (polist_t::iterator it = m_po.begin(); it != m_po.end(); ++it)
		{
			if (it->short_name == *opt && opt[1] == 0 || it->long_name == opt)
			{
				return &(*it);
			}
		}

		return NULL;
	}

	void save_option(parse_option* po, const wchar_t *value)
	{
		po->value = value;
		if (po->cf && po->addr)
		{
			(*po->cf)(value, po->addr);
		}
	}
	bool save_option(const wchar_t* opt, const wchar_t* value)
	{
		parse_option* po = lookup(opt);
		if (po)
		{
			save_option(po, value);
			return true;
		}
		return false;
	}


public:
	
	static bool cf_bool(const wchar_t *, void* val)
	{
		*reinterpret_cast<bool*>(val) = true;
		return true;
	}
	static bool cf_string(const wchar_t * str, void * val)
	{
		*reinterpret_cast<std::wstring*>(val) = str;
		return true;
	}

	bool add_option(const wchar_t short_name, const wchar_t * long_name, void * addr, bool has_param = false, cvt_func * cf = 0)
	{
		parse_option po;
		po.short_name = short_name;
		po.long_name  = long_name?  long_name:  L"";
		po.addr = addr;
		po.cf = cf;
		po.has_param = has_param;
		if (!has_param && !cf)
		{
			cf = &cmdline_parser::cf_bool;
		}
		m_po.push_back(po);

		return true;
	}

	bool parse(int argc, wchar_t** argv)
	{
		for (int i = 1; i < argc; i++)
		{
			if (argv[i][0] != '-')
			{
				m_targets.push_back(argv[i]);
				continue;
			}

			if (argv[i][1] == '-')
			{
				// 两个减号
				const wchar_t *p = wcschr(argv[i], L'=');
				if (p != NULL)
				{
					std::wstring opt(argv[i] + 2, p - argv[i] - 2);
					if (!save_option(opt.c_str(), p + 1)) return false;
				}
				else
				{
					parse_option* po = lookup(argv[i] + 2);
					if (!po) return false;
					if (po->has_param)
					{
						if (i >= argc - 1) return false;
						save_option(po, argv[++i]);
					}
					else
					{
						save_option(po, L"");
					}
				}
			}
			else
			{
				// 一个减号
				for (const wchar_t * p = argv[i] + 1; *p; p++)
				{
					wchar_t key[] = {*p, 0};
					parse_option* po = lookup(key);
					if (!po) return false;
					if (po->has_param)
					{
						if (p[1]) return false;
						if (i >= argc - 1) return false;
						save_option(po, argv[++i]);
					}
					else
					{
						save_option(po, L"");
					}
				}
			}
		}

		return true;
	}

	bool parse(const wchar_t * cmdline);

	std::wstring get_option(const wchar_t * opt)
	{
		if (!opt) return L"";

		ssmap_t::const_iterator it = m_om.find(opt);
		if (it != m_om.end())
		{
			return it->second;
		}

		parse_option * p = lookup(opt);
		if (p != NULL)
		{
			return p->value;
		}

		return L"";
	}

	std::list<std::wstring> get_targets() const
	{
		return m_targets;
	}
};

}