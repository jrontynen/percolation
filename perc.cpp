#include <iostream>
#include <fstream>
using namespace std;

const int L {5};
const int N {L*L};
const int EMPTY {-(N+1)};


// We have N sites with indices 0 ... N-1. 

int ptr[N]; // Array of pointers
int nn[N][4]; // Nearest neighbors
int order[N]; // Occupation order
int dx[N];
int dy[N];


void boundaries() {
  // Defines the nearest neighbours of each site.
	
  for (int i {0}; i<N; i++) {
    nn[i][0] = (i+1)%N;
    nn[i][1] = (i+N-1)%N;
    nn[i][2] = (i+L)%N;
    nn[i][3] = (i+N-L)%N;
		
    if (i%L==0) {
      nn[i][1] = i+L-1;
    }
    if ((i+1)%L==0) {
      nn[i][0] = i-L+1;
    }
  }
}


void permutation() {
  //Defines the order of occupying the sites.
	
  int i,j,temp;
	
  // Initialize the array with a linear order:
  for (i=0; i<N; i++) {
    order[i] = i;
  }
	
  // Permute:
  for (i=0; i<N; i++) {
    //j = i+(N-i)*RNGLONG;
    j = i+rand()%(N-i); //random index in the range (i+1)...(N-1)
		
    //Switch the order of i and j:
    temp = order[i];
    order[i] = order[j];
    order[j] = temp;
  }
}


int findroot(int i) {
  //Finds the root site of the site with an index i. Uses path compression.
	
  //The site with an index i is its own root, i.e. does not point anywhere:
  if (ptr[i]<0) { 
    return [i,0,0];
  }
  
  //Else find the root of the site where the site i points to, and make the site i point to the root:
  [r,dx,dy] = findroot(ptr[i]);
  ptr[i] = r;
  dx[i] = dx + ;
  dy[i] = dy;
  return [r,dx,dy];
}

/*
  int findroot(int i) {
  //Uses path halving.
	
  int r,s;
  r = s = i;
  while (ptr[r]>=0) {
  ptr[s] = ptr[r]; // the new pointer is the pointer of the pointer
  s = r; // copy the value of r
  r = ptr[r]; // move to the pointed site
  }
  return r;
  }
*/


void percolate() {
	
  ofstream f_big;
  f_big.open("biggest.txt");
  f_big << L << endl;
  
  int i,j;
  int s1,s2;
  int r1,r2;
  int big=0;
  int perc_ind=0;

  //Initialize the array of pointers:
  for (i=0; i<N; i++) {
    ptr[i] = EMPTY; // each site points nowhere (negative value) and contains a record of the cluster size (-1)
    dx[i] = 0;
    dy[i] = 0;
  }
	
  //Percolate:
  for (i=0; i<N; i++) {
    r1 = s1 = order[i]; // choose an empty site to be occupied (s1) which is its own root (r1)
    ptr[s1] = -1; // the occupied site points nowhere and has size 1.
    for (j=0; j<4; j++) { // loop over the nearest neighbours
      s2 = nn[s1][j]; // index of a neighbor (s2)
      if (ptr[s2]!=EMPTY) { // if the neighbor is occupied
	[r2,dx2,dy2] = findroot(s2); // find the root of an occupied neighbor (r2)
	if (r2!=r1) { // if the occupied neighbor belongs to a different cluster (roots r1 and r2 are different)
	  if (ptr[r1]>ptr[r2]) { // if the neighbor belongs to a larger cluster
	    ptr[r2] += ptr[r1]; // add the size of the current cluster to the neighboring cluster
	    ptr[r1] = r2; // the occupied site points to the root of the neighboring larger cluster
	    dx[r1] = dx2 + r1%L - ;
	    dy[r1] = dy[r2] - dy[r1];	    
	    r1 = r2; // r1 changes to the root of the larger cluster
	  } 
	  else { // if the neighbor belongs to a smaller cluster
	    ptr[r1] += ptr[r2]; // add the size of the neighboring cluster to the current cluster
	    ptr[r2] = r1; // the root of the neighboring cluster points to the occupied site
	    dx[s2] = dx2 + (r2%L - r1%L);
	    dy[s2] = dy2 + (r2/L - r1/L);
	    // r1 remains unchaged
	  }
	  // Keep track of the size of the biggest cluster:
	  if (-ptr[r1]>big) { // if the size of the joined cluster is larger than the biggest cluster
	    big = -ptr[r1];
	  }
	  //Check for percolation:
	  
	}
      }
    }
  }
  f_big.close();
}



int main(){
  boundaries();
  permutation();
	
  int n {0};
  cout << n << endl;
  for (int i {0}; i<4; i++) {
    cout << nn[n][i] << " ";
  }
  cout << endl;
	
  for (int i {0}; i<N; i++) {
    cout << order[i] << " ";
  }
  cout << endl;
	
	
}
