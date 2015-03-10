#include <iostream>
#include <tuple>

using namespace std;

int main ( int argc, char** argv ){
  std::tuple<int, int> a( 2, 3 );
  std::tuple<int, int> b( 3, 2 );
  std::tuple<int, int> c( 2, 3 );

  cout << (a == b) << endl;
  cout << (a == c) << endl;
}
