#pragma once

// ----------------------------------------------------------------------------
// ��Դ�Զ��ͷź�AUTO_RELEASE
// �ڵ�ǰ�����ʱ�Զ�����ע���f(r)
// ��: AUTO_RELEASE(fp, fclose)

#define CONCAT_INNER(a,b) a##b
#define CONCAT(a,b) CONCAT_INNER(a,b)

#define AUTO_RELEASE(r, f) \
	const tp::_inner::call_on_ret_base &CONCAT(coeb__,__LINE__) = tp::_inner::f_coe(f, r); \
	CONCAT(coeb__,__LINE__).dummy(); /*avoid warning */

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
        mutable F m_fun_ptr;               // ��Դ�ͷź���
        P m_param;                         // ��Դ
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