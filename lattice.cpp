#include <iostream>
#include <fstream>
using namespace std;


int main() {

  const int L {5};
  const int N {L*L};
  
  int lat[N];
  
  int i;
  for (i=0; i<N; i++) {
    lat[i] = i;
  }

  ofstream file;
  file.open("lattice.txt");


  for (i=0; i<N; i++) {
    file << lat[i] << " ";
    cout << lat[i] << " ";
    if (lat[i]<10) {
	file << " ";
	cout << " ";
    }
    if (i%L == L-1) {
      file << endl;
      cout << endl;
    }
  }
  file.close();

  
  cout << endl;
  for (i=0; i<N; i++) {
    cout << lat[i]%L << " ";
    if (i%L == L-1) {
      cout << endl;
    }
  }

  cout << endl;
  for (i=0; i<N; i++) {
    cout << lat[i]/L << " ";
    if (i%L == L-1) {
      cout << endl;
    }
  }
	
  return 0;
}
