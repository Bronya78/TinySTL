#ifndef TINYSTL_FUNCTIONAL_H_
#define TINYSTL_FUNCTIONAL_H_

#include <cstddef>

//仿函数的定义
//仿函数是一种有函数性质的对象
//主要的应用场景是在算法的谓词部分
//算法的谓词用于代替原生的操作方法,一般有两种处理方法
//一种是将多条操作设计为一个函数,然后将函数指针作为谓词传入算法
//一种就是将这些操作设计为一个仿函数(实际是一个class),然后将这个class实例化为对象,再将这个对象作为参数传递给算法
namespace mystl{

    //第一部分仿函数可以配接到STL的关键
    //仿函数的相应型别主要用于表现函数参数型别和返回值型别
    //为了方便,STL定义了一元仿函数和二元仿函数(不支持三元仿函数)
    //每一个仿函数选择其中的一个继承,就会获得这些型别,就拥有了自动配接的能力

// 定义一元函数的参数型别和返回值型别
template <class Arg, class Result>
struct unarg_function
{
  typedef Arg       argument_type;
  typedef Result    result_type;
};

// 定义二元函数的参数型别的返回值型别
template <class Arg1, class Arg2, class Result>
struct binary_function
{
  typedef Arg1      first_argument_type;
  typedef Arg2      second_argument_type;
  typedef Result    result_type;
};

// 函数对象：加法
//仿函数的重点: 对()操作符的重载,以便通过下面的方式使用
//plus<int> add;   add(x,y);//常用
//或者plus<int>(x,y);//临时对象使用,不常用
template <class T>
struct plus :public binary_function<T, T, T>
{
  T operator()(const T& x, const T& y) const { return x + y; }
};

// 函数对象：减法
template <class T>
struct minus :public binary_function<T, T, T>
{
  T operator()(const T& x, const T& y) const { return x - y; }
};

// 函数对象：乘法
template <class T>
struct multiplies :public binary_function<T, T, T>
{
  T operator()(const T& x, const T& y) const { return x * y; }
};

// 函数对象：除法
template <class T>
struct divides :public binary_function<T, T, T>
{
  T operator()(const T& x, const T& y) const { return x / y; }
};

// 函数对象：取模运算
template <class T>
struct modulus :public binary_function<T, T, T>
{
  T operator()(const T& x, const T& y) const { return x % y; }
};

// 函数对象：否定,一元函数
template <class T>
struct negate :public unarg_function<T, T>
{
  T operator()(const T& x) const { return -x; }
};

// 加法的证同元素,A加上B=A,那么B就是A 的证同元素,比如0(任何数字+0都等于原数)
template <class T>
T identity_element(plus<T>) { return T(0); }

// 乘法的证同元素A*B=A,那么B就是A的乘同元素,比如1
template <class T>
T identity_element(multiplies<T>) { return T(1); }

// 函数对象：等于
template <class T>
struct equal_to :public binary_function<T, T, bool>
{
  bool operator()(const T& x, const T& y) const { return x == y; }
};

// 函数对象：不等于
template <class T>
struct not_equal_to :public binary_function<T, T, bool>
{
  bool operator()(const T& x, const T& y) const { return x != y; }
};

// 函数对象：大于
template <class T>
struct greater :public binary_function<T, T, bool>
{
  bool operator()(const T& x, const T& y) const { return x > y; }
};

// 函数对象：小于
template <class T>
struct less :public binary_function<T, T, bool>
{
  bool operator()(const T& x, const T& y) const { return x < y; }
};

// 函数对象：大于等于
template <class T>
struct greater_equal :public binary_function<T, T, bool>
{
  bool operator()(const T& x, const T& y) const { return x >= y; }
};

// 函数对象：小于等于
template <class T>
struct less_equal :public binary_function<T, T, bool>
{
  bool operator()(const T& x, const T& y) const { return x <= y; }
};

// 函数对象：逻辑与
template <class T>
struct logical_and :public binary_function<T, T, bool>
{
  bool operator()(const T& x, const T& y) const { return x && y; }
};

// 函数对象：逻辑或
template <class T>
struct logical_or :public binary_function<T, T, bool>
{
  bool operator()(const T& x, const T& y) const { return x || y; }
};

// 函数对象：逻辑非
template <class T>
struct logical_not :public unarg_function<T, bool>
{
  bool operator()(const T& x) const { return !x; }
};

// 证同函数：不会改变元素，返回本身
template <class T>
struct identity :public unarg_function<T, bool>
{
  const T& operator()(const T& x) const { return x; }
};

// 选择函数：接受一个 pair，返回第一个元素
template <class Pair>
struct selectfirst :public unarg_function<Pair, typename Pair::first_type>
{
  const typename Pair::first_type& operator()(const Pair& x) const
  {
    return x.first;
  }
};

// 选择函数：接受一个 pair，返回第二个元素
template <class Pair>
struct selectsecond :public unarg_function<Pair, typename Pair::second_type>
{
  const typename Pair::second_type& operator()(const Pair& x) const
  {
    return x.second;
  }
};

// 投射函数：返回第一参数
template <class Arg1, class Arg2>
struct projectfirst :public binary_function<Arg1, Arg2, Arg1>
{
  Arg1 operator()(const Arg1& x, const Arg2&) const { return x; }
};

// 投射函数：返回第二参数
template <class Arg1, class Arg2>
struct projectsecond :public binary_function<Arg1, Arg2, Arg1>
{
  Arg2 operator()(const Arg1&, const Arg2& y) const { return y; }
};
}

#endif