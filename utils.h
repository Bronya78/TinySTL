#ifndef TINYSTL_UTILS_H
#define TINYSTL_UTILS_H

#include <cstddef>
#include <type_traits>

namespace mystl{
    // 移动语义需要的move操作

template <class T>
typename std::remove_reference<T>::type&& move(T&& arg) noexcept
{
  return static_cast<typename std::remove_reference<T>::type&&>(arg);
}

// 完美传递需要的forward操作

template <class T>
T&& forward(typename std::remove_reference<T>::type& arg) noexcept
{
  //将传入一个左值引用,转换为右值引用
  return static_cast<T&&>(arg);
}

template <class T>
T&& forward(typename std::remove_reference<T>::type&& arg) noexcept
{
  //进行编译时检查,如果传入一个左值引用则引发断言错误bad forward
  //借用std的is_lvalue_reference帮助判断是否为左值引用
  static_assert(!std::is_lvalue_reference<T>::value, "bad forward");
  //然后将其转化为右值引用
  return static_cast<T&&>(arg);
}


//swap
template<class Tp>
void swap(Tp &lhs,Tp &rhs){
    auto tmp(mystl::move(lhs));
    lhs=mystl::move(rhs);
    rhs=mystl::move(lhs);
}

//swap_range
template<class ForwardIter1,class ForwardIter2>
ForwardIter2 swap_range(ForwardIter1 first1,ForwardIter1 last1,ForwardIter2 first2){
    //循环体中的(void)++first2表示计算++first2,但是不关注返回的结果
    for(;first1!=last1;++first1,(void)++first2){
        mystl::swap((*first1),(*first2));
    }
    return first2;
}

template <class Tp, size_t N>
void swap(Tp(&a)[N], Tp(&b)[N])
{
  mystl::swap_range(a, a + N, b);
}



//pair的定义
//结构体模板pair
//两个成员,第一个first,第二个second,两个成员类型不严格相同或不同
template<class T1,class T2>
struct pair
{
    typedef T1 first_type;
    typedef T2 second_type;

    first_type first;
    second_type second;

    //默认构造函数
    template<class Other1=T1,class Other2=T2,
    typename =typename std::enable_if<
    std::is_default_constructible<Other1>::value&&
    std::is_default_constructible<Other2>::value,void>::type>
    constexpr pair():first(),second()
    {}

    //is_copy_constructible检测U1,U2是否可以被拷贝
    //is_convertible检测U1,U2是否可以被隐式转化为T1,和T2类型
    template <class U1 = T1, class U2 = T2,
    typename std::enable_if<
    std::is_copy_constructible<U1>::value &&
    std::is_copy_constructible<U2>::value &&
    std::is_convertible<const U1&, T1>::value &&
    std::is_convertible<const U2&, T2>::value, int>::type = 0>
    constexpr pair(const T1& a, const T2& b): first(a), second(b)
    {}


    template <class U1 = T1, class U2 = T2,
    typename std::enable_if<
    std::is_copy_constructible<U1>::value &&
    std::is_copy_constructible<U2>::value &&
    (!std::is_convertible<const U1&, T1>::value ||
     !std::is_convertible<const U2&, T2>::value), int>::type = 0>
    explicit constexpr pair(const T1& a, const T2& b): first(a), second(b)
    //explicit声明构造函数为显式构造函数。显式构造函数要求在使用构造函数进行隐式类型转换时，
    //需要显式地调用构造函数。这可以防止意外的隐式类型转换
    {}

    //is_constructible判断T1是否可以由Other1来构造
    //is_Convertible判断Other类型的右值引用是否可以由T1隐式转换而来
    //如果可以的话,那么使用隐式转化构造方法
    template<class Other1=T1,class Other2=T2,
    typename std::enable_if<
    std::is_constructible<T1,Other1>::value&&
    std::is_constructible<T2,Other2>::value&&
    std::is_convertible<Other1&&,T1>::value&&
    std::is_convertible<Other2&&,T2>::value , int>::type=0>
    constexpr pair(Other1 &&a,Other2 &&b)
    :first(mystl::forward<Other1>(a)),
          second(mystl::forward<Other2>(b))
    {}


    //Ty1 类型可以通过 Other1 类型进行构造
    //Ty2 类型可以通过 Other2 类型进行构造
    //Other1 不能隐式转换为 Ty1 或者 Other2 不能隐式转换为 Ty2
    //此时使用explicit关键字强制要求使用显式构造方案
    template<class Other1=T1,class Other2=T2,
    typename std::enable_if<
    std::is_constructible<T1,Other1>::value&&
    std::is_constructible<T2,Other2>::value&&
    ( !std::is_convertible<Other1&&,T1>::value||
      !std::is_convertible<Other2&&,T2>::value )
    , int>::type=0>
    explicit constexpr pair(Other1 &&a,Other2 &&b)
    :first(mystl::forward<Other1>(a)),
          second(mystl::forward<Other2>(b))
    {}


    //T1是否可以由consr Other1构造
    //cosnt Other1和const Other2是否可以由T1和T2隐式转化
    //如果上述条件都满足,则使用隐式转化版的构造函数,接收一个pair,使用这个pair来进行初始化
    template<class Other1=T1,class Other2=T2,
    typename std::enable_if<
    std::is_constructible<T1,const Other1&>::value&&
    std::is_constructible<T2,const Other2&>::value&&
    std::is_convertible<const Other1&,T1>::value&&
    std::is_convertible<const Other2&,T2>::value,int>::type=0>
    constexpr pair(const pair<Other1,Other2>& other):first(other.first),second(other.second)
    {}

    //cosnt Other1和const Other2如果不能都由T1和T2隐式转化
    //则强制使用显式构造版本接收一个pair,使用这个pair来进行初始化

    template<class Other1=T1,class Other2=T2,
    typename std::enable_if<
    std::is_constructible<T1,const Other1&>::value&&
    std::is_constructible<T2,const Other2&>::value&&
    (!std::is_convertible<const Other1&,T1>::value||
    !std::is_convertible<const Other2&,T2>::value),int>::type=0>
    explicit constexpr pair(const pair<Other1,Other2>& other):first(other.first),second(other.second)
    {}


    //传入一个pair的右值引用
    template<class Other1=T1,class Other2=T2,
    typename std::enable_if<
    std::is_constructible<T1,const Other1>::value&&
    std::is_constructible<T2,const Other2>::value&&
    std::is_convertible<const Other1,T1>::value&&
    std::is_convertible<const Other2,T2>::value,int>::type=0>
    constexpr pair(pair<Other1,Other2>&& other)
    :first(mystl::forward(other.first)),
    second(mystl::forward(other.second))
    {}

    //显式构造版
    template<class Other1=T1,class Other2=T2,
    typename std::enable_if<
    std::is_constructible<T1,const Other1>::value&&
    std::is_constructible<T2,const Other2>::value&&
    (!std::is_convertible<const Other1,T1>::value||
    !std::is_convertible<const Other2,T2>::value),int>::type=0>
    explicit constexpr pair(pair<Other1,Other2>&& other)
    :first(mystl::forward(other.first)),
    second(mystl::forward(other.second))
    {}


    pair& operator=(const pair& rhs){
      if(this!=&rhs){
        this->first=rhs.first;
        this->second=rhs.second;
      }
      return *rhs;
    }

    //右值引用特化
    pair& operator=(pair&& rhs){
      if(this!=&rhs){
        this->first=mystl::move(rhs.first);
        this->second=mystl::move(rhs.second);
      }
      return *this;
    }


    //拷贝构造函数
    template<class Other1,class Other2>
    pair& operator=(const pair<Other1,Other2> & rhs){
      this->first=rhs.first;
      this->second=rhs.second;
      return *this;
    }

    //移动语义版本
    template<class Other1,class Other2>
    pair& operator=(const pair<Other1,Other2> && rhs){
      this->first=mystl::forward<Other1>(rhs.first);
      this->second=mystl::forward<Other2>(rhs.second);
      return *this;
    }



    //移动构造函数和拷贝构造函数,析构函数都使用默认的
    pair(const pair& rhs) = default;
    pair(pair&& rhs) = default;
    ~pair()=default;

    void swap(pair& other){
      if(this!=&other){
        mystl::swap(this->first,other.first);
        mystl::swap(this->second,other.second);
      }
    }

 
};

//重载比较操作符
template<class T1,class T2>
bool operator==(const pair<T1,T2>&lhs,const pair<T1,T2> &rhs){
  return lhs.first==rhs.first&&lhs.second==rhs.second;
}

template<class T1,class T2>
bool operator<(const pair<T1,T2> &lhs,const pair<T1,T2> &rhs){
  return lhs.first<rhs.first||(lhs.first==rhs.first&&lhs.second<rhs.second);
}

template<class T1,class T2>
bool operator!=(const pair<T1,T2> &lhs,const pair<T1,T2> &rhs){
  return !(lhs==rhs);
}

template<class T1,class T2>
bool operator>(const pair<T1,T2> &lhs,const pair<T1,T2> &rhs){
  return rhs>lhs;
}


template<class T1,class T2>
bool operator<=(const pair<T1,T2> &lhs,const pair<T1,T2> &rhs){
  return !(rhs>lhs);
}

template<class T1,class T2>
bool operator>=(const pair<T1,T2> &lhs,const pair<T1,T2> &rhs){
  return !(lhs<rhs);
}

// 重载 mystl 的 swap
template <class Ty1, class Ty2>
void swap(pair<Ty1, Ty2>& lhs, pair<Ty1, Ty2>& rhs)
{
  lhs.swap(rhs);
}

// 全局函数，让两个数据成为一个 pair
template <class Ty1, class Ty2>
pair<Ty1, Ty2> make_pair(Ty1&& first, Ty2&& second)
{
  return pair<Ty1, Ty2>(mystl::forward<Ty1>(first), mystl::forward<Ty2>(second));
}

}



#endif