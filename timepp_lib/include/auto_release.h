#pragma once

// ----------------------------------------------------------------------------
// 资源自动释放宏AUTO_RELEASE
// 在当前块结束时自动调用注册的f(r)
// 例: AUTO_RELEASE(fp, fclose)

#define CONCAT_INNER(a,b) a##b
#define CONCAT(a,b) CONCAT_INNER(a,b)

#define AUTO_RELEASE(r, f) \
	const tp::_inner::call_on_ret_base &CONCAT(coeb__,__LINE__) = tp::_inner::f_coe(f, r); \
	CONCAT(coeb__,__LINE__).dummy(); /*avoid warning */

#define ON_LEAVE(statement) \
	struct CONCAT(s_ol_, __LINE__) { \
		~CONCAT(s_ol_, __LINE__)() { statement; } \
	} CONCAT(v_ol_, __LINE__);

#define ON_LEAVE_1(type, var, statement) \
	struct CONCAT(s_ol_, __LINE__) { \
		type var; \
		CONCAT(s_ol_, __LINE__)(type v): var(v) {} \
		~CONCAT(s_ol_, __LINE__)() { statement; } \
	} CONCAT(v_ol_, __LINE__)(var);

#define ON_LEAVE_2(type1, var1, type2, var2, statement) \
	struct CONCAT(s_ol_, __LINE__) { \
		type1 var1; type2 var2; \
		CONCAT(s_ol_, __LINE__)(type1 v1, type2 v2): var1(v1), var2(v2) {} \
		~CONCAT(s_ol_, __LINE__)() { statement; } \
	} CONCAT(v_ol_, __LINE__)(var1, var2);


namespace tp
{

namespace _inner
{

// call_on_ret_base
struct call_on_ret_base
{
	void dummy() const {}
};
// call_on_ret
template <typename F, typename P>
struct call_on_ret : call_on_ret_base
{
        mutable F m_fun_ptr;               // 资源释放函数
        P m_param;                         // 资源
        call_on_ret(F f, P p) : m_fun_ptr(f), m_param(p) {}
        call_on_ret(const call_on_ret& c)
                : m_fun_ptr(c.m_fun_ptr), m_param(c.m_param)
        {
                c.m_fun_ptr = 0;
        }
        ~call_on_ret()
        {
                if (m_fun_ptr) m_fun_ptr(m_param);
        }
};
template <typename F, typename P>
call_on_ret<F, P> f_coe(F f, P p)
{
        return call_on_ret<F, P>(f, p);
}

} // namespace _inner

} // namespace tp
