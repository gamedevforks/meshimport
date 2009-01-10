#ifndef MEMORY_CONTAINER_H
#define MEMORY_CONTAINER_H

//-ignore_file      Do not allow SCANCPP to process this file!

#pragma warning(disable:4702)

#if defined(WIN32)
#include <xstddef>
#elif defined(LINUX)
using std::type_info;
#define _DESTRUCTOR(ty, ptr)    (ptr)->~ty()
#define _CRTIMP2
#endif

#include <typeinfo>
#include <iosfwd>
#include <vector>
#include <list>

class MemoryPoolClassDefinition
{
public:
  MemoryPoolClassDefinition(const char * supplied_name, const type_info& typeid_pointer)
    : m_name(supplied_name)
    , m_typeinfo(&typeid_pointer)
  {
  }

  const char * name(void) const { return m_name; }
  const char * formal_name(void) const { return m_typeinfo->name(); }

private: // undefined, noncopyable
  const char * const m_name;
  const type_info* const m_typeinfo;

  MemoryPoolClassDefinition(const MemoryPoolClassDefinition&);
  MemoryPoolClassDefinition& operator=(const MemoryPoolClassDefinition&);
};


namespace MemoryContainer
{

  class MemoryPool_Private
  {
  public:
    struct MemoryInfo
    {
      const char *pool_name;
      size_t in_use;
    };
    typedef std::vector<MemoryInfo> MemoryInfoVector;
    typedef const MemoryPoolClassDefinition * const POOL_NUMBER;
    static void* OperatorNew(POOL_NUMBER, size_t,const char *className,const char *file,int lineno);
    static void  OperatorDelete(POOL_NUMBER, void*);
    static void* OperatorNewArray(POOL_NUMBER, size_t,const char *className,const char *file,int lineno);
    static void  OperatorDeleteArray(POOL_NUMBER, void*);
  };
}

//
// the net effect of these macros is to declare specialized operator new/delete (scalar and array, placement and the usual)
// to track object allocations.
//
// in Class.h declaration area:
//
// class X { ...
//   DEFINE_MEMORYPOOL_IN_CLASS(X); // creates a public operator new (etc)
//   DEFINE_MEMORYPOOL_IN_CLASS_SECTION(protected,X); // creates it in the protected section
// ... };
// and then in the implementation of X, or pretty much anywhere (once), put
// IMPLEMENT_MEMORYPOOL_IN_CLASS(X);
//
//

//
//class XXX {
//  DEFINE_MEMORYPOOL_IN_CLASS(XXX);
//public:
//  ...
//};
//IMPLEMENT_MEMORYPOOL_IN_CLASS(XXX);
//

//#define DISABLE_MEMORYPOOL_TRACKING
#undef DISABLE_MEMORYPOOL_TRACKING

#if !defined(DISABLE_MEMORYPOOL_TRACKING)

#if defined(new)
#error HEY, someone #define'd new, and that's not a good thing.  Fix it.  (Probably BugSlayer)
#endif
#if defined(malloc)
#error HEY, someone #define'd malloc, and that's not a good thing.  Fix it.
#endif
#if defined(realloc)
#error HEY, someone #define'd realloc, and that's not a good thing.  Fix it.
#endif
#if defined(free)
#error HEY, someone #define'd free, and that's not a good thing.  Fix it.
#endif

#define IMPLEMENT_MEMORYPOOL_IN_CLASS(className) const MemoryPoolClassDefinition className::MemoryPoolForClass(#className, typeid(className))

#define DEFINE_MEMORYPOOL_IN_OTHER_CLASS_SECTION(sectionType,className) \
sectionType: \
  static void * operator new    (size_t s) { return MemoryContainer::MemoryPool_Private::OperatorNew(&className::MemoryPoolForClass, s,#className,__FILE__,__LINE__); } \
  static void operator delete   (void* p) { return MemoryContainer::MemoryPool_Private::OperatorDelete(&className::MemoryPoolForClass, p); } \
  static void * operator new[]  (size_t s) { return MemoryContainer::MemoryPool_Private::OperatorNewArray(&className::MemoryPoolForClass, s,#className,__FILE__,__LINE__); } \
  static void operator delete[] (void* p) { return MemoryContainer::MemoryPool_Private::OperatorDeleteArray(&className::MemoryPoolForClass, p); } \
  static void * operator new(size_t, void* p) { return p; } \
  static void   operator delete(void *, void *) {} \
  static void * operator new[](size_t, void* p) { return p; } \
  static void   operator delete[](void *, void *) {} \
	typedef className object_pool_type;

#define DEFINE_MEMORYPOOL_IN_CLASS_SECTION(sectionType,className) \
public: \
  static const MemoryPoolClassDefinition MemoryPoolForClass; \
  DEFINE_MEMORYPOOL_IN_OTHER_CLASS_SECTION(sectionType,className)

#define DEFINE_MEMORYPOOL_IN_CLASS(className) DEFINE_MEMORYPOOL_IN_CLASS_SECTION(public,className)
#define DEFINE_MEMORYPOOL_IN_OTHER_CLASS(className) DEFINE_MEMORYPOOL_IN_OTHER_CLASS_SECTION(public,className)


#else
#define IMPLEMENT_MEMORYPOOL_IN_CLASS(className)
#define DEFINE_MEMORYPOOL_IN_CLASS_SECTION(a,b)
#define DEFINE_MEMORYPOOL_IN_CLASS(a)
#define DEFINE_MEMORYPOOL_IN_OTHER_CLASS(a)
#endif


namespace MemoryContainer {
  class GlobalMemoryPool {
    DEFINE_MEMORYPOOL_IN_CLASS(GlobalMemoryPool);
  };
}

namespace MemoryContainer {
  void* malloc(size_t s, const MemoryPoolClassDefinition& pool,const char *file,int lineno);
  void  free(void * p, const MemoryPoolClassDefinition& pool);
  void* realloc(void* p, size_t s, const MemoryPoolClassDefinition& pool);
  void* malloc_aligned(size_t alignment, size_t s, const MemoryPoolClassDefinition& pool,const char *file,int lineno);
  void* mallocSTL(size_t s, const MemoryPoolClassDefinition& pool);
  void  freeSTL(void * p, const MemoryPoolClassDefinition& pool);
  void* reallocSTL(void* p, size_t s, const MemoryPoolClassDefinition& pool);
}

#include <vector>
#include <set>
#include <map>
#if defined(WIN32)
#include <hash_set>
#include <hash_map>
#else
#ifndef ORA_PROC
#include <ext/hash_set>
#include <ext/hash_map>
#endif
#endif
#include <deque>
#include <stack>
#include <queue>
#include <string>

namespace MemoryContainer {
  // TEMPLATE FUNCTION _Allocate
  template<class _Ty> inline
    _Ty  *Simu_Allocate(size_t _Count, _Ty  *, MemoryPool_Private::POOL_NUMBER pool)
  {	// allocate storage for _Count elements of type _Ty
    return ((_Ty  *)MemoryContainer::mallocSTL(_Count * sizeof (_Ty), *pool));
  }

  template <class _Ty> inline
    void Simu_Deallocate(_Ty * _Ptr, MemoryPool_Private::POOL_NUMBER pool)
  {
    return MemoryContainer::freeSTL(_Ptr, *pool);
  }

  // TEMPLATE FUNCTION _Construct
  template<class _T1,
  class _T2> inline
    void Simu_Construct(_T1  *_Ptr, const _T2& _Val)
  {	// construct object at _Ptr with value _Val
    new ((void  *)_Ptr) _T1(_Val);
  }

  // TEMPLATE FUNCTION _Destroy
  template<class _Ty> inline
    void Simu_Destroy(_Ty  *_Ptr)
  {	// destroy object at _Ptr
    _DESTRUCTOR(_Ty, _Ptr);
    _Ptr;
  }

  template<> inline
    void Simu_Destroy(char  *)
  {	// destroy a char (do nothing)
  }

  template<> inline
    void Simu_Destroy(wchar_t  *)
  {	// destroy a wchar_t (do nothing)
  }


  // TEMPLATE CLASS _Allocator_base
  template<class _Ty>
  struct _Allocator_base
  {	// base class for generic allocators
    typedef _Ty value_type;
  };

  // TEMPLATE CLASS _Allocator_base<const _Ty>
  template<class _Ty>
  struct _Allocator_base<const _Ty>
  {	// base class for generic allocators for const _Ty
    typedef _Ty value_type;
  };

  // TEMPLATE CLASS allocator
  template<class _Ty, class _Class>
  class simu_allocator
    : public _Allocator_base<_Ty>
  {	// pooled allocator for objects of class _Ty
  public:
    const MemoryPoolClassDefinition* const m_Pool;
  public:
    typedef _Allocator_base<_Ty> _Mybase;
    //lint -e1516
    typedef typename _Mybase::value_type value_type;

    typedef value_type  *pointer;
    typedef value_type & reference;
    typedef const value_type  *const_pointer;
    typedef const value_type & const_reference;

    typedef size_t size_type;
    typedef ptrdiff_t difference_type;

    template<class _Other>
    struct rebind
    {	// convert an allocator<_Ty> to an allocator <_Other>
      typedef simu_allocator<_Other, _Class> other;
    };

    pointer address(reference _Val) const
    {	// return address of mutable _Val
      return (&_Val);
    }

    const_pointer address(const_reference _Val) const
    {	// return address of nonmutable _Val
      return (&_Val);
    }

    simu_allocator()
      : m_Pool(& _Class ::MemoryPoolForClass )
    {	// construct default allocator 
    }

    simu_allocator(const simu_allocator<_Ty, _Class>& to_copy)
      : m_Pool(to_copy.m_Pool)
    {	// construct by copying 
    }

    simu_allocator& operator=(const simu_allocator<_Ty, _Class>& to_copy)
    {
      m_Pool = to_copy.m_Pool;
    }

    template<class _Other>
      simu_allocator(const simu_allocator<_Other, _Class>& to_copy)
      : m_Pool(to_copy.m_Pool)
    {	// construct from a related allocator 
    }

    template<class _Other>
      simu_allocator<_Ty, _Class>& operator=(const simu_allocator<_Other, _Class>& to_copy)
    {	// assign from a related allocator 
      m_Pool(to_copy.m_Pool);
      return (*this);
    }

    void deallocate(pointer _Ptr, size_type)
    {	// deallocate object at _Ptr, ignore size
      return (Simu_Deallocate(_Ptr, m_Pool));
    }

    pointer allocate(size_type _Count)
    {	// allocate array of _Count elements
      return (Simu_Allocate(_Count, (pointer)0, m_Pool));
    }

    pointer allocate(size_type _Count, const void  *)
    {	// allocate array of _Count elements, ignore hint
      return (allocate(_Count));
    }

    void construct(pointer _Ptr, const _Ty& _Val)
    {	// construct object at _Ptr with value _Val
      Simu_Construct(_Ptr, _Val);
    }

    void destroy(pointer _Ptr)
    {	// destroy object at _Ptr
      Simu_Destroy(_Ptr);
    }

    size_t max_size() const
    {	// estimate maximum array size
      size_t _Count = (size_t)(-1) / sizeof (_Ty);
      return (0 < _Count ? _Count : 1);
    }
  };

  // allocator TEMPLATE OPERATORS
  template<class _Ty,
  class _Other, class _Class> inline
    bool operator==(const simu_allocator<_Ty, _Class>&, const simu_allocator<_Other, _Class>&)
  {	// test for allocator equality (always true)
    return (true);
  }

  template<class _Ty,
  class _Other, class _Class> inline
    bool operator!=(const simu_allocator<_Ty, _Class>&, const simu_allocator<_Other, _Class>&)
  {	// test for allocator inequality (always false)
    return (false);
  }

  // CLASS allocator<void>
  template<class _Class> class _CRTIMP2 simu_allocator<void, _Class>
  {	// generic allocator for type void
  public:
    typedef void _Ty;
    typedef _Ty  *pointer;
    typedef const _Ty  *const_pointer;
    typedef _Ty value_type;

    template<class _Other>
    struct rebind
    {	// convert an allocator<void> to an allocator <_Other>
      typedef simu_allocator<_Other, _Class> other;
    };

    simu_allocator()
    {	// construct default allocator (do nothing)
    }

    simu_allocator(const simu_allocator<_Ty, _Class>&)
    {	// construct by copying (do nothing)
    }

    template<class _Other>
      simu_allocator(const simu_allocator<_Other, _Class>&)
    {	// construct from related allocator (do nothing)
    }

    template<class _Other>
      simu_allocator<_Ty, _Class>& operator=(const simu_allocator<_Other, _Class>&)
    {	// assign from a related allocator (do nothing)
      return (*this);
    }
  };

  // TEMPLATE FUNCTION _Destroy_range
  template<class _Ty,
  class _Alloc> inline
    void Simu_Destroy_range(_Ty *_First, _Ty *_Last, _Alloc& _Al)
  {	// destroy [_First, _Last)
    Simu_Destroy_range(_First, _Last, _Al, _Ptr_cat(_First, _Last));
  }

  struct _Nonscalar_ptr_iterator_tag
  {	// pointer to unknown type
  };
  struct _Scalar_ptr_iterator_tag
  {	// pointer to scalar type
  };

  template<class _Ty,
  class _Alloc> inline
    void Simu_Destroy_range(_Ty *_First, _Ty *_Last, _Alloc& _Al,
    _Nonscalar_ptr_iterator_tag)
  {	// destroy [_First, _Last), arbitrary type
    for (; _First != _Last; ++_First)
      _Al.destroy(_First);
  }

  template<class _Ty,
  class _Alloc> inline
    void Simu_Destroy_range(_Ty *_First, _Ty *_Last, _Alloc& _Al,
    _Scalar_ptr_iterator_tag)
  {	// destroy [_First, _Last), scalar type (do nothing)
  }

  template<typename T, class _Class = GlobalMemoryPool>
  class list : public std::list<T
    , MemoryContainer::simu_allocator<T, _Class>
  >
  {
  };

  // TODO: If I correctly understand what's going on here, we need a way to allow strings to use pools other than
  // the global pool.
  typedef std::basic_string<char, std::char_traits<char>
    , MemoryContainer::simu_allocator<char, GlobalMemoryPool>
  > string;

  // TODO: If I correctly understand what's going on here, we need a way to allow strings to use pools other than
  // the global pool.
  typedef std::basic_string<wchar_t, std::char_traits<wchar_t>
    , MemoryContainer::simu_allocator<wchar_t, GlobalMemoryPool>
  > wstring;

  // TODO: when we know the source of the uninitialized_copy weirdity, put the vectors back into the memory pooling
  template <typename T, class _Class = GlobalMemoryPool>
  class vector: public std::vector<T
    , MemoryContainer::simu_allocator<T, _Class>
  >
  {
  public:
    typedef size_t size_type;
    vector() :   std::vector<T
      , MemoryContainer::simu_allocator<T, _Class>
    >() { }

    template <typename ITER>
    vector(ITER start, ITER end): std::vector<T
      , MemoryContainer::simu_allocator<T, _Class>
    > (start, end) { }

    vector(size_type num) : std::vector<T
      , MemoryContainer::simu_allocator<T, _Class>
    > (num) { }

		vector(size_type num, const T& val) : std::vector<T
			, MemoryContainer::simu_allocator<T, _Class>
		> (num, val) { }
  };

  template <typename T, class _Class = GlobalMemoryPool>
  class deque: public std::deque<T
    , MemoryContainer::simu_allocator<T, _Class>
  >
  {
  };

  template <typename T, class _Class = GlobalMemoryPool, class L = std::less<T> >
  class set: public std::set<T, L
    , MemoryContainer::simu_allocator<T, _Class>
  >
  {
  };

  template <typename T, class _Class = GlobalMemoryPool, class L = std::less<T> >
  class multiset: public std::multiset<T, L
    , MemoryContainer::simu_allocator<T, _Class>
  >
  {
  };

  template <typename T, class _Class = GlobalMemoryPool, typename _Container = MemoryContainer::deque<T,_Class> >
  class stack: public std::stack<T, _Container>
  {
  };

  template <typename T, class _Class = GlobalMemoryPool, class _Container = MemoryContainer::deque<T,_Class> >
  class queue: public std::queue<T, _Container>
  {
  };

  template <typename TK, typename TV, class _Class = GlobalMemoryPool, class L = std::less<TK> >
  class map: public std::map<TK, TV, L
    , MemoryContainer::simu_allocator< std::pair< TK, TV>, _Class >
  >
  {
  };

  template <typename TK, typename TV, class _Class = GlobalMemoryPool, class L = std::less<TK> >
  class multimap: public std::multimap<TK, TV, L
    , MemoryContainer::simu_allocator< std::pair< TK, TV>, _Class >
  >
  {
  };

  ////////////// BEGIN WIN32_SPECIFIC CODE //////////////
#if defined(WIN32)
  // TEMPLATE CLASS hash_compare

  template<class _Kty, int _Bucket_size = 4, int _Min_buckets = 8,
  class _Pr = _STD less<_Kty> >
  class configured_hash_compare
  {	// traits class for hash containers
  public:
	  enum
	  {	// parameters for hash table
		  bucket_size = 4,	// 0 < bucket_size
		  min_buckets = 8};	// min_buckets = 2 ^^ N, 0 < N

		  configured_hash_compare()
			  : comp()
		  {	// construct with default comparator
		  }

		  configured_hash_compare(_Pr _Pred)
			  : comp(_Pred)
		  {	// construct with _Pred comparator
		  }

		  size_t operator()(const _Kty& _Keyval) const
		  {	// hash _Keyval to size_t value
			  return ((size_t)stdext::hash_value(_Keyval));
		  }

		  bool operator()(const _Kty& _Keyval1, const _Kty& _Keyval2) const
		  {	// test if _Keyval1 ordered before _Keyval2
			  return (comp(_Keyval1, _Keyval2));
		  }

  protected:
	  _Pr comp;	// the comparator object
  };

  template <typename TK, typename TV, class _Class = GlobalMemoryPool >
  class hash_map: public stdext::hash_map<TK, TV, stdext::hash_compare<TK>
    , MemoryContainer::simu_allocator< std::pair< TK, TV>, _Class >
  >
  {
  public:
    hash_map()
    {
    }

    template <typename InputIter>
    hash_map(InputIter b, InputIter e)
      : stdext::hash_map<TK, TV, stdext::hash_compare<TK>
      , MemoryContainer::simu_allocator< std::pair< TK, TV>, _Class >
      >(b,e)
    {
    }
  };

  template <typename TK, typename TV, class _Class = GlobalMemoryPool >
  class hash_multimap: public stdext::hash_multimap<TK, TV, stdext::hash_compare<TK>
    , MemoryContainer::simu_allocator< std::pair< TK, TV>, _Class >
  >
  {
  };

  template <typename TK, typename TV, int _Bucket_size = 4, int _Min_buckets = 4096, 
    class _Class = GlobalMemoryPool >
  class large_hash_map: public stdext::hash_map<TK, TV, MemoryContainer::configured_hash_compare<TK, _Bucket_size, _Min_buckets>
    , MemoryContainer::simu_allocator< std::pair< TK, TV>, _Class >
  >
  {
  };

  template <typename TK, typename TV, int _Bucket_size = 4, int _Min_buckets = 4096, 
    class _Class = GlobalMemoryPool >
  class large_hash_multimap: public stdext::hash_multimap<TK, TV, MemoryContainer::configured_hash_compare<TK, _Bucket_size, _Min_buckets>
    , MemoryContainer::simu_allocator< std::pair< TK, TV>, _Class >
  >
  {
  };

  template <typename T, class _Class = GlobalMemoryPool >
  class hash_set: public stdext::hash_set<T, stdext::hash_compare<T>
	  , MemoryContainer::simu_allocator<T, _Class>
  >
  {
  public:
    hash_set()
    {
    }

    template <typename InputIter>
    hash_set(InputIter b, InputIter e)
      : stdext::hash_set<T, stdext::hash_compare<T>
      , MemoryContainer::simu_allocator< T, _Class >
      >(b,e)
    {
    }

  };

  template <typename T, int _Bucket_size = 4, int _Min_buckets = 4096,
  class _Class = GlobalMemoryPool >
  class large_hash_set: public stdext::hash_set<T, MemoryContainer::configured_hash_compare<T, _Bucket_size, _Min_buckets>
	  , MemoryContainer::simu_allocator<T, _Class>
  >
  {
  };
    ////////////// END WIN32_SPECIFIC CODE //////////////
#endif 
}
#if defined(LINUX)
  ////////////// BEGIN LINUX_SPECIFIC CODE //////////////
#include "simuhash_linux.h"
#endif
  ////////////// END LINUX_SPECIFIC CODE //////////////



#endif
