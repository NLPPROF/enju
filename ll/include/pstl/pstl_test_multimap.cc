#define __PSTL_COMPATIBLE_64
#define __PSTL_ALLOC_BIT_CLASS pstl::b64

#include "pstl_pheap.h"
#include "pstl_utility"
#include "pstl_vector"
#include "pstl_map"

#include <iostream>
#include <utility>
#include <cstdlib>
#include <ctime>
#include <vector>
#include <map>

#include <sys/time.h>
#include <sys/resource.h>
#include <unistd.h>

const int MAX_RAND_INT = 10000;

namespace pstl {
    template <class _Arch> class global_variable {
    private:
	typedef pstl::multimap<int, int, _Arch> multimap_type;
	typedef pheap<_Arch> pheap_type;
    public:
	typedef _Arch arch_type;
	ptr<multimap_type, arch_type> mp;
    };

    template <class _Arch>
    struct replace_pheap<global_variable<_Arch> > {
	void operator()(const pheap_ptr __ph, global_variable<_Arch> &__gv) {
	    __replace_pheap(__ph, __gv.mp);
	}
    };

    template <class _Arch> class b {
    private:
	typedef pheap<_Arch> pheap_type;
	typedef pstl::multimap<int, int, _Arch> multimap_type;
	typedef global_variable<_Arch> global_variable_type;

    public:
	typedef _Arch arch_type;
	
    private:
	pheap_type phv;
	pheap_ptr ph;
	ptr<multimap_type, arch_type> mp;
    public:
	typename multimap_type::reference m;

    public:
	b() : phv(16), ph(pheap_type::pheap_convert(&phv)) {}
	~b() {}
	void open(char *filename) {
	    if (!pheap_type::pheap_convert(ph)->open(filename)) {
		std::cerr << "fail to open" << std::endl;
		return;
	    }

	    ptr<global_variable_type, _Arch> gvp = ptr<global_variable_type, _Arch>(pheap_type::pheap_convert(ph)->getRoot(), ph);
	    if (!gvp) {
		gvp = pheap_type::pheap_convert(ph)->malloc(sizeof(global_variable_type));
		(*gvp)->mp = multimap_type::pnew(ph, typename multimap_type::key_compare(), typename multimap_type::allocator_type(ph));
		pheap_type::pheap_convert(ph)->setRoot(ptr<void, arch_type>(gvp, ph));
	    }
	    m.reassign(*(*gvp)->mp);
	}

	void init_multimap(const std::multimap<int, int> &stdm, const int sz = 0);
	void func_apply_multimap(const int trial_func, const int seed, const int seed2, const int seed3);

	pheap_ptr get_pheap() const { return ph; }

	template <class _Obj> friend struct replace_pheap;
    };

    template <class _Arch>
    struct replace_pheap<b<_Arch> > {
	void operator()(const pheap_ptr __ph, b<_Arch> &__obj) {
	    __obj.phv = *pheap<_Arch>::pheap_convert(__ph);
	    __obj.ph = pheap<_Arch>::pheap_convert(&__obj.phv);
	    __replace_pheap(__ph, __obj.mp);
	}
    };

    template <class _Arch>
    void b<_Arch>::init_multimap(const std::multimap<int, int> &stdm, const int sz) {
	m->clear();
	if (!stdm.empty()) {
	    for (typename std::multimap<int, int>::const_iterator it = stdm.begin();
		 it != stdm.end(); ++it) {
		
		m->insert(pstl::make_pair<int, int, _Arch>(it->first, it->second));
	    }
	}
	else {
	    for (int i = 0; i < sz; i++) {
		m->insert(pstl::make_pair<int, int, _Arch>(i, std::rand() % MAX_RAND_INT));
	    }
	}
    }

    template <class _Arch>
    void b<_Arch>::func_apply_multimap(const int trial_func, const int seed, const int seed2, const int seed3) {
	const int NUM_OF_FUNCS = 17;
	typename pstl::multimap<int, int, _Arch>::iterator it, it1, it2;
	int id, r, pos;
	pstl::pair<typename pstl::multimap<int, int, _Arch>::iterator,
	    typename pstl::multimap<int, int, _Arch>::iterator,
	    _Arch> pa;
	pstl::vector<pstl::pair<int, int, _Arch>, _Arch> v(ph);
	v.push_back(pstl::make_pair<int, int, _Arch>(seed % m->size(), seed2));
	std::cout << "pstl::multimap[" << _Arch::dev_type::dev_type << "," << _Arch::bit_type::bit_size << "]::";

	switch (trial_func % NUM_OF_FUNCS) {
	case 0: // empty()
	    std::cout << "empty() = ";
	    if (m->empty()) {
		std::cout << "true" << std::endl;
	    }
	    else {
		std::cout << "false" << std::endl;
	    }
	    break;
	case 1: // size()
	    std::cout << "size() = " << m->size() << std::endl;
	    break;
	case 2: // max_size()
	    std::cout << "max_size() = " << m->max_size() << std::endl;
	    break;
	case 3:
	    std::cout << "multimap does not have operator[]()" << std::endl;
	    break;
	case 4: // swap(multimap &)
	    std::cout << "swap(multimap &) (TEST PROGRAM UNIMPLEMENTED)" << std::endl;
	    break;
	case 5: // insert(const value_type &)
	    if (m->empty()) {
		id = 0;
	    }
	    else {
	        id = seed % (m->size() * 2);
	    }
	    r = seed2;
	    m->insert(pstl::make_pair<int, int, _Arch>(id, r));
	    break;
	case 6: // insert(iterator, const value_type &)
	    if (m->empty()) {
		std::cout << "insert(iterator, const value_type &) NOT EXECUTED (empty multimap)" << std::endl;
		break;
	    }
	    pos = seed % m->size();
	    it = m->begin();
	    for (int i = 0; i < pos; i++, ++it);
	    id = seed2 % (m->size() * 2);
	    r = seed3;
	    std::cout << "insert(begin()+" << pos << ", (" << id;
	    std::cout << ", " << r << "))" << std::endl;
	    m->insert(it, pstl::make_pair<int, int, _Arch>(id, r));
	    break;
	case 7: // insert(_InputIterator, _InputIterator)
	    std::cout << "insert(v.begin(), v.end())" << std::endl;
	    m->insert(v.begin(), v.end());
	    break;
	case 8: // erase(iterator)
	    if (m->empty()) {
		std::cout << "erase(iterator) NOT EXECUTED (empty multimap)" << std::endl;
		break;
	    }
	    std::cout << "erase(begin())" << std::endl;
	    m->erase(m->begin());
	    break;
	case 9: // erase(const key_type &)
	    if (m->empty()) {
		std::cout << "erase(const key_type &) NOT EXECUTED (empty multimap)" << std::endl;
		break;
	    }
	    id = seed % m->size();
	    std::cout << "erase(" << id << ") = ";
	    std::cout << m->erase(id) << std::endl;
	    break;
	case 10: // erase(iterator, iterator);
	    if (m->empty()) {
		std::cout << "erase(iterator, iterator) NOT EXECUTED (empty multimap)" << std::endl;
		break;
	    }
	    it1 = m->begin();
	    it2 = m->begin();
	    if (it2 != m->end()) {
		++it2;
	    }
	    if (it2 != m->end()) {
		++it2;
	    }
	    std::cout << "erase(begin(),begin()+2)" << std::endl;
	    m->erase(it1, it2);
	    break;
	case 11: // clear()
	    // std::cout << "clear()" << std::endl;
	    // m->clear();
	    std::cout << "clear() NOT EXECUTED (for test)" << std::endl;
	    break;
	case 12: // find(const key_type &)
	    if (m->empty()) {
		std::cout << "find(const key_type &) NOT EXECUTED (empty multimap)" << std::endl;
		break;
	    }
	    id = seed % m->size();
	    it = m->find(id);
	    std::cout << "*find(" << id << ") = ";
	    if (it == m->end()) {
		std::cout << "end() (NOT FOUND)" << std::endl;
	    }
	    else {
		std::cout << "(" << (*it)->get_first(it) << ", ";
		std::cout << (*it)->get_second(it) << ")" << std::endl;
	    }
	    break;
	case 13: // count(const key_type &)
	    if (m->empty()) {
		std::cout << "count(const key_type &) NOT EXECUTED (empty multimap)" << std::endl;
		break;
	    }
	    id = seed % m->size();
	    std::cout << "count(" << id << ") = " << m->count(id) << std::endl;
	    break;
	case 14: // lower_bound(const key_type &)
	    if (m->empty()) {
		std::cout << "lower_bound(const key_type &) NOT EXECUTED (empty multimap)" << std::endl;
		break;
	    }
	    id = seed % m->size();
	    it = m->lower_bound(id);
	    std::cout << "*lower_bound(" << id << ") = ";
	    if (it == m->end()) {
		std::cout << "end() (NOT FOUND)" << std::endl;
	    }
	    else {
		std::cout << "(" << (*it)->get_first(it) << ", ";
		std::cout << (*it)->get_second(it) << ")" << std::endl;
	    }
	    break;
	case 15: // upper_bound(const key_type &)
	    if (m->empty()) {
		std::cout << "upper_bound(const key_type &) NOT EXECUTED (empty multimap)" << std::endl;
		break;
	    }
	    id = seed % m->size();
	    it = m->upper_bound(id);
	    std::cout << "*upper_bound(" << id << ") = ";
	    if (it == m->end()) {
		std::cout << "end() (NOT FOUND)" << std::endl;
	    }
	    else {
		std::cout << "(" << (*it)->get_first(it) << ", ";
		std::cout << (*it)->get_second(it) << ")" << std::endl;
	    }
	    break;
	case 16: // equal_range(const key_type &)
	    if (m->empty()) {
		std::cout << "equal_range(const key_type &) NOT EXECUTED (empty multimap)" << std::endl;
		break;
	    }
	    id = seed % m->size();
	    pa = m->equal_range(id);
	    it1 = pa.first;
	    it2 = pa.second;
	    std::cout << "equal_range(" << id << ") = (";
	    if (it1 == m->end()) {
		std::cout << "end(), ";
	    }
	    else {
		std::cout << "&(" << (*it1)->get_first(it1) << ", ";
		std::cout << (*it1)->get_second(it1) << "), ";
	    }
	    if (it2 == m->end()) {
		std::cout << "end())";
	    }
	    else {
		std::cout << "&(" << (*it2)->get_first(it2) << ", ";
		std::cout << (*it2)->get_second(it2) << "))";
	    }
	    std::cout << std::endl;
	    break;
	default:
	    break;
	}
    }

    class c {
    private:
	typedef std::multimap<int, int> multimap_type;
    public:
	multimap_type m;

	template <class _Arch>
	void init_multimap(pstl::multimap<int, int, _Arch> &pstlm, const int sz = 0);
	void func_apply_multimap(const int trial_func, const int seed, const int seed2, const int seed3);
    };

    template <class _Arch>
    void c::init_multimap(pstl::multimap<int, int, _Arch> &pstlm, const int sz) {
	m.clear();
	if (!pstlm.empty()) {
	    for (typename pstl::multimap<int, int, _Arch>::const_iterator it =
		     pstlm.begin(); it != pstlm.end(); ++it) {
		m.insert(std::make_pair((*it)->get_first(it).get_obj(), (*it)->get_second(it).get_obj()));
	    }
	}
	else {
	    for (int i = 0; i < sz; i++) {
		m.insert(std::make_pair(i, std::rand() % MAX_RAND_INT));
	    }
	}
    }

    void c::func_apply_multimap(const int trial_func, const int seed, const int seed2, const int seed3) {
	const int NUM_OF_FUNCS = 17;
	std::multimap<int, int>::iterator it, it1, it2;
	int id, r, pos;
	std::pair<std::multimap<int, int>::iterator, std::multimap<int, int>::iterator> pa;
	std::vector<std::pair<int, int> > v;
	v.push_back(std::make_pair(seed % m.size(), seed2));
	std::cout << "std::multimap::";
       
	switch (trial_func % NUM_OF_FUNCS) {
	case 0: // empty()
	    std::cout << "empty() = ";
	    if (m.empty()) {
		std::cout << "true" << std::endl;
	    }
	    else {
		std::cout << "false" << std::endl;
	    }
	    break;
	case 1: // size()
	    std::cout << "size() = " << m.size() << std::endl;
	    break;
	case 2: // max_size()
	    std::cout << "max_size() = " << m.max_size() << std::endl;
	    break;
	case 3:
	    std::cout << "multimap does not have operator[]()" << std::endl;
	    break;
	case 4: // swap(multimap &)
	    std::cout << "swap(multimap &) (TEST PROGRAM UNIMPLEMENTED)" << std::endl;
	    break;
	case 5: // insert(const value_type &)
	    if (m.empty()) {
		id = 0;
	    }
	    else {
	        id = seed % (m.size() * 2);
	    }
	    r = seed2;
	    std::cout << "insert((" << id << ", " << r << "))" << std::endl;
	    m.insert(std::make_pair(id, r));
	    break;
	case 6: // insert(iterator, const value_type &)
	    if (m.empty()) {
		std::cout << "insert(iterator, const value_type &) NOT EXECUTED (empty multimap)" << std::endl;
		break;
	    }
	    pos = seed % m.size();
	    it = m.begin();
	    for (int i = 0; i < pos; i++, ++it);
	    id = seed2 % (m.size() * 2);
	    r = seed3;
	    std::cout << "insert(begin()+" << pos << ", (" << id;
	    std::cout << ", " << r << "))" << std::endl;
	    m.insert(it, std::make_pair(id, r));
	    break;
	case 7: // insert(_InputIterator, _InputIterator)
	    std::cout << "insert(v.begin(), v.end())" << std::endl;
	    m.insert(v.begin(), v.end());
	    break;
	case 8: // erase(iterator)
	    if (m.empty()) {
		std::cout << "erase(iterator) NOT EXECUTED (empty multimap)" << std::endl;
		break;
	    }
	    std::cout << "erase(begin())" << std::endl;
	    m.erase(m.begin());
	    break;
	case 9: // erase(const key_type &)
	    if (m.empty()) {
		std::cout << "erase(const key_type &) NOT EXECUTED (empty multimap)" << std::endl;
		break;
	    }
	    id = seed % m.size();
	    std::cout << "erase(" << id << ") = ";
	    std::cout << m.erase(id) << std::endl;
	    break;
	case 10: // erase(iterator, iterator);
	    if (m.empty()) {
		std::cout << "erase(iterator, iterator) NOT EXECUTED (empty multimap)" << std::endl;
		break;
	    }
	    it1 = m.begin();
	    it2 = m.begin();
	    if (it2 != m.end()) {
		++it2;
	    }
	    if (it2 != m.end()) {
		++it2;
	    }
	    std::cout << "erase(begin(),begin()+2)" << std::endl;
	    m.erase(it1, it2);
	    break;
	case 11: // clear()
	    //std::cout << "clear()" << std::endl;
	    //m.clear();
	    std::cout << "clear() NOT EXECUTED (for test)" << std::endl;
	    break;
	case 12: // find(const key_type &)
	    if (m.empty()) {
		std::cout << "find(const key_type &) NOT EXECUTED (empty multimap)" << std::endl;
		break;
	    }
	    id = seed % m.size();
	    it = m.find(id);
	    std::cout << "*find(" << id << ") = ";
	    if (it == m.end()) {
		std::cout << "end() (NOT FOUND)" << std::endl;
	    }
	    else {
		std::cout << "(" << it->first << ", ";
		std::cout << it->second << ")" << std::endl;
	    }
	    break;
	case 13: // count(const key_type &)
	    if (m.empty()) {
		std::cout << "count(const key_type &) NOT EXECUTED (empty multimap)" << std::endl;
		break;
	    }
	    id = seed % m.size();
	    std::cout << "count(" << id << ") = " << m.count(id) << std::endl;
	    break;
	case 14: // lower_bound(const key_type &)
	    if (m.empty()) {
		std::cout << "lower_bound(const key_type &) NOT EXECUTED (empty multimap)" << std::endl;
		break;
	    }
	    id = seed % m.size();
	    it = m.lower_bound(id);
	    std::cout << "*lower_bound(" << id << ") = ";
	    if (it == m.end()) {
		std::cout << "end() (NOT FOUND)" << std::endl;
	    }
	    else {
		std::cout << "(" << it->first << ", ";
		std::cout << it->second << ")" << std::endl;
	    }
	    break;
	case 15: // upper_bound(const key_type &)
	    if (m.empty()) {
		std::cout << "upper_bound(const key_type &) NOT EXECUTED (empty multimap)" << std::endl;
		break;
	    }
	    id = seed % m.size();
	    it = m.upper_bound(id);
	    std::cout << "*upper_bound(" << id << ") = ";
	    if (it == m.end()) {
		std::cout << "end() (NOT FOUND)" << std::endl;
	    }
	    else {
		std::cout << "(" << it->first << ", ";
		std::cout << it->second << ")" << std::endl;
	    }
	    break;
	case 16: // equal_range(const key_type &)
	    if (m.empty()) {
		std::cout << "equal_range(const key_type &) NOT EXECUTED (empty multimap)" << std::endl;
		break;
	    }
	    id = seed % m.size();
	    pa = m.equal_range(id);
	    it1 = pa.first;
	    it2 = pa.second;
	    std::cout << "equal_range(" << id << ") = (";
	    if (it1 == m.end()) {
		std::cout << "end(), ";
	    }
	    else {
		std::cout << "&(" << it1->first << ", ";
		std::cout << it1->second << "), ";
	    }
	    if (it2 == m.end()) {
		std::cout << "end())";
	    }
	    else {
		std::cout << "&(" << it2->first << ", ";
		std::cout << it2->second << "))";
	    }
	    std::cout << std::endl;
	    break;
	default:
	    break;
	}
    }

    template <class _Arch>
    void validate_multimap(const b<_Arch> &b_obj, const c &c_obj) {
	std::multimap<int, int>::const_iterator std_it = c_obj.m.begin(), std_diff_it;
	typename pstl::multimap<int, int, _Arch>::const_iterator pstl_it = b_obj.m->begin(), pstl_diff_it;
	bool matched = true;
	for ( ; std_it != c_obj.m.end(); ++std_it, ++pstl_it) {
	    if (pstl_it == b_obj.m->end()) {
		matched = false;
		break;
	    }
	    if (std_it->first != (*pstl_it)->get_first(pstl_it) ||
		std_it->second != (*pstl_it)->get_second(pstl_it)) {
		std_diff_it = std_it;
		pstl_diff_it = pstl_it;
		matched = false;
		break;
	    }
	}
	std::cout << "std result:" << std::endl;
	for (std_it = c_obj.m.begin(); std_it != c_obj.m.end(); ++std_it) {
	    if (std_it != c_obj.m.begin()) {
		std::cout << ", ";
	    }
	    std::cout << "(" << std_it->first << ", ";
	    std::cout << std_it->second << ")";
	}
	std::cout << std::endl;
	if (!matched) {
	    std::cout << "pstl result:" << std::endl;
	    for (pstl_it = b_obj.m->begin(); pstl_it != b_obj.m->end();
		 ++pstl_it) {
		if (pstl_it != b_obj.m->begin()) {
		    std::cout << ", ";
		}
		std::cout << "(" << (*pstl_it)->get_first(pstl_it) << ", ";
		std::cout << (*pstl_it)->get_second(pstl_it) << ")";
	    }
	    std::cout << std::endl;
	    std::cout << "DIFFERENT" << std::endl;
	    std::cout << "first difference point:" << std::endl;
	    std::cout << "std: ";
	    std::cout << "(" << std_diff_it->first << ", ";
	    std::cout << std_diff_it->second << ")" << std::endl;
	    std::cout << "pstl: ";
	    std::cout << "(" << (*pstl_diff_it)->get_first(pstl_diff_it) << ", ";
	    std::cout << (*pstl_diff_it)->get_second(pstl_diff_it) << ")" << std::endl;
	}
	else {
	    std::cout << "AGREEMENT" << std::endl;
	}
    }
}

int main() {
    const int NUM_TRIAL = 100;
    const int NUM_OF_FUNCS = 17;
    
    std::srand(static_cast<unsigned int>(std::time(NULL)));

    pstl::c k;
    pstl::b<pstl::arch<pstl::pmem> > i;
    i.open("");
    pstl::b<pstl::arch<pstl::vmem, pstl::b64> > j;
    j.open("hoge.ldb");
    
    k.init_multimap(*j.m, 100);
    i.init_multimap(k.m, 100);
    j.init_multimap(k.m, 100);

    pstl::validate_multimap(i, k);
    pstl::validate_multimap(j, k);
    
    for (int s = 0; s < NUM_TRIAL; s++) {
	int func_id = std::rand() % NUM_OF_FUNCS;
	int seed = std::rand() % MAX_RAND_INT,
	    seed2 = std::rand() % MAX_RAND_INT,
	    seed3 = std::rand() % MAX_RAND_INT;
	k.func_apply_multimap(func_id, seed, seed2, seed3);
	i.func_apply_multimap(func_id, seed, seed2, seed3);
	j.func_apply_multimap(func_id, seed, seed2, seed3);
    }
    
    pstl::validate_multimap(i, k);
    pstl::validate_multimap(j, k);

    return 0;
}
