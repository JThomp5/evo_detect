//gtc - ground truth communities
//gte - evolutions
//devo - detected evolutions

#include <iostream>
#include <string>
#include <set>
#include <vector>
#include "iset.h"

#include <tr1/memory>

#include "../../Libraries/Params/Parameters.h"
#include "../../Libraries/Files/IOX.h"
#include "../../Libraries/Files/StringEx.h"

#include "file_handler.h"

using namespace std;


int main ( int argc, char** argv ){
  shared_ptr < Parameters > P ( new Parameters() );
  P->Read ( argc, argv );
  
  double community_matching_threshold = 0.5;
  double evolution_matching_threshold = 0.5;

  

  //Read in the ground truth evolutions
  cerr << "Reading in ground truths" << endl;
  vector < unique_ptr < iset <str_com> > > ground_truth;
  multimap < cid, cid > gt_evolutions;
  ifstream grin;
  openFileHarsh ( &grin, P->get < string > ( "gtc", "ground_truth_communities.dat" ) );
  vector < string > fields;
  while ( fline_tr ( &grin, &fields, " " ) ){
    for ( unsigned int i = 0; i < fields.size(); i++ ){
      ground_truth.push_back ( load_ground_truth_com_file ( fields[i] ) );
    }
  }

  grin.close();

  openFileHarsh ( &grin, P->get < string > ( "gte", "ground_truth_evolutions.dat") );
  
  int time_window = 0;
  while ( fline_tr ( &grin, &fields, " " ) ){
    for ( unsigned int i = 0; i < fields.size(); i++ ){
      load_ground_truth_evo_file ( fields[i] , time_window++, gt_evolutions );
    }
  }

  vector < vector < cid > > full_gt_evo;

  for ( unsigned int i = 0; i < ground_truth.size(); i++ ){
    map < int, set < str_com >::iterator >::const_iterator it_m;
    for (it_m = ground_truth[i]->istart(); it_m != ground_truth[i]->iend(); it_m++ ){
      if ( it_m->second->seen == false ){
	vector < vector < cid > > to_add = getPaths ( it_m->first, i, ground_truth, gt_evolutions );
	for ( int t = 0; t < to_add.size(); t++ ){
	  if ( to_add[t].size() < 3 ){
	    swap(to_add[t], to_add[to_add.size() - 1]);
	    to_add.pop_back();
	    --t;
	  }
	}
        full_gt_evo.insert ( full_gt_evo.end(), to_add.begin(), to_add.end() );
      }
    }
  }

  /*for ( unsigned int i = 0; i < full_gt_evo.size(); i++ ){
    for ( unsigned int j = 0; j < full_gt_evo[i].size(); j++ ){
      cid p = full_gt_evo[i][j];
      cout << "(" << p.first << ", " << p.second << ") ";
    }
    cout << endl;
    }*/

  //Read in the detected evolutions
  //     Probably need a different function for each algorithm
  cerr << "Reading in detected structure" << endl;
  vector < unique_ptr < iset < str_com > > > detected;
  vector < vector < cid > > det_evo;
  detected = load_rpi_evo_file (P->get < string > ( "devo", "detected_evolutions.dat" ), det_evo );

  map < cid, map < cid, double > > community_similarities = compare_community_structures ( ground_truth, detected );

  cerr << "Getting indexing of evolutions done" << endl;
  map < cid, vector < int > > gt_com_to_evo = com_evo_mem ( full_gt_evo ), det_com_to_evo = com_evo_mem(det_evo);
  set < cid > evo_pairs;
  map < cid, vector < int > >::iterator it_g, it_d;
  
  for ( it_g = gt_com_to_evo.begin(); it_g != gt_com_to_evo.end(); it_g++ ){
    if ( (it_d = det_com_to_evo.find ( it_g->first ) ) != det_com_to_evo.end() ){
      for ( unsigned int k = 0; k < it_g->second.size(); k++ ){
	for ( unsigned int t = 0; t < it_d->second.size(); t++ ){
	  evo_pairs.insert ( cid ( it_g->second[k], it_d->second[t])  );
	}
      }
    }
  }
  
  cerr << "Matching " << evo_pairs.size() << endl;
  set < cid >::iterator it_sc;
  map < int, pair < int, double > > matches;
  map < int, pair < int, double > >::iterator it_mm;
  
  unsigned int count = 0;
  for ( it_sc = evo_pairs.begin(); it_sc != evo_pairs.end(); it_sc++ ){
    if ( ++count % 100 == 0 ) cerr << count << endl;
    double current_match = Evo_Jaccard ( full_gt_evo[it_sc->first], det_evo[it_sc->second], community_similarities, community_matching_threshold );
    if ( current_match < evolution_matching_threshold ) continue;

    if ( ( it_mm = matches.find ( it_sc->first ) ) != matches.end() ){
      if ( it_mm->second.second < current_match ) {
	it_mm->second = pair < int, double > ( it_sc->second, current_match );
      }
    } else {
      matches.insert ( pair < int , pair < int, double > > ( it_sc->first, pair < int, double > ( it_sc->second, current_match ) ) );
    }
  }
  
  set < int > det_matched;
  for ( it_mm = matches.begin(); it_mm != matches.end(); it_mm++ ){
    det_matched.insert ( it_mm->second.first );
  }

  cout << "True Positives: " << matches.size() << endl;
  cout << "False Positives: " << det_evo.size() - det_matched.size() << endl;
  cout << "False Negatives: " << full_gt_evo.size() - matches.size() << endl;

  cout << "Similarity measures: ";
  for ( it_mm = matches.begin(); it_mm != matches.end(); it_mm++ ){
    cout << it_mm->second.second << " ";
  }
  cout << endl;

}
