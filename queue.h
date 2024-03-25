#ifndef MYTINYSTL_QUEUE_H_
#define MYTINYSTL_QUEUE_H_

// 这个头文件包含了两个模板类 queue 和 priority_queue
// queue          : 队列
// 优先队列的实现有问题,故暂时删除
#include "deque.h"
#include "vector.h"
#include "functional.h"
#include "heap_algo.h"

namespace mystl
{

// 模板类 queue
//适配器,不是容器
// 参数一代表数据类型，参数二代表底层容器类型，缺省使用 mystl::deque 作为底层容器
template <class T, class Container = mystl::deque<T>>
class queue
{
public:
  typedef Container                           container_type;
  // 使用底层容器的型别
  typedef typename Container::value_type      value_type;
  typedef typename Container::size_type       size_type;
  typedef typename Container::reference       reference;
  typedef typename Container::const_reference const_reference;

  static_assert(std::is_same<T, value_type>::value,
                "the value_type of Container should be same with T");
private:
  container_type c_;  // 用底层容器表现 queue

public:
  // 构造、复制、移动函数

  queue() = default;

  explicit queue(size_type n) 
    :c_(n) 
  {
  }
  queue(size_type n, const value_type& value)
    :c_(n, value)
  {
  }

  template <class IIter>
  queue(IIter first, IIter last)
    :c_(first, last) 
  {
  }

  queue(std::initializer_list<T> ilist)
    :c_(ilist.begin(), ilist.end()) 
  {
  }

  queue(const Container& c) 
    :c_(c) 
  {
  }
  queue(Container&& c) noexcept(std::is_nothrow_move_constructible<Container>::value)
    :c_(mystl::move(c)) 
  {
  }

  queue(const queue& rhs) 
    :c_(rhs.c_) 
  {
  }
  queue(queue&& rhs) noexcept(std::is_nothrow_move_constructible<Container>::value)
    :c_(mystl::move(rhs.c_)) 
  {
  }

  queue& operator=(const queue& rhs) 
  {
    c_ = rhs.c_; 
    return *this; 
  }
  queue& operator=(queue&& rhs) noexcept(std::is_nothrow_move_assignable<Container>::value)
  { 
    c_ = mystl::move(rhs.c_);
    return *this;
  }

  queue& operator=(std::initializer_list<T> ilist)
  { 
    c_ = ilist; 
    return *this; 
  }

  ~queue() = default;

  // 访问元素相关操作
  reference       front()       { return c_.front(); }
  const_reference front() const { return c_.front(); }
  reference       back()        { return c_.back(); }
  const_reference back()  const { return c_.back(); }

  // 容量相关操作
  bool      empty() const noexcept { return c_.empty(); }
  size_type size()  const noexcept { return c_.size(); }

  // 修改容器相关操作
  template <class ...Args>
  void emplace(Args&& ...args)
  { c_.emplace_back(mystl::forward<Args>(args)...); }

  void push(const value_type& value) 
  { c_.push_back(value); }
  void push(value_type&& value)      
  { c_.emplace_back(mystl::move(value)); }

  void pop()                         
  { c_.pop_front(); }

  void clear()         
  { 
    while (!empty())
      pop(); 
  }

  void swap(queue& rhs) noexcept(noexcept(mystl::swap(c_, rhs.c_)))
  { mystl::swap(c_, rhs.c_); }

public:
  friend bool operator==(const queue& lhs, const queue& rhs) { return lhs.c_ == rhs.c_; }
  friend bool operator< (const queue& lhs, const queue& rhs) { return lhs.c_ <  rhs.c_; }
};

// 重载比较操作符
template <class T, class Container>
bool operator==(const queue<T, Container>& lhs, const queue<T, Container>& rhs)
{
  return lhs == rhs;
}

template <class T, class Container>
bool operator!=(const queue<T, Container>& lhs, const queue<T, Container>& rhs)
{
  return !(lhs == rhs);
}

template <class T, class Container>
bool operator<(const queue<T, Container>& lhs, const queue<T, Container>& rhs)
{
  return lhs < rhs;
}

template <class T, class Container>
bool operator>(const queue<T, Container>& lhs, const queue<T, Container>& rhs)
{
  return rhs < lhs;
}

template <class T, class Container>
bool operator<=(const queue<T, Container>& lhs, const queue<T, Container>& rhs)
{
  return !(rhs < lhs);
}

template <class T, class Container>
bool operator>=(const queue<T, Container>& lhs, const queue<T, Container>& rhs)
{
  return !(lhs < rhs);
}

// 重载 mystl 的 swap
template <class T, class Container>
void swap(queue<T, Container>& lhs, queue<T, Container>& rhs) noexcept(noexcept(lhs.swap(rhs)))
{
  lhs.swap(rhs);
}

/*****************************************************************************************/


} // namespace mystl
#endif // !MYTINYSTL_QUEUE_H_

