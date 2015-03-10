#include "file_handler.h"

unique_ptr < iset < str_com > > load_ground_truth_com_file ( const string& filename ){
  unique_ptr < iset < str_com > > result ( new iset <str_com> ( ) );

  ifstream fin;
  openFileHarsh ( &fin, filename );
  
  vector < string > fields;
  
  while ( fline_tr ( &fin, &fields, " " ) ){
    vector < string > get_id;
    str_com current_com;
    split_tr ( fields[0], &get_id, ":" );
    current_com.insert ( get_id[1] );
    for ( unsigned int i = 1; i < fields.size(); i++ ){
      current_com.insert ( fields[i] );
    }

    result->insert ( str_to < int > ( get_id[0] ), current_com );
  }

  fin.close();

  return result;
}

void load_ground_truth_evo_file ( const string& filename, int window, multimap < cid, cid >& evolutions ){
  ifstream fin;
  openFileHarsh ( &fin, filename);
  
  vector < string > fields;
  while ( fline_tr ( &fin, &fields, " " ) ){
    int base = str_to < int > ( fields[0] );
    for ( unsigned int i = 1; i < fields.size(); i++ ){
      evolutions.insert(pair < cid, cid > ( cid (window, base), cid ( window + 1, str_to<int>(fields[i])) ));
    }
  }

  fin.close();
}

vector < unique_ptr < iset <str_com> > > load_rpi_evo_file ( const string& filename, vector < vector < cid > >& evolutions){
  vector < unique_ptr < iset <str_com> > > result;
  for (int i = 0; i < 20; i++ ){
    result.push_back ( unique_ptr < iset < str_com > > ( new iset < str_com > () ) );
  }
 
  ifstream fin;
  openFileHarsh ( &fin, filename );
  
  vector < string > fields;
  int base = -1;
  int current_window = 0;

  while ( fline_tr ( &fin, &fields, " " ) ){
    if ( fields[0].compare ( "Timestep" ) == 0 ){
      base = str_to < int > ( fields[1] );
      current_window = base;
      if ( ( evolutions.size() > 0 ) && ( evolutions[evolutions.size() - 1].size() < 3 ) ){
	  evolutions.pop_back();
	}

      if ( (evolutions.size() == 0) || ( evolutions[evolutions.size() - 1].size() > 0 ) )
	evolutions.push_back ( vector < cid > () );
    } else {
      if ( fields[0][0] == '-' ) {
	current_window = base;
	if ( ( evolutions.size() > 0 ) && ( evolutions[evolutions.size() - 1].size() < 3 ) ){
	  evolutions.pop_back();
	}
	
	if ( (evolutions.size() == 0) || ( evolutions[evolutions.size() - 1].size() > 0 ) )
	  evolutions.push_back ( vector < cid > () );	
      } else {
	str_com current_com;
	for (int unsigned i = 0; i < fields.size(); i++ ){
	  current_com.insert(fields[i]);
	}
	int current_id = result[current_window]->insert ( current_com );
	evolutions [ evolutions.size() - 1 ].push_back ( cid ( current_window, current_id ) );
	++current_window;
      }
    }
  }
  
  while ( evolutions[evolutions.size() - 1].size() == 0 ){
    evolutions.pop_back();
  }

  fin.close();

  return result;
}

map < string, vector < int > > vertex_map ( unique_ptr < iset < str_com > >& C ){
  map < int, set < str_com >::iterator >::const_iterator it_m = C->istart();
  map < string, vector < int > > result;
  map < string, vector < int > >::iterator it_r;
    
  while ( it_m != C->iend() ){
    set < string >::iterator it_s = it_m->second->start();
    while ( it_s != it_m->second->end() ){
      if ( ( it_r = result.find ( *it_s ) ) == result.end() ){
	vector < int > new_list;
	new_list.push_back( it_m->first );
	result.insert ( pair < string, vector < int > > ( *it_s, new_list ) );
      } else {
	it_r->second.push_back ( it_m->first );
      }
      
      ++it_s;
    }

    ++it_m;
  }

  return result;
}

double Jaccard ( const str_com& A, const str_com& B ) {
  set < string >::const_iterator it_s;
  double similar = 0, total = 0;
  for ( it_s = A.start(); it_s != A.end(); it_s++ ){
    if ( B.hasMember ( *it_s ) ){
      similar++;
    }
    ++total;
  }
  
  for ( it_s = B.start(); it_s != B.end(); it_s++ ){
    if ( !( A.hasMember ( *it_s ) ) ){
      ++total;
    }
  }

  /*cout << "---------------" << endl;
  A.print();
  B.print();
  cout << similar << " " << total << " " << similar / total << endl;*/

  return similar / total;
}

map < cid, map < cid, double > > compare_community_structures ( vector < unique_ptr < iset < str_com > > >& truth, vector < unique_ptr < iset < str_com > > >& detected ){
  map < cid, map < cid, double > > result;
  map < cid, map < cid, double > >::iterator it_r;
  map < cid, double >::iterator it_rr;

  for ( unsigned int i = 0; i < truth.size(); i++ ){
    if ( i >= detected.size() )
      break;

    map < string, vector < int > > truth_map = vertex_map ( truth[i] ), detected_map = vertex_map ( detected[i] );
    map < string, vector < int > >::iterator it_truth, it_detect;

    for ( it_truth = truth_map.begin(); it_truth != truth_map.end(); it_truth++ ){
      if ( ( it_detect = detected_map.find ( it_truth->first ) ) != detected_map.end() ){
	for ( unsigned int j = 0; j < it_truth->second.size(); j++ ){
	  cid base_pair = cid ( i, it_truth->second[j] );
	  if ( ( it_r = result.find ( base_pair ) ) == result.end() ) {
	    map < cid, double > empty_map;
	    result.insert ( pair < cid, map < cid, double > > (base_pair, empty_map ) );
	    it_r = result.find ( base_pair ); // I know -> laziness and low amount of time
	  }
	  
	  for ( unsigned int k = 0; k < it_detect->second.size(); k++ ){
	    it_r->second.insert ( pair < cid, double > ( cid (i, it_detect->second[k] ), 0.0 ) );
	  }
	}
      }
    }

    for ( it_r = result.begin(); it_r != result.end(); it_r++ ){
      for ( it_rr = it_r->second.begin(); it_rr != it_r->second.end(); it_rr++ ){
	it_rr->second = Jaccard ( (*(truth[it_r->first.first]))[it_r->first.second], (*(detected[it_rr->first.first]))[it_rr->first.second] );
      }
    }

    /*cout << "-------------_Round Begin_------------------" << endl;
    for ( it_r = result.begin(); it_r != result.end(); it_r++ ){
      for ( it_rr = it_r->second.begin(); it_rr != it_r->second.end(); it_rr++ ){
	cout << "(" << it_r->first.first << " " << it_r->first.second << ") , (" << it_rr->first.first << " " << it_rr->first.second << ") :: " << it_rr->second << endl;
      }
      }*/
  }
  
  return result;
}

vector < vector < cid > > getPaths ( int start_id, int start_window, vector < unique_ptr < iset < str_com > > >& coms, multimap < cid, cid >& events ){
  vector < vector < cid > > result;
  coms[start_window]->setseen ( start_id );
  cid base = cid ( start_window, start_id );
  pair < map < cid, cid >::iterator, map < cid, cid >::iterator > it_e;
  
  map < cid, cid >::iterator it_res;

  it_e = events.equal_range ( base );
  if ( ( it_e.first != events.end() ) && ( it_e.first != it_e.second) ){
    for ( it_res = it_e.first; it_res != it_e.second; it_res++ ){
      vector < vector < cid > > future_paths = getPaths ( it_res->second.second, it_res->second.first, coms, events );
      for ( unsigned int i = 0; i < future_paths.size(); i++ ){
	future_paths[i].insert ( future_paths[i].begin(), base );
      }
      result.insert ( result.end(), future_paths.begin(), future_paths.end() );
    }
  } else if ( ( (unsigned int) start_window < coms.size() - 1 ) &&  (coms[start_window + 1]->contains ( start_id ) ) ){ 
    result = getPaths ( start_id, start_window + 1, coms, events );
    for ( unsigned int i = 0; i < result.size(); i++ ){
      result[i].insert ( result[i].begin(), base );
    }
  } else {
    vector < cid > solo_path;
    solo_path.push_back ( base );
    result.push_back ( solo_path );
  }

  return result;
}

double Evo_Jaccard ( vector < cid > truth, vector < cid > detected, map < cid, map < cid, double > > similarities, double threshold ){
  if ( ( truth[truth.size() - 1].first < detected[0].first ) || ( detected[detected.size()-1].first < truth[0].first ) ){
    return 0;
  }

  int t = 0, d = 0;
  while ( truth[t].first < detected[d].first ){
    ++t;
  }
  
  while ( detected[d].first < truth[t].first ){
    ++d;
  }
  
  double compared = 0;
  double total = 0;
  
  while ( ( (unsigned int)d < detected.size() ) && ( (unsigned int)t < truth.size() ) ){
    compared++;

    map < cid, map < cid, double > >::iterator it_m;
    map < cid, double >::iterator it_s;
    if ( ( it_m = similarities.find ( truth[t] ) ) != similarities.end() ){
      if ( ( it_s = it_m->second.find ( detected[d] ) ) != it_m->second.end() ){
	total += it_s->second;
      } 
    } 
    
    ++d;
    ++t;
  }

  int to_add1 = detected.size() - compared;
  int to_add2 = truth.size() - compared;
  compared += to_add1 + to_add2;
  
  return total / compared;
}

map < cid, vector < int > > com_evo_mem ( vector < vector < cid > >& evo ){
  map < cid, vector < int > > result;
  map < cid, vector < int > >::iterator it_r;

  for ( unsigned int i = 0; i < evo.size(); i++ ){
    for ( unsigned int j = 0; j < evo[i].size(); j++ ){
      if ( (it_r=result.find (evo[i][j]) ) == result.end() ){
	vector < int > new_list;
	new_list.push_back ( i );
	result.insert ( pair < cid, vector < int > > ( evo[i][j], new_list ) );
      } else {
	it_r->second.push_back ( i );
      }
    }
  }
  
  return result;
}
