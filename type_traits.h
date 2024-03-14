#ifndef TINYSTL_TYPE_TRAITS_H_
#define TINYSTL_TYPE_TRAITS_H_


namespace mystl{


template<class T,T v>
struct m__integral_constant
{
    //封装编译时常量的创建细节
    static constexpr T value = v;
};

template <bool b>
using m_bool_constant = m_integral_constant<bool, b>;
//方便直接使用true和false两个编译时常量
typedef m_bool_constant<true>  m_true_type;
typedef m_bool_constant<false> m_false_type;

//前向申明,保证下面的引用是合法的,但是还未具体定义pair
template <class T1, class T2>
struct pair;
// 声明结束

//基础版
template <class T>
struct is_pair : mystl::m_false_type {};
//对pair的偏特化版,如果传入的是mystl::pair<T1, T2>,那么ispair=编译时常量true
template <class T1, class T2>
struct is_pair<mystl::pair<T1, T2>> : mystl::m_true_type {};


}

#endif