#ifndef TINYSTL_MEMORY_H_
#define TINYSTL_MEMORY_H_

#include <cstddef>
#include <cstdlib>
#include <climits>

#include "algobase.h"
#include "allocator.h"
#include "construct.h"
#include "uninitialized.h"

namespace mystl{

// 获取对象地址
template <class Tp>
constexpr Tp* address_of(Tp& value) noexcept
{
  return &value;
}

// 获取 / 释放 临时缓冲区

template <class T>
pair<T*, ptrdiff_t> get_buffer_helper(ptrdiff_t len, T*)
{
    //使用malloc分配一个大小为len * sizeof(T)的缓冲区
  if (len > static_cast<ptrdiff_t>(INT_MAX / sizeof(T))){
    //如果超出这个大小,则长度定为INT_MAX / sizeof(T)
    len = INT_MAX / sizeof(T);
  }
    
  while (len > 0)
  {
    //当len合法时,使用malloc尝试进行内存分配
    T* tmp = static_cast<T*>(malloc(static_cast<size_t>(len) * sizeof(T)));
    if (tmp)
        //成功则返回缓冲区指针和缓冲区大小
      return pair<T*, ptrdiff_t>(tmp, len);
    len /= 2;  // 申请失败时减少 len 的大小,每次减半
  }
  //若失败的话返回nullptr和0
  return pair<T*, ptrdiff_t>(nullptr, 0);
}

//使用空指针进行空间分配
template <class T>
pair<T*, ptrdiff_t> get_temporary_buffer(ptrdiff_t len)
{
  return get_buffer_helper(len, static_cast<T*>(0));
}
//用指定类型的指针来进行空间分配
template <class T>
pair<T*, ptrdiff_t> get_temporary_buffer(ptrdiff_t len, T*)
{
  return get_buffer_helper(len, static_cast<T*>(0));
}

//释放
template <class T>
void release_temporary_buffer(T* ptr)
{
  free(ptr);
}

// 进行临时缓冲区的申请与释放
template <class ForwardIterator, class T>
class temporary_buffer
{
private:
  ptrdiff_t original_len;  // 缓冲区申请的大小
  ptrdiff_t len;           // 缓冲区实际的大小
  T*        buffer;        // 指向缓冲区的指针

public:
  // 构造、析构函数
  temporary_buffer(ForwardIterator first, ForwardIterator last);

  ~temporary_buffer()
  {
    //调用destory销毁
    mystl::destroy(buffer, buffer + len);
    free(buffer);
  }

public:

    //不抛出异常的四个只读函数
  ptrdiff_t size()           const noexcept { return len; }
  ptrdiff_t requested_size() const noexcept { return original_len; }
  T*        begin()                noexcept { return buffer; }
  T*        end()                  noexcept { return buffer + len; }

private:
  void allocate_buffer();
  //如果一个对象的构造函数是平凡的，那么在内存分配后立即使用这些对象是安全的
  //，即使它们没有被显式初始化。这利用了C++对平凡构造函数和POD类型的特殊规则，
  //使得在某些情况下可以避免初始化开销，而不会影响程序的正确性。
  //简单的说就是, 一个对象的构造函数是平凡的话,那么就不要在分配空间之后对他进行显式的初始化
  //而如果是非平凡的,那么就必须使用uninitialized方法进行显式的构造
  void initialize_buffer(const T&, std::true_type) {}
  void initialize_buffer(const T& value, std::false_type)
  { mystl::uninitialized_fill_n(buffer, len, value); }

private:
  temporary_buffer(const temporary_buffer&);
  void operator=(const temporary_buffer&);
};

// 构造函数
//传入first和last两个指针,计算得出len长度,初始化缓冲区的len
//使用allocate_buffer分配len长度的空间,单位大小由模板参数T来提供,所以是sizeof(T)
//然后用first指向的内容完成对新申请的缓冲区空间的初始化
template <class ForwardIterator, class T>
temporary_buffer<ForwardIterator, T>::
temporary_buffer(ForwardIterator first, ForwardIterator last)
{
  try
  {
    len = mystl::distance(first, last);
    allocate_buffer();
    if (len > 0)
    {
      initialize_buffer(*first, std::is_trivially_default_constructible<T>());
    }
  }
  catch (...)
  {
    free(buffer);
    buffer = nullptr;
    len = 0;
  }
}

//实现original_len, len, buffer的初始化
template <class ForwardIterator, class T>
void temporary_buffer<ForwardIterator, T>::allocate_buffer()
{
  original_len = len;
  if (len > static_cast<ptrdiff_t>(INT_MAX / sizeof(T)))
    len = INT_MAX / sizeof(T);
  while (len > 0)
  {
    buffer = static_cast<T*>(malloc(len * sizeof(T)));
    if (buffer)
      break;
    len /= 2;  // 申请失败时减少申请空间大小
  }
}



// 一个具有严格对象所有权的小型智能指针,重点在严格的所有权上
//集中体现在其中的release函数上,构建新指针的时候使用release函数将原函数的所有权放弃,然后将所有权转移到新指针上
//保证一个对象只能由一个指针所有
template <class T>
class auto_ptr
{
public:
  typedef T    elem_type;

private:
  T* m_ptr;  // 实际指针

public:
  // 构造、复制、析构函数
  explicit auto_ptr(T* p = nullptr) :m_ptr(p) {}
  auto_ptr(auto_ptr& rhs) :m_ptr(rhs.release()) {}
  template <class U>
  auto_ptr(auto_ptr<U>& rhs) : m_ptr(rhs.release()) {}

  auto_ptr& operator=(auto_ptr& rhs)
  {
    if (this != &rhs)
    {
      delete m_ptr;
      m_ptr = rhs.release();
    }
    return *this;
  }
  template <class U>
  auto_ptr& operator=(auto_ptr<U>& rhs)
  {
    if (this->get() != rhs.get())
    {
      delete m_ptr;
      m_ptr = rhs.release();
    }
    return *this;
  }

  ~auto_ptr() { delete m_ptr; }

public:
  // 重载 operator* 和 operator->
  T& operator*()  const { return *m_ptr; }
  T* operator->() const { return m_ptr; }

  // 获得指针
  T* get() const { return m_ptr; }

  // 释放指针,体现所有权严格,使用本指针this初始化其他的指针的时候要使用this.release作为参数
  //需要将原指针置空,也就是原指针需要放弃对指向对象的权限,然后用tmp初始化新的指针,将权限赋予新的指针
  //
  T* release()
  {
    T* tmp = m_ptr;
    m_ptr = nullptr;
    return tmp;
  }

  // 重置指针
  void reset(T* p = nullptr)
  {
    if (m_ptr != p)
    {
      delete m_ptr;
      m_ptr = p;
    }
  }
};


    
}
#endif