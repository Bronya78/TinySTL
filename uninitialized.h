#ifndef TINYSTL_UNINITIALIZED_H_
#define TINYSTL_UNINITIALIZED_H_

#include "algobase.h"
#include "construct.h"
#include "iterator.h"
#include "type_traits.h"
#include "utils.h"


namespace mystl{
//对于POD类型,C++11之后新标准称之为is_trivially_copy_assignable的对象来说
//可以为其提供较为低级别的拷贝操作,这个操作位于copy中的trivially_copy_assignable特化版本实现
//所以,这里直接调用copy函数
template <class InputIter, class ForwardIter>
ForwardIter 
unchecked_uninit_copy(InputIter first, InputIter last, ForwardIter result, std::true_type)
{
  return mystl::copy(first, last, result);
}

//对于非POD版本的对象,则必须为其提供安全的拷贝方案
//必须使用construct方法,并且配合严格的异常检测
template <class InputIter, class ForwardIter>
ForwardIter
unchecked_uninit_copy(InputIter first, InputIter last, ForwardIter result, std::false_type)
{
  auto cur = result;
  try
  {
    for (; first != last; ++first, ++cur)
    {
      mystl::construct(&*cur, *first);
    }
  }
  catch (...)
  {
    //存在异常则将目标位置已经构造好的对象全部销毁
    for (; result != cur; --cur)
      mystl::destroy(&*cur);
  }
  return cur;
}

//通过第四个参数来决定使用POD和非POD版拷贝方法
template <class InputIter, class ForwardIter>
ForwardIter uninitialized_copy(InputIter first, InputIter last, ForwardIter result)
{
  return mystl::unchecked_uninit_copy(first, last, result, 
                                     std::is_trivially_copy_assignable<
                                     typename iterator_traits<ForwardIter>::
                                     value_type>{});
}

//与unint_copy是一样的思路
template <class InputIter, class Size, class ForwardIter>
ForwardIter 
unchecked_uninit_copy_n(InputIter first, Size n, ForwardIter result, std::true_type)
{
  return mystl::copy_n(first, n, result).second;
}

template <class InputIter, class Size, class ForwardIter>
ForwardIter
unchecked_uninit_copy_n(InputIter first, Size n, ForwardIter result, std::false_type)
{
  auto cur = result;
  try
  {
    for (; n > 0; --n, ++cur, ++first)
    {
      mystl::construct(&*cur, *first);
    }
  }
  catch (...)
  {
    for (; result != cur; --cur)
      mystl::destroy(&*cur);
  }
  return cur;
}

template <class InputIter, class Size, class ForwardIter>
ForwardIter uninitialized_copy_n(InputIter first, Size n, ForwardIter result)
{
  return mystl::unchecked_uninit_copy_n(first, n, result,
                                        std::is_trivially_copy_assignable<
                                        typename iterator_traits<InputIter>::
                                        value_type>{});
}


//uninit_fill
template <class ForwardIter, class T>
void 
unchecked_uninit_fill(ForwardIter first, ForwardIter last, const T& value, std::true_type)
{
  mystl::fill(first, last, value);
}

template <class ForwardIter, class T>
void 
unchecked_uninit_fill(ForwardIter first, ForwardIter last, const T& value, std::false_type)
{
  auto cur = first;
  try
  {
    for (; cur != last; ++cur)
    {
      mystl::construct(&*cur, value);
    }
  }
  catch (...)
  {
    for (;first != cur; ++first)
      mystl::destroy(&*first);
  }
}

template <class ForwardIter, class T>
void  uninitialized_fill(ForwardIter first, ForwardIter last, const T& value)
{
  mystl::unchecked_uninit_fill(first, last, value, 
                               std::is_trivially_copy_assignable<
                               typename iterator_traits<ForwardIter>::
                               value_type>{});
}

//uninit_fill_n
template <class ForwardIter, class Size, class T>
ForwardIter 
unchecked_uninit_fill_n(ForwardIter first, Size n, const T& value, std::true_type)
{
  return mystl::fill_n(first, n, value);
}

template <class ForwardIter, class Size, class T>
ForwardIter 
unchecked_uninit_fill_n(ForwardIter first, Size n, const T& value, std::false_type)
{
  auto cur = first;
  try
  {
    for (; n > 0; --n, ++cur)
    {
      mystl::construct(&*cur, value);
    }
  }
  catch (...)
  {
    for (; first != cur; ++first)
      mystl::destroy(&*first);
  }
  return cur;
}

template <class ForwardIter, class Size, class T>
ForwardIter uninitialized_fill_n(ForwardIter first, Size n, const T& value)
{
  return mystl::unchecked_uninit_fill_n(first, n, value, 
                                        std::is_trivially_copy_assignable<
                                        typename iterator_traits<ForwardIter>::
                                        value_type>{});
}

//uninit_move
template <class InputIter, class ForwardIter>
ForwardIter 
unchecked_uninit_move(InputIter first, InputIter last, ForwardIter result, std::true_type)
{
  return mystl::move(first, last, result);
}

template <class InputIter, class ForwardIter>
ForwardIter 
unchecked_uninit_move(InputIter first, InputIter last, ForwardIter result, std::false_type)
{
  ForwardIter cur = result;
  try
  {
    for (; first != last; ++first, ++cur)
    {
      mystl::construct(&*cur, mystl::move(*first));
    }
  }
  catch (...)
  {
    mystl::destroy(result, cur);
  }
  return cur;
}

template <class InputIter, class ForwardIter>
ForwardIter uninitialized_move(InputIter first, InputIter last, ForwardIter result)
{
  return mystl::unchecked_uninit_move(first, last, result,
                                      std::is_trivially_move_assignable<
                                      typename iterator_traits<InputIter>::
                                      value_type>{});
}

//uninit_move_n
template <class InputIter, class Size, class ForwardIter>
ForwardIter 
unchecked_uninit_move_n(InputIter first, Size n, ForwardIter result, std::true_type)
{
  return mystl::move(first, first + n, result);
}

template <class InputIter, class Size, class ForwardIter>
ForwardIter
unchecked_uninit_move_n(InputIter first, Size n, ForwardIter result, std::false_type)
{
  auto cur = result;
  try
  {
    for (; n > 0; --n, ++first, ++cur)
    {
      mystl::construct(&*cur, mystl::move(*first));
    }
  }
  catch (...)
  {
    for (; result != cur; ++result)
      mystl::destroy(&*result);
    throw;
  }
  return cur;
}

template <class InputIter, class Size, class ForwardIter>
ForwardIter uninitialized_move_n(InputIter first, Size n, ForwardIter result)
{
  return mystl::unchecked_uninit_move_n(first, n, result,
                                        std::is_trivially_move_assignable<
                                        typename iterator_traits<InputIter>::
                                        value_type>{});
}

}

#endif