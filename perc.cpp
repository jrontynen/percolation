#include <iostream>
#include <fstream>
using namespace std;

const int L {9};
const int N {L*L};
const int EMPTY {-(N+1)};


// We have N sites with indices 0 ... N-1. 

int ptr[N]; // Array of pointers
int nn[N][4]; // Nearest neighbors
int order[N]; // Occupation order
int dx[N]; // vertical distance of the path from the site to its root
int dy[N]; // horizontal distance of the path from the site to its root


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
  srand (time(NULL));
  for (i=0; i<N; i++) {
    j = i+rand()%(N-i); //random index in the range (i+1)...(N-1)
		
    //Switch the order of i and j:
    temp = order[i];
    order[i] = order[j];
    order[j] = temp;
  }
}


int findroot(int i) {
  //Finds the root site of the site with an index i.
  //Uses path compression and keeps track of the length of the path.
	
  if (ptr[i]<0) { // the site i does not point anywhere (root site)
    return i;
  }
  dx[i] += dx[ptr[i]]; // add the horizontal distance of the pointer to its own pointer
  dy[i] += dy[ptr[i]]; 
  return ptr[i] = findroot(ptr[i]); // go through the tree until the root site is found
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


void print_init(ofstream & file) {
  
  file << "L=" << L << ", N=" << N << ", EMPTY=" << EMPTY << endl;
  file << endl << "Lattice indices: " << endl;
  
  for (int n=0; n<N; n++) {
    file << n << " ";
    if (n<10) {
      file << " ";
    }
    if (n%L == L-1) {
      file << endl;
    }
  }
  file << endl << "----------------------------------------------" << endl;
}

void print_lattice(int n, ofstream & file) {
  
  int i;
  
  file << "n = " << n << ":" << endl;
    
  for (i=0; i<N; i++) {
    if (ptr[i]!=EMPTY) {
      file << "x ";
    }
    else {
      file << ". ";
    }
    if (i%L == L-1) {
      file << endl;
    }
  }

  file << "ptr:" << endl;
  for (i=0; i<N; i++) {
    if (-9<=ptr[i] && ptr[i]<=-1) file << " ";
    if (0<=ptr[i] && ptr[i]<=9) file << "  ";
    if (ptr[i]>=10) file << " ";
    file << ptr[i] << " ";
    if (i%L == L-1) file << endl;
  }

  file << "dx:" << endl;
  for (i=0; i<N; i++) {
    if (dx[i]>-1) file << " ";
    file << dx[i] << " ";
    if (i%L == L-1) file << endl;
  }

  file << "dy:" << endl;
  for (i=0; i<N; i++) {
    if (dy[i]>-1) file << " ";
    file << dy[i] << " ";
    if (i%L == L-1) file << endl;
  }

  file << endl << "----------------------------------------------" << endl;
}
  



void percolate() {
	
  ofstream f_big, f_mov;
  f_big.open("biggest.txt");
  f_big << "L=" << L << endl << "n big" << endl;
  f_mov.open("movie.txt");
  print_init(f_mov);
  
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
    r1 = s1 = order[i]; // choose an empty site to be occupied (s1) and set r1 = s1 
    ptr[s1] = -1; // the occupied site points nowhere and has size 1.
    for (j=0; j<4; j++) { // loop over the nearest neighbours
      s2 = nn[s1][j]; // index of a neighbor (s2)
      if (ptr[s2]!=EMPTY) { // if the neighbor is occupied
	r2 = findroot(s2); // find the root of an occupied neighbor (r2)	  
	if (r2!=r1) { // if the occupied neighbor belongs to a different cluster (roots r1 and r2 are different)
	  if (ptr[r1]>ptr[r2]) { // if the neighbor belongs to a larger cluster
	    ptr[r2] += ptr[r1]; // add the size of the current cluster to the neighboring cluster
	    ptr[r1] = r2; // the occupied site points to the root of the neighboring larger cluster
	    dx[r1] = -dx[s1] + (j/2-1)*(2*j-1) + dx[s2]; //update the distace from r1 to its new root r2
	    dy[r1] = -dy[s1] + (j/2)*(2*(j/3)-1) + dy[s2];
	    r1 = r2; // r2 is the new root of the combined cluster
	  } 
	  else { // if the neighbor belongs to a smaller or equal size cluster
	    ptr[r1] += ptr[r2]; // add the size of the neighboring cluster to the current cluster
	    ptr[r2] = r1; // the root of the neighboring cluster points to the root of the current cluster
	    dx[r2] = -dx[s2] - (j/2-1)*(2*j-1) + dx[s1]; //update the distace from r2 to its new root r1
	    dy[r2] = -dy[s2] - (j/2)*(2*(j/3)-1) + dy[s1];
	    // r1 remains unchaged
	  }
	  // Keep track of the size of the biggest cluster:
	  if (-ptr[r1]>big) { // if the size of the joined cluster is larger than the biggest cluster
	    big = -ptr[r1];
	    f_big << i+1 << " " << big << endl;
	  } 
	}
	else { //the neighbor belongs to the same cluster
	  //Check for percolation:
	  if (abs(dx[s1]-dx[s2]) + abs(dy[s1]-dy[s2]) != 1 && perc_ind==0) {
	    f_mov << endl << "PERCOLATION!" << endl;
	    f_mov << "The difference of the distances of sites " << s1 <<" and "<< s2;
	    f_mov << " to their shared root site is not equal to unity."<<endl<<endl;
	    perc_ind=1; // do not repeat the percolation message
	  }
	}
      }
    }
    print_lattice(i+1,f_mov);
  }
  f_big.close();
  f_mov.close();
}



int main() {
    boundaries();
    permutation();
    percolate();
	
}
