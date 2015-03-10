#include "../MyLibraries/Files/IOX.h"
#include "../MyLibraries/Params/Parameters.h"
#include <set>
#include <map>
#include <vector>
#include <iostream>
#include <fstream>
#include <tuple>

typedef set < string > Community;
typedef map < string, vector < int > > Membership; //Vertex to indices of communities it's a member of

void read_file ( const string& filename, vector < Community >& structure, Membership& membership_structure );
double Jaccard ( const Community& A, const Community& B );
double MinNorm ( const Community& A, const Community& B);

void Print ( const Membership& M );
void Print ( const Community& C );
 
void calculate_valid_steps ( const vector < vector < Community > >& C, const vector < Membership >& M, vector < vector < int > >& steps, double threshold = 0.15, double (*compare_fnc) ( const Community& A, const Community& B ) = &Jaccard );

tuple<int, int> int_tuple ( int a, int b ){
  if ( a < b ){
    return tuple<int, int> ( a, b );
  } else {
    return tuple<int, int> ( b, a );
  }
}


int main ( int argc, char** argv ){
  Parameters P; P.Read ( argc, argv );
  ifstream fin;
  vector < vector < Community > > dyn_structure;
  vector < Membership > mem_structure;
  openFileHarsh ( &fin, P.get < string > ( "i", "communities.dat" ) );
  vector < string > filenames;

  Community A, B;  
  
  while ( fline_tr ( &fin, &filenames, " " ) ){
    for ( int i = 0; i < filenames.size(); i++ ){
      vector < Community > empty;
      map < string, vector < int > > empty_member;

      read_file ( filenames[i], empty, empty_member );

      dyn_structure.push_back ( empty );
      mem_structure.push_back ( empty_member );
    }
  }

  /*for ( int i = 0; i < dyn_structure.size(); i++ ){
    for ( int j = 0; j < dyn_structure[i].size(); j++ ){
      Print ( dyn_structure[i][j] );
    }
    cout << " -------- " << endl;
  }

  for ( int i = 0; i < mem_structure.size(); i++ ){
    Print ( mem_structure[i] );
    cout << " -------- " << endl;
    }*/

  fin.close(); 

  vector < vector < int > > steps;
  calculate_valid_steps ( dyn_structure, mem_structure, steps ); 
}

void Print ( const Community& C ){
  Community::const_iterator it_c;
  for ( it_c = C.begin(); it_c != C.end(); it_c++ ){
    cout << *it_c << " ";
  }
  cout << endl;
}

void Print ( const Membership& M ){
  Membership::const_iterator it_m;
  for ( it_m = M.begin(); it_m != M.end(); it_m++ ){
    cout << it_m->first << " :";
    for ( int i = 0; i < it_m->second.size(); i++ ){
      cout << " " << it_m->second[i];
    }
    cout << endl;
  }
}

void Print ( const int i, const int j, const vector < vector < Community > >& structure, const vector < vector < int > >& steps, set < tuple < int, int > >& visited ) {
  int ci = i, cj = j;
  
  Print ( structure[ci][cj] );

  while ( ( steps[ci][cj] >= 0 ) ){
    //cout << (ci + 1) << " " << steps[ci][cj] << " " << ci << " " << cj << " " << endl;
    Print ( structure[ci + 1][steps[ci][cj]] );
    cj = steps[ci][cj];
    ci = ci + 1;
    
    visited.insert ( tuple < int , int > ( ci, cj ) );
  }

  cout << "------------" << endl;
}
 
void read_file ( const string& filename, vector < Community >& structure, Membership& membership_structure ){
  ifstream fin;
  openFileHarsh( &fin, filename );
  
  Community next_com;
  Community::iterator it_c;
  Membership::iterator it_m;
  
  while ( fline_tr ( &fin, &next_com, " " ) ){    

    for ( it_c = next_com.begin(); it_c != next_com.end(); it_c++ ){
      if ( ( it_m = membership_structure.find ( *it_c ) ) != membership_structure.end() ){
	it_m->second.push_back( structure.size() );
      } else {
	vector < int > tmp; tmp.push_back(structure.size());
	membership_structure.insert(pair <string, vector < int > > ( *it_c, tmp ) );
      }
    }
    
    structure.push_back ( next_com );
    next_com.clear();
  }

  fin.close();
}

double Jaccard ( const Community& A, const Community& B ){
  double same = 0.0;
  double total = 0.0;

  Community::const_iterator it_a, it_b;
  for ( it_a = A.begin(); it_a != A.end(); it_a++ ){
    ++total;
    if ( B.find ( *it_a ) != B.end() ) ++same;
  }

  for ( it_b = B.begin(); it_b != B.end(); it_b++ ){
    if ( A.find ( *it_b ) == A.end() ) ++total;
  }

  return same / total;
}

double MinNorm ( const Community& A, const Community& B){
  return 1;
}

int path_length ( const int start_window, const int start_com, const vector < vector < int > >& steps ){
  int i = start_window, j = start_com;
  int len = 0;
  while ( (i < steps.size() ) && ( j < steps[i].size() ) && (i > 0 ) && (j>0) && ( steps[i][j] >= 0 ) ) {
    j = steps[i][j];
    i++;
    ++len;
  }

  return len;
}

void calculate_valid_steps ( const vector < vector < Community > >& C, const vector < Membership >& M, vector < vector < int > >& steps, double threshold, double (*compare_fnc) ( const Community& A, const Community& B ) ) {
  steps.clear();
  for ( int i = 0; i < C.size(); i++ ){
    vector < int > init ( C[i].size(), -1 );
    steps.push_back( init );
  }
  
  set < tuple < int, int > > matches;
  Membership::const_iterator it_m, it_n;

  for ( int i = C.size() - 2; i >= 0; i-- ){
    matches.clear();
    for ( it_m = M[i].begin(); it_m != M[i].end(); it_m++ ){
      if ( ( it_n = M[i + 1].find ( it_m->first ) ) != M[i+1].end() ){
	for ( int k = 0; k < it_m->second.size(); k++ ){
	  for ( int j = 0; j < it_n->second.size(); j++ ){
	    matches.insert ( tuple <int, int> ( it_m->second[k], it_n->second[j] ) );
	  }
	}
      }
    }

    set < tuple < int, int > >::iterator it_t;
    it_t = matches.begin();
    while ( it_t != matches.end() ) {
      int current_base = get < 0 > ( *it_t );
      int best_target = -1;      
      double best_value = -1.0, next_value;
      while ( ( it_t != matches.end() ) && ( get < 0 > ( *it_t ) == current_base ) ){
	if ( ( ( next_value = compare_fnc ( C[i][get<0>(*it_t)], C[i+1][get<1>(*it_t)] ) ) >= best_value ) && ( next_value > threshold ) ) {
	  if ( next_value == best_value ){
	    if ( path_length ( i+1, best_target, steps ) < path_length ( i+1, get < 1 > (*it_t), steps ) ) {
	      best_value = next_value;
	      best_target = get<1>(*it_t);	      
	    }
	  } else {  
	    best_value = next_value;
	    best_target = get<1>(*it_t);
	  }
	}

	//cout << next_value << " " << get < 0 > (*it_t ) << " " << get < 1 > (*it_t) << endl;

	++it_t;
      }

      if ( best_value > -1 )
	steps[i][current_base] = best_target;
    }
  }

  set < tuple < int, int > > visited;

  for ( int i = 0; i < steps.size(); i++ ){
    cout << "Timestep " << i << endl;
    for ( int j = 0; j < steps[i].size(); j++ ){
      if ( visited.find(tuple < int, int > (i, j) ) == visited.end() ) {
	//Print out the evolution for i,j
	Print ( i, j, C, steps, visited); 
      }
    }
  }
}
