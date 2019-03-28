//$Id: hash.h,v 1.12 2013/02/16 12:18:36 darseq Exp $
#ifndef HashTableH
#define HashTableH

#include <vector>
#include <list>
#include "defines.h"

//---------------------------------------------------------------------------
namespace hash_space {
	using namespace std;
	template
	<class T>

	class HashTable : public vector<list<T>,allocator< list<T> > >
	{
		typedef list<T> list_t;
		typedef allocator<list_t> listalloc;
		typedef vector<list_t,listalloc> vect_t;
		typedef typename list_t::iterator list_iter;
		typedef typename vect_t::iterator vect_iter;

	private:
		unsigned int eltnumber;	// howmany Ts are present in the hashtable
		unsigned int global_index;
		list_iter global_cur;
		//------------------------------------------------

	public:
		__fastcall HashTable(unsigned int hashsize) : vect_t(hashsize) {
			if(hashsize == 0) {ATTO("hash:Invalid constructor arguments");}
			eltnumber = 0;
		}
		//------------------------------------------------------------------

		__fastcall HashTable(HashTable & oldhash,unsigned int newsize)
			: vect_t(newsize) {
			if(newsize == 0) {ATTO("hash:Invalid constructor arguments");}
			eltnumber = 0;

			vect_iter it = oldhash.begin();
			for(;it != oldhash.end();it++) {
				list_iter cur = it->begin();
				for(;cur != it->end();cur++) {insert(*cur);}
			}
		}
		//------------------------------------------------------------------

		__fastcall vector<T> toVector() {
			vector<T> myvector;
			vect_iter it = this->begin();
			for(;it != this->end();it++) {
				list_iter cur = it->begin();
				for(;cur != it->end();cur++) {myvector.push_back(*cur);}
			}
			return myvector;
		}
		//------------------------------------------------------------------

		__fastcall void insert(T e) {
			(*this)[e.hashcode() % this->size()].push_back(e);
			eltnumber++;
		}
		//------------------------------------------------------------------

		// todo: return iterator
		__fastcall bool search(T & search_e,T & result_e) {
			global_index = search_e.hashcode() % this->size();
			bool found; global_cur = (*this)[global_index].begin();
			for(;(found = global_cur != (*this)[global_index].end()) == true &&
				(search_e == *global_cur) == false;global_cur++);
			if(found) {result_e = *global_cur;}
			return found;
		}
		//------------------------------------------------------------------

		// erase should ALWAYS be called immediately after a search. This means
		// that no other hash function may be called in between. example:
		// search(e1,e2); if(erase()) {message("element e2 has been deleted");}
		// todo: accept iterator
		__fastcall bool erase() {
			bool found;
			if((found = global_cur != (*this)[global_index].end()) == true) {
				global_cur = (*this)[global_index].erase(global_cur);
				eltnumber--;
			}
			return found;
		}
		//------------------------------------------------------------------

		__fastcall void clear() {
			vect_iter it = this->begin();
			for(;it != this->end();it++) {it->clear();}
			eltnumber = 0;
		}
		//------------------------------------------------------------------

		__fastcall unsigned int elementCount() {
			return eltnumber;
		}
		//------------------------------------------------------------------

/*
		__fastcall bool next(T & next_e,bool & initialize) {
			if(initialize) { // hsize is always > 0
				initialize = false; index = 0;
				cur = (*this)[index].begin();
			}
			if(cur == (*this)[index].end()) {
				if(++index < this->size()) {
					cur = (*this)[index].begin();
					next(next_e,initialize);
				} else {initialize = true;}
			} else {next_e = *cur; cur++;}
			return initialize == false;
		}
		//------------------------------------------------------------------
*/
	}; //class
}// namespace

#endif

