// -*- C++ -*-
/*
 *
 * Copyright (c) 1994
 * Hewlett-Packard Company
 *
 * Permission to use, copy, modify, distribute and sell this software
 * and its documentation for any purpose is hereby granted without fee,
 * provided that the above copyright notice appear in all copies and
 * that both that copyright notice and this permission notice appear
 * in supporting documentation.  Hewlett-Packard Company makes no
 * representations about the suitability of this software for any
 * purpose.  It is provided "as is" without express or implied warranty.
 *
 *
 * Copyright (c) 1996
 * Silicon Graphics Computer Systems, Inc.
 *
 * Permission to use, copy, modify, distribute and sell this software
 * and its documentation for any purpose is hereby granted without fee,
 * provided that the above copyright notice appear in all copies and
 * that both that copyright notice and this permission notice appear
 * in supporting documentation.  Silicon Graphics makes no
 * representations about the suitability of this software for any
 * purpose.  It is provided "as is" without express or implied warranty.
 *
 * Copyright (c) 2005
 * Takashi Tsunakawa, Kenta Oouchida and Takashi Ninomiya
 *
 * Permission to use, copy, modify, distribute and sell this software
 * and its documentation for any purpose is hereby granted without fee,
 * provided that the above copyright notice appear in all copies and
 * that both that copyright notice and this permission notice appear
 * in supporting documentation.  Takashi Tsunakawa, Kenta Oouchida and
 * Takashi Ninomiya make no
 * representations about the suitability of this software for any
 * purpose.  It is provided "as is" without express or implied warranty.
 *
 */

/* NOTE: This is an internal header file, included by other PSTL headers.
 *   You should not attempt to use it directly.
 */

#ifndef __SGI_PSTL_INTERNAL_MULTISET_H
#define __SGI_PSTL_INTERNAL_MULTISET_H

#include "pstl_concept_checks.h"

__PSTL_BEGIN_NAMESPACE

#if defined(__sgi) && !defined(__GNUC__) && (_MIPS_SIM != _MIPS_SIM_ABI32)
#pragma set woff 1174
#pragma set woff 1375
#endif

// Forward declaration of operators < and ==, needed for friend declaration.

template <class _Key, class _Arch, 
	  class _Compare __PSTL_DEPENDENT_DEFAULT_TMPL(less<_Key>),
          class _Alloc = __PSTL_DEFAULT_ALLOCATOR(_Key, _Arch) >
class multiset;

template <class _Key, class _Arch, class _Compare, class _Alloc>
inline bool operator==(const multiset<_Key, _Arch, _Compare, _Alloc> &__x, 
                       const multiset<_Key, _Arch, _Compare, _Alloc> &__y);

template <class _Key, class _Arch, class _Compare, class _Alloc>
inline bool operator<(const multiset<_Key, _Arch, _Compare, _Alloc> &__x, 
                      const multiset<_Key, _Arch, _Compare, _Alloc> &__y);

template <class _Key, class _Arch, class _Compare, class _Alloc>
class multiset {
    // requirements:
  
    __PSTL_CLASS_REQUIRES(_Key, _Assignable);
    __PSTL_CLASS_BINARY_FUNCTION_CHECK(_Compare, bool, _Key, _Key);

public:

    // typedefs:

    typedef _Key     key_type;
    typedef _Key     value_type;
    typedef _Compare key_compare;
    typedef _Compare value_compare;

    typedef _Arch arch_type;
    typedef typename arch_type::dev_type dev_type;
    typedef typename arch_type::bit_type bit_type;
    typedef pheap<arch_type> pheap_type;
    
private:
    typedef _Rb_tree<key_type, value_type, arch_type, 
		     _Identity<value_type>, key_compare, _Alloc> _Rep_type;
    _Rep_type _M_t;  // red-black tree representing multiset
public:
    typedef typename _Rep_type::const_pointer pointer;
    typedef typename _Rep_type::const_pointer const_pointer;
    typedef typename _Rep_type::const_reference value_reference;
    typedef typename _Rep_type::const_reference const_value_reference;
    typedef typename _Rep_type::const_iterator iterator;
    typedef typename _Rep_type::const_iterator const_iterator;
    typedef typename _Rep_type::const_reverse_iterator reverse_iterator;
    typedef typename _Rep_type::const_reverse_iterator const_reverse_iterator;
    typedef typename _Rep_type::size_type size_type;
    typedef typename _Rep_type::difference_type difference_type;
    typedef typename _Rep_type::allocator_type allocator_type;

    typedef ref<multiset, arch_type> reference;
    typedef const_ref<multiset, arch_type> const_reference;
    
    // allocation/deallocation

    multiset() : _M_t(_Compare(), allocator_type()) {}
    explicit multiset(const _Compare &__comp,
		      const allocator_type &__a = allocator_type())
	: _M_t(__comp, __a) {}

#ifdef __PSTL_MEMBER_TEMPLATES

    template <class _InputIterator>
    multiset(_InputIterator __first, _InputIterator __last)
	: _M_t(_Compare(), allocator_type())
	{ _M_t.insert_equal(__first, __last); }

    template <class _InputIterator>
    multiset(_InputIterator __first, _InputIterator __last,
	     const _Compare &__comp,
	     const allocator_type &__a = allocator_type())
	: _M_t(__comp, __a) { _M_t.insert_equal(__first, __last); }

#else

    multiset(const value_type* __first, const value_type* __last)
	: _M_t(_Compare(), allocator_type())
	{ _M_t.insert_equal(__first, __last); }

    multiset(const value_type* __first, const value_type* __last,
	     const _Compare &__comp,
	     const allocator_type &__a = allocator_type())
	: _M_t(__comp, __a) { _M_t.insert_equal(__first, __last); }

    multiset(const_iterator __first, const_iterator __last)
	: _M_t(_Compare(), allocator_type())
	{ _M_t.insert_equal(__first, __last); }

    multiset(const_iterator __first, const_iterator __last,
	     const _Compare &__comp,
	     const allocator_type &__a = allocator_type())
	: _M_t(__comp, __a) { _M_t.insert_equal(__first, __last); }
   
#endif /* __PSTL_MEMBER_TEMPLATES */

    multiset(const multiset<_Key, _Arch, _Compare, _Alloc> &__x) : _M_t(__x._M_t) {}

    // pnew
    static ptr<multiset, arch_type> pnew(const pheap_ptr __ph) {
	ptr<multiset, arch_type> __p(pheap_type::pheap_convert(__ph)->malloc(sizeof(multiset)), __ph);
	ref<multiset, arch_type> r = *__p;
	new ((void *)&(r.get_obj())) multiset();
	return __p;
    }
    static ptr<multiset, arch_type> pnew(const pheap_ptr __ph, const _Compare &__comp) {
	ptr<multiset, arch_type> __p(pheap_type::pheap_convert(__ph)->malloc(sizeof(multiset)), __ph);
	ref<multiset, arch_type> r = *__p;
	new ((void *)&(r.get_obj())) multiset(__comp, allocator_type(__ph));
	return __p;
    }
    static ptr<multiset, arch_type> pnew(const pheap_ptr __ph, const _Compare &__comp, const allocator_type &__a) {
	ptr<multiset, arch_type> __p(pheap_type::pheap_convert(__ph)->malloc(sizeof(multiset)), __ph);
	ref<multiset, arch_type> r = *__p;
	new ((void *)&(r.get_obj())) multiset(__comp, __a);
	return __p;
    }
#ifdef __PSTL_MEMBER_TEMPLATES
    template <class _InputIterator>
    static ptr<multiset, arch_type> pnew(const pheap_ptr __ph, _InputIterator __first, _InputIterator __last) {
	ptr<multiset, arch_type> __p(pheap_type::pheap_convert(__ph)->malloc(sizeof(multiset)), __ph);
	ref<multiset, arch_type> r = *__p;
	new ((void *)&(r.get_obj())) multiset(__first, __last);
	return __p;
    }
    template <class _InputIterator>
    static ptr<multiset, arch_type> pnew(const pheap_ptr __ph, _InputIterator __first, _InputIterator __last, const _Compare &__comp) {
	ptr<multiset, arch_type> __p(pheap_type::pheap_convert(__ph)->malloc(sizeof(multiset)), __ph);
	ref<multiset, arch_type> r = *__p;
	new ((void *)&(r.get_obj())) multiset(__first, __last, __comp, allocator_type(__ph));
	return __p;
    }
    template <class _InputIterator>
    static ptr<multiset, arch_type> pnew(const pheap_ptr __ph, _InputIterator __first, _InputIterator __last, const _Compare &__comp, const allocator_type &__a) {
	ptr<multiset, arch_type> __p(pheap_type::pheap_convert(__ph)->malloc(sizeof(multiset)), __ph);
	ref<multiset, arch_type> r = *__p;
	new ((void *)&(r.get_obj())) multiset(__first, __last, __comp, __a);
	return __p;
    }
#else
    static ptr<multiset, arch_type> pnew(const pheap_ptr __ph, const_ptr<value_type, arch_type> __first, const_ptr<value_type, arch_type> __last) {
	ptr<multiset, arch_type> __p(pheap_type::pheap_convert(__ph)->malloc(sizeof(multiset)), __ph);
	ref<multiset, arch_type> r = *__p;
	new ((void *)&(r.get_obj())) multiset(__first, __last);
	return __p;
    }
    static ptr<multiset, arch_type> pnew(const pheap_ptr __ph, const_ptr<value_type, arch_type> __first, const_ptr<value_type, arch_type> __last, const _Compare &__comp) {
	ptr<multiset, arch_type> __p(pheap_type::pheap_convert(__ph)->malloc(sizeof(multiset)), __ph);
	ref<multiset, arch_type> r = *__p;
	new ((void *)&(r.get_obj())) multiset(__first, __last, __comp, allocator_type(__ph));
	return __p;
    }
    static ptr<multiset, arch_type> pnew(const pheap_ptr __ph, const_ptr<value_type, arch_type> __first, const_ptr<value_type, arch_type> __last, const _Compare &__comp, const allocator_type &__a) {
	ptr<multiset, arch_type> __p(pheap_type::pheap_convert(__ph)->malloc(sizeof(multiset)), __ph);
	ref<multiset, arch_type> r = *__p;
	new ((void *)&(r.get_obj())) multiset(__first, __last, __comp, __a);
	return __p;
    }
    static ptr<multiset, arch_type> pnew(const pheap_ptr __ph, const_iterator __first, const_iterator __last) {
	ptr<multiset, arch_type> __p(pheap_type::pheap_convert(__ph)->malloc(sizeof(multiset)), __ph);
	ref<multiset, arch_type> r = *__p;
	new ((void *)&(r.get_obj())) multiset(__first, __last);
	return __p;
    }
    static ptr<multiset, arch_type> pnew(const pheap_ptr __ph, const_iterator __first, const_iterator __last, const _Compare &__comp) {
	ptr<multiset, arch_type> __p(pheap_type::pheap_convert(__ph)->malloc(sizeof(multiset)), __ph);
	ref<multiset, arch_type> r = *__p;
	new ((void *)&(r.get_obj())) multiset(__first, __last, __comp, allocator_type(__ph));
	return __p;
    }
    static ptr<multiset, arch_type> pnew(const pheap_ptr __ph, const_iterator __first, const_iterator __last, const _Compare &__comp, const allocator_type &__a) {
	ptr<multiset, arch_type> __p(pheap_type::pheap_convert(__ph)->malloc(sizeof(multiset)), __ph);
	ref<multiset, arch_type> r = *__p;
	new ((void *)&(r.get_obj())) multiset(__first, __last, __comp, __a);
	return __p;
    }

#endif /* __PSTL_MEMBER_TEMPLATES */

    static ptr<multiset, arch_type> pnew(const pheap_ptr __ph, const multiset &__x) {
       	ptr<multiset, arch_type> __p(pheap_type::pheap_convert(__ph)->malloc(sizeof(multiset)), __ph);
        ref<multiset, arch_type> r = *__p;
        new ((void *)&(r.get_obj())) multiset(__x);
	return __p;
    }
    
    multiset<_Key, _Arch, _Compare, _Alloc> &
    operator=(const multiset<_Key, _Arch, _Compare, _Alloc> &__x) {
	_M_t = __x._M_t; 
	return *this;
    }

    // accessors:

    key_compare key_comp() const { return _M_t.key_comp(); }
    value_compare value_comp() const { return _M_t.key_comp(); }
    allocator_type get_allocator() const { return _M_t.get_allocator(); }

    iterator begin() const { return _M_t.begin(); }
    iterator end() const { return _M_t.end(); }
    reverse_iterator rbegin() const { return _M_t.rbegin(); } 
    reverse_iterator rend() const { return _M_t.rend(); }
    bool empty() const { return _M_t.empty(); }
    size_type size() const { return _M_t.size(); }
    size_type max_size() const { return _M_t.max_size(); }
    void swap(multiset<_Key, _Arch, _Compare, _Alloc> &__x) { _M_t.swap(__x._M_t); }

    // insert/erase
    iterator insert(const value_type &__x) { 
	return _M_t.insert_equal(__x);
    }
    iterator insert(iterator __position, const value_type &__x) {
	typedef typename _Rep_type::iterator _Rep_iterator;
	return _M_t.insert_equal((_Rep_iterator &)__position, __x);
    }

#ifdef __PSTL_MEMBER_TEMPLATES  
    template <class _InputIterator>
    void insert(_InputIterator __first, _InputIterator __last) {
	_M_t.insert_equal(__first, __last);
    }
#else
    void insert(const_ptr<value_type, arch_type> __first, const_ptr<value_type, arch_type> __last) {
	_M_t.insert_equal(__first, __last);
    }
    void insert(const_iterator __first, const_iterator __last) {
	_M_t.insert_equal(__first, __last);
    }
#endif /* __PSTL_MEMBER_TEMPLATES */
    void erase(iterator __position) { 
	typedef typename _Rep_type::iterator _Rep_iterator;
	_M_t.erase((_Rep_iterator &)__position); 
    }
    size_type erase(const key_type &__x) { 
	return _M_t.erase(__x); 
    }
    void erase(iterator __first, iterator __last) { 
	typedef typename _Rep_type::iterator _Rep_iterator;
	_M_t.erase((_Rep_iterator &)__first, (_Rep_iterator &)__last); 
    }
    void clear() { _M_t.clear(); }

    // multiset operations:

    iterator find(const key_type &__x) const { return _M_t.find(__x); }
    size_type count(const key_type &__x) const { return _M_t.count(__x); }
    iterator lower_bound(const key_type &__x) const {
	return _M_t.lower_bound(__x);
    }
    iterator upper_bound(const key_type &__x) const {
	return _M_t.upper_bound(__x); 
    }
    pstl::pair<iterator, iterator, arch_type> equal_range(const key_type &__x) const {
	return _M_t.equal_range(__x);
    }

#ifdef __PSTL_TEMPLATE_FRIENDS
    template <class _K1, class _Ar1, class _C1, class _A1>
    friend bool operator== (const multiset<_K1, _Ar1, _C1, _A1> &,
			    const multiset<_K1, _Ar1, _C1, _A1> &);
    template <class _K1, class _Ar1, class _C1, class _A1>
    friend bool operator< (const multiset<_K1, _Ar1, _C1, _A1> &,
			   const multiset<_K1, _Ar1, _C1, _A1> &);
#else /* __PSTL_TEMPLATE_FRIENDS */
    friend bool __STD_QUALIFIER
    operator== __PSTL_NULL_TMPL_ARGS (const multiset &, const multiset &);
    friend bool __STD_QUALIFIER
    operator< __PSTL_NULL_TMPL_ARGS (const multiset &, const multiset &);
#endif /* __PSTL_TEMPLATE_FRIENDS */

    template <class _Obj> friend struct replace_pheap;
};

template <class _Key, class _Arch, class _Compare, class _Alloc>
struct replace_pheap<multiset<_Key, _Arch, _Compare, _Alloc> > {
    void operator()(const pheap_ptr __ph, multiset<_Key, _Arch, _Compare, _Alloc> &__s) {
	__replace_pheap(__ph, __s._M_t);
    }
};

template <class _Key, class _Arch, class _Compare, class _Alloc>
inline bool operator==(const multiset<_Key, _Arch, _Compare, _Alloc> &__x, 
                       const multiset<_Key, _Arch, _Compare, _Alloc> &__y) {
    return __x._M_t == __y._M_t;
}

template <class _Key, class _Arch, class _Compare, class _Alloc>
inline bool operator<(const multiset<_Key, _Arch, _Compare, _Alloc> &__x, 
                      const multiset<_Key, _Arch, _Compare, _Alloc> &__y) {
    return __x._M_t < __y._M_t;
}

#ifdef __PSTL_FUNCTION_TMPL_PARTIAL_ORDER

template <class _Key, class _Arch, class _Compare, class _Alloc>
inline bool operator!=(const multiset<_Key, _Arch, _Compare, _Alloc> &__x, 
                       const multiset<_Key, _Arch, _Compare, _Alloc> &__y) {
    return !(__x == __y);
}

template <class _Key, class _Arch, class _Compare, class _Alloc>
inline bool operator>(const multiset<_Key, _Arch, _Compare, _Alloc> &__x, 
                      const multiset<_Key, _Arch, _Compare, _Alloc> &__y) {
    return __y < __x;
}

template <class _Key, class _Arch, class _Compare, class _Alloc>
inline bool operator<=(const multiset<_Key, _Arch, _Compare, _Alloc> &__x, 
                       const multiset<_Key, _Arch, _Compare, _Alloc> &__y) {
    return !(__y < __x);
}

template <class _Key, class _Arch, class _Compare, class _Alloc>
inline bool operator>=(const multiset<_Key, _Arch, _Compare, _Alloc> &__x, 
                       const multiset<_Key, _Arch, _Compare, _Alloc> &__y) {
    return !(__x < __y);
}

template <class _Key, class _Arch, class _Compare, class _Alloc>
inline void swap(multiset<_Key, _Arch, _Compare, _Alloc> &__x, 
                 multiset<_Key, _Arch, _Compare, _Alloc> &__y) {
    __x.swap(__y);
}

#endif /* __PSTL_FUNCTION_TMPL_PARTIAL_ORDER */

#if defined(__sgi) && !defined(__GNUC__) && (_MIPS_SIM != _MIPS_SIM_ABI32)
#pragma reset woff 1174
#pragma reset woff 1375
#endif

__PSTL_END_NAMESPACE

#endif /* __SGI_PSTL_INTERNAL_MULTISET_H */

// Local Variables:
// mode:C++
// End:
