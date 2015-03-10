#ifndef JT_ISET
#define JT_ISET

#include <map>
#include <set>
#include <functional>
#include <string>
#include <iostream>
#include <cstdlib>

using namespace std;


class str_com{
 public:
  str_com () : seen ( false ){};

  bool operator< ( const str_com& rhs ) const {
    set < string >::iterator it_a, it_b = rhs.members.begin();
    for ( it_a = members.begin(); (it_a != members.end()) && (it_b != rhs.members.end()); it_a++, it_b++ ){
      int res = (*it_a).compare ( *it_b );
      if ( res < 0 ) return true;
      if ( res > 0 ) return false;
    }
    
    if ( ( it_a == members.end() ) && ( it_b == rhs.members.end() ) )
      return false;
    if ( it_a == members.end() )
      return true;
    if ( it_b == rhs.members.end() )
      return false;
    
    return false;
  }

  void insert ( string s ) {members.insert(s);}
  void remove ( string s ) {members.erase(s);}

  bool hasMember ( string s ) const {
    return (members.find ( s ) != members.end() );
  }

  void print ( ) const {
    set < string >::const_iterator it_s;
    for ( it_s = members.begin(); it_s != members.end(); it_s++ ){
      cout << *it_s << " ";
    }
    cout << endl;
  }

  void print ( ) {
    set < string >::iterator it_s;
    for ( it_s = members.begin(); it_s != members.end(); it_s++ ){
      cout << *it_s << " ";
    }
    cout << endl;
  }

  set < string >::const_iterator start() { return members.begin(); }
  set < string >::const_iterator end() { return members.end(); }
  set < string >::const_iterator start() const { return members.begin(); }
  set < string >::const_iterator end() const { return members.end(); }
  
  void setseen () { seen=true;}
  bool seen;
 private:
  set < string > members;
};

template <typename T>
class iset {
 public:
  bool insert ( int index_request, const T& new_value ) {
    pair < typename set < T >::iterator, bool > it_res;
    it_res = values_.insert(new_value);
    
    pair < typename map < int, typename set < T >::iterator >::iterator, bool > it_res2 = index_.insert ( pair < int, typename set < T >::iterator > ( index_request, it_res.first ) );
    return it_res2.second;
  }

  int insert ( const T& new_value ) {
    pair < typename set < T >::iterator, bool > it_res;
    it_res = values_.insert(new_value);
    
    pair < typename map < int, typename set < T >::iterator >::iterator, bool > it_res2 = index_.insert ( pair < int, typename set < T >::iterator > ( index_.size(), it_res.first ) );
    return it_res2.first->first;
  }

  T  operator[] ( const int& i ) {
    typename map < int, typename set < T >::iterator >::iterator it_m;
    if ( ( it_m = index_.find ( i ) ) == index_.end() ){
      cerr << "Index out of range in iset" << endl;
      exit ( 3 );
    } else {
      return *it_m->second;
    }
  }

  void setseen ( int id ){
    typename map < int, typename set < T >::iterator >::iterator it_m;
    if ( ( it_m = index_.find ( id ) ) != index_.end() ){
      (const_cast < str_com& > (*(it_m->second))).setseen();
    }
  }
  
  bool contains ( int i ) { 
    
    return index_.find ( i ) != index_.end(); 
  }
  
  int indices ( ) { return index_.size(); }
  int communities ( ) { return values_.size(); }

  void print ( ) {
    typename map < int, typename set < T >::iterator >::iterator it_m;
    for ( it_m = index_.begin(); it_m != index_.end(); it_m++ ){
      cout << it_m->first << " :: ";
      it_m->second->print();
    }
  }

  typename map < int, typename set < T >::iterator >::const_iterator istart ( ) { return index_.begin(); }
  typename map < int, typename set < T >::iterator >::const_iterator iend ( ) { return index_.end(); }
  typename map < int, typename set < T >::iterator >::const_iterator istart ( ) const { return index_.begin(); }
  typename map < int, typename set < T >::iterator >::const_iterator iend ( ) const { return index_.end(); }

 private:
  set < T > values_;
  map < int, typename set < T >::iterator > index_;
};


typedef pair < int, int > cid;
#endif
