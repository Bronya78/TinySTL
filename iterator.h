#ifndef TINYSTL_ITERATOR_H
#define TINYSTL_ITERATOR_H


#include<cstddef>
#include"type_traits.h"


namespace mystl{

//五种迭代器类型,只做区分没有实际内容,作为ddq的分类依据
struct input_iterator_tag{};
struct output_iterator_tag{};
struct forward_iterator_tag:public input_iterator_tag{};
struct bidirectional_iterator_tag:public forward_iterator_tag{};
struct random_access_iterator_tag:public bidirectional_iterator_tag{};

//iterator的基础模板,自定义的迭代器必须继承这个基础模板
template <class Category, class T, class Distance = ptrdiff_t,
  class Pointer = T*, class Reference = T&>
  struct iterator
{
  typedef Category                             iterator_category;
  typedef T                                    value_type;
  typedef Pointer                              pointer;
  typedef Reference                            reference;
  typedef Distance                             difference_type;
};


//迭代器的萃取器
//这个结构体用于辅助判断迭代器内部是否有内嵌的iterator_category类型
template <class T>
struct has_iterator_cat
{
private:
  struct two { char a; char b; };
  //接收任意参数,返回two
  template <class U> static two test(...);
  //接收一个指向 U::iterator_category 类型的指针作为参数,返回一个char类型
  template <class U> static char test(typename U::iterator_category* = 0);
public:
    //如果返回值的大小等于char的大小,那么说明迭代器有内嵌的iterator_category类型,返回true
    //这个处理方式通过返回值的类型大小来区别两种情况
  static const bool value = sizeof(test<T>(0)) == sizeof(char);
};

//常用的方式,首先定义一个基础版本的模板
template<class Iterator,bool>
struct iterator_traits_impl{};
//然后设计一个偏向版本,此处偏向于传入参数为true的情况
//可以用来在具体的迭代器类型上实例化 iterator_traits_impl 结构体模板，并从中获取迭代器的特性类型别名。
template<class Iterator>
struct iterator_traits_impl<Iterator,true>{
typedef typename Iterator::iterator_category itetator_category;
typedef typename Iterator::value_type value_type;
typedef typename Iterator::pointer pointer;
typedef typename Iterator::reference reference;
typedef typename Iterator::difference_type   difference_type;
};


//基础版本
template<class Iterator,bool>
struct iterator_traits_helper{};


//如果输入的是可以隐式转化为Iterator::iterator_category的input_iterator_tag或者output_iterator_tag
//那么这个表达式值为true
//则调用上述的iterator_traits_impl<Iterator,true>
template<class Iterator>
struct iterator_traits_helper<Iterator,true>
    :public iterator_traits_impl<Iterator,
    std::is_convertible<typename Iterator::iterator_category, input_iterator_tag>::value ||
    std::is_convertible<typename Iterator::iterator_category, output_iterator_tag>::value>
    {};


//萃取迭代器的特性
//第一级调用 has_iterator_cat先判断是否存在category
//如果存在则调用iterator_traits_helper
template<class Iterator>
struct iterator_traits
    :public iterator_traits_helper<Iterator, has_iterator_cat<Iterator>::value>{};


//由于原生指针并不是class,所有需要对原生指针进行特殊的偏特化处理
//对T*的特化
template<class T>
struct iterator_traits<T*>
{
    typedef random_access_iterator_tag          iterator_type;
    typedef T                                   value_type;
    typedef T*                                  pointer;
    typedef T&                                  reference;
    typedef ptrdiff_t                           difference_type;
};
//对constT*的特化
template <class T>
struct iterator_traits<const T*>
{
  typedef random_access_iterator_tag           iterator_category;
  typedef T                                    value_type;
  typedef const T*                             pointer;
  typedef const T&                             reference;
  typedef ptrdiff_t                            difference_type;
};

//用于检查U是否可以转化为T
//bool = has_iterator_cat<iterator_traits<T>>::value：一个默认模板参数，用于检查类型T是否有迭代器类别
//用std::is_convertible<typename iterator_traits<T>::iterator_category, U>::value,检查iterator_category T是否可以隐式转化为U
//public继承m_bool_constant,是为了直接利用它的结果类型
//综合来看,如果T存在iterator_category,那么就继承m_bool_constant,值为T是否可以隐式转化为U
template <class T, class U, bool = has_iterator_cat<iterator_traits<T>>::value>
struct has_iterator_cat_of
  : public m_bool_constant<std::is_convertible<
  typename iterator_traits<T>::iterator_category, U>::value>
{
};
//如果T不存在iterator_category,那么继承 m_false_type=false
template <class T, class U>
struct has_iterator_cat_of<T, U, false> : public m_false_type {};


//萃取迭代器
//input_iterator举例
//若传入的Iter,具有category,那么检查iterator_traits<T>::iterator_category,是否可以隐式转化为input_iterator
//继承获得上述表达式的结果,真或假,表示是否为input_iterator
template <class Iter>
struct is_input_iterator : public has_iterator_cat_of<Iter, input_iterator_tag> {};

template <class Iter>
struct is_output_iterator : public has_iterator_cat_of<Iter, output_iterator_tag> {};

template <class Iter>
struct is_forward_iterator : public has_iterator_cat_of<Iter, forward_iterator_tag> {};

template <class Iter>
struct is_bidirectional_iterator : public has_iterator_cat_of<Iter, bidirectional_iterator_tag> {};

template <class Iter>
struct is_random_access_iterator : public has_iterator_cat_of<Iter, random_access_iterator_tag> {};


//外层封装
//只有input_iterator和output_iterator是不同的,其余的迭代器都是这两个的派生
template <class Iterator>
struct is_iterator :
  public m_bool_constant<is_input_iterator<Iterator>::value ||
    is_output_iterator<Iterator>::value>
{};

//获取迭代器的category
template<class Iterator>
inline typename iterator_traits<Iterator>::iterator_category
  iterator_category(const Iterator &){
    typedef typename iterator_traits<Iterator>::iterator_category category;
    return category();
  }

//获取迭代器的distance_type
template <class Iterator>
inline typename iterator_traits<Iterator>::difference_type*
distance_type(const Iterator&)
{
  return static_cast<typename iterator_traits<Iterator>::difference_type*>(0);
}


//获取迭代器的value_type
template <class Iterator>
inline typename iterator_traits<Iterator>::value_type*
value_type(const Iterator&)
{
  return static_cast<typename iterator_traits<Iterator>::value_type*>(0);
}


//计算distance
//input_iterator版本
template<class InputIterator>
inline  typename iterator_traits<InputIterator>::difference_type
_distance(InputIterator first,InputIterator last,input_iterator_tag){
    iterator_traits<InputIterator>::difference_type n=0;
    while(first!=last){
      ++first;
      ++n;
    }
    return n;
}
//random_access_iterator版本
template<class RandomAccessIterator>
inline typename iterator_traits<RandomAccessIterator>::difference_type
_distance(RandomAccessIterator first,RandomAccessIterator last,random_access_iterator_tag){
  return last-first;
}

//上层封装

template<class InputIterator>
inline typename iterator_traits<InputIterator>::difference_type
distance(InputIterator first,InputIterator last,input_iterator_tag){
  typedef typename iterator_traits<InputIterator>::iterator_category category;
  return _distance(first,last,category());

}


//advance的input_iterator版本
template <class InputIterator, class Distance>
void advance_dispatch(InputIterator& i, Distance n, input_iterator_tag)
{
  while (n--) 
    ++i;
}

// advance 的 bidirectional_iterator_tag 的版本
template <class BidirectionalIterator, class Distance>
void advance_dispatch(BidirectionalIterator& i, Distance n, bidirectional_iterator_tag)
{
  if (n >= 0)
    while (n--)  ++i;
  else
    while (n++)  --i;
}

// advance 的 random_access_iterator_tag 的版本
template <class RandomIter, class Distance>
void advance_dispatch(RandomIter& i, Distance n, random_access_iterator_tag)
{
  i += n;
}

template <class InputIterator, class Distance>
void advance(InputIterator& i, Distance n)
{
  advance_dispatch(i, n, iterator_category(i));
}


//反向迭代器
template<class Iterator>
class reverse_iterator{
private: 
    Iterator current;//记录对应的正向迭代器的位置
public:
    //反向迭代器的五种型别
    typedef typename iterator_traits<Iterator>::iterator_category  iterator_category;
    typedef typename iterator_traits<Iterator>::value_type        value_type;
    typedef typename iterator_traits<Iterator>::difference_type   difference_type;
    typedef typename iterator_traits<Iterator>::pointer           pointer;
    typedef typename iterator_traits<Iterator>::reference         reference;

    typedef Iterator          iterator_type;
    typedef reverse_iterator<Iterator>  self;

public:
    //构造函数
    reverse_iterator() {}
    explicit reverse_iterator(iterator_type i) :current(i) {}
    reverse_iterator(const self& rhs) :current(rhs.current) {}

public:
    iterator_type base() const{
      return current;
    }
    //解引用重载,对应的是正向迭代器的前一个位置
    reference operator*() const{
      auto tmp=current;
      return *(--tmp);
    }

    pointer operator->() const
  {
    return &(operator*());
  }

  // 前进(++)变为后退(--)
  //前缀
  self& operator++()
  {
    --current;
    return *this;
  }
  //后缀
  self operator++(int)
  {
    self tmp = *this;
    --current;
    return tmp;
  }
  // 后退(--)变为前进(++)
  //前缀
  self& operator--()
  {
    ++current;
    return *this;
  }
  //后缀
  self operator--(int)
  {
    self tmp = *this;
    ++current;
    return tmp;
  }

  self& operator+=(difference_type n)
  {
    current -= n;
    return *this;
  }
  self operator+(difference_type n) const
  {
    return self(current - n);
  }
  self& operator-=(difference_type n)
  {
    current += n;
    return *this;
  }
  self operator-(difference_type n) const
  {
    return self(current + n);
  }

  //重载operator[]
  reference operator[](difference_type n) const
  {
    return *(*this + n);
  }

  //重载比较
  template <class Iterator>
  bool operator==(const reverse_iterator<Iterator>& lhs,
                const reverse_iterator<Iterator>& rhs)
  {
    return lhs.base() == rhs.base();
  }

  template <class Iterator>
  bool operator<(const reverse_iterator<Iterator>& lhs,
  const reverse_iterator<Iterator>& rhs)
  {
    //反过来
    return rhs.base() < lhs.base();
  }

  template <class Iterator>
  bool operator!=(const reverse_iterator<Iterator>& lhs,
                const reverse_iterator<Iterator>& rhs)
  {
    return !(lhs == rhs);
  }

  template <class Iterator>
  bool operator>(const reverse_iterator<Iterator>& lhs,
               const reverse_iterator<Iterator>& rhs)
  {
    //反过来
    return rhs < lhs;
  }

  template <class Iterator>
  bool operator<=(const reverse_iterator<Iterator>& lhs,
                const reverse_iterator<Iterator>& rhs)
  {
    //反过来
    return !(rhs < lhs);
  }

  template <class Iterator>
  bool operator>=(const reverse_iterator<Iterator>& lhs,
                const reverse_iterator<Iterator>& rhs)
  {
    //反过来
    return !(lhs < rhs);
  }

  
};

}

#endif