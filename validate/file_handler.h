#ifndef RPI_EVO_FILE_HANDLE
#define RPI_EVO_FILE_HANDLE

#include "iset.h"
#include <tr1/memory>

#include "../../MyLibraries/Params/Parameters.h"
#include "../../MyLibraries/Files/IOX.h"
#include "../../MyLibraries/Files/StringEx.h"

unique_ptr < iset <str_com> > load_ground_truth_com_file ( const string& filename );
void load_ground_truth_evo_file ( const string& filename, int window, multimap < cid, cid >& evolutions );
vector < unique_ptr < iset <str_com> > > load_rpi_evo_file ( const string& filename, vector < vector < cid > >& evolutions );

map < cid, map < cid, double > > compare_community_structures ( vector < unique_ptr < iset < str_com > > >& truth, vector < unique_ptr < iset < str_com > > >& detected );

vector < vector < cid > > getPaths ( int start_id, int start_window, vector < unique_ptr < iset < str_com > > >& coms, multimap < cid, cid >& events );

double Evo_Jaccard ( vector < cid > truth, vector < cid > detected, map < cid, map < cid, double > > similarities, double threshold );

map < cid, vector < int > > com_evo_mem ( vector < vector < cid > >& evo );
#endif

