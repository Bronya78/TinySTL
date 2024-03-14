#ifndef TINYSTL_CONSTRUCT_H_
#define TINYSTL_CONSTRUCT_H_

/**
 * 包含两个函数construct和destory
*/
#include<new>

#include"type_traits.h"
#include"iterator.h"

#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable : 4100)  // unused parameter
#endif // _MSC_VER


namespace mystl{

//construct构造函数
template<class T>
void construct(T* ptr){
    ::new ((void*) ptr) T();
}
//使用Ty2类型的value进行构造
template <class T1, class T2>
void construct(T1* ptr, const T2& value)
{
  ::new ((void*)ptr) T1(value);
}
//右值,使用完美传递的方法构造
template <class T, class... Args>
void construct(T* ptr, Args&&... args)
{
  ::new ((void*)ptr) T(mystl::forward<Args>(args)...);
}


//destroy
//std::true_type,std::false_type这个参数决定对象是否是平凡析构的,如果是平凡析构的,那么就不需要任何操作
template <class T>
void destroy_one(T*, std::true_type) {}
//如果不是平凡析构的,那么就需要显式销毁
template <class T>
void destroy_one(T* pointer, std::false_type)
{
  if (pointer != nullptr)
  {
    pointer->~T();
  }
}
//两个迭代器范围内的销毁
template <class ForwardIter>
void destroy_cat(ForwardIter , ForwardIter , std::true_type) {}

template <class ForwardIter>
void destroy_cat(ForwardIter first, ForwardIter last, std::false_type)
{
  for (; first != last; ++first)
    destroy(&*first);
}

//单个对象的销毁
template <class T>
void destroy(T* pointer)
{
    //传入的第二个参数,检查pointer是否为平凡析构
  destroy_one(pointer, std::is_trivially_destructible<T>{});
}

template <class ForwardIter>
void destroy(ForwardIter first, ForwardIter last)
{
  destroy_cat(first, last, std::is_trivially_destructible<
              typename iterator_traits<ForwardIter>::value_type>{});
}



}// namespace mystl

#ifdef _MSC_VER
#pragma warning(pop)
#endif // _MSC_VER




#endif