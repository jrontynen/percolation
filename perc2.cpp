#include <iostream>
#include <fstream>
#include <cmath>
using namespace std;

const int L {32};
const int N {L*L};
const int EMPTY {-(N+1)};

// We have N sites with indices 0 ... N-1. No array is created for the lattice.
int ptr[N]; // array of pointers
int nn[N][4]; // nearest neighbors
int order[N]; // occupation order
int dx[N] {}; // vertical distance of the path from the site to its root, initialized to zero
int dy[N] {}; // horizontal distance of the path from the site to its root, initialized to zero

const double PMIN {0};
const double PMAX {1};
const int Np {100}; //the # of values of p
double bN[N+1][Np]; //binomial distribution b(N,n,p)

void boundaries(); //sets the topology of the lattice
void permutation(); //randomly permutes the occupation order
int percolate();
int findroot(int);
void binomial();


int main() {

  const int Nruns {10000};
  double n_perc; //percolation treshold
  double perc_p[Np];

  ofstream fwrap;
  string fname {"./out/wrap" + fname + to_string(L) + ".txt"};
  fwrap.open(fname);
  
  srand(time(NULL));
  boundaries(); //set periodic boundary conditions

  
  //Calculate the average percolation treshold:
  n_perc = 0;
  for (int i {0}; i<Nruns; i++) {
    permutation(); //sets a random occupation order
    n_perc += percolate(); //calculates the percolation treshold for each run
  }
  n_perc = n_perc/Nruns; //average value of the percolation treshold
  fwrap << L << " " << Nruns << " " << n_perc << endl;

  
  binomial(); //calculate b(N,n,p) for all n and p
  
  //Calculate thr wrapping probability:
  double p;
  for (int pind {0}; pind<Np; pind++) {
    
    perc_p[pind] = 0;
    for (int n {int(ceil(n_perc))}; n<=N; n++) {
      perc_p[pind] += bN[n][pind];
    }

    p = PMIN + (PMAX-PMIN)/(Np-1)*pind;
    fwrap << p << " "  << perc_p[pind] << endl;
  }
  fwrap.close();
}


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
	
  // Initialize the array with a linear order:
  for (int i {0}; i<N; i++) {
    order[i] = i;
  }
	
  // Permute:
  int j, temp;
  for (const int &i : order) {
    j = i + rand()%(N-i); //random index in the range (i+1)...(N-1)
		
    //Switch the order of i and j:
    temp = order[i];
    order[i] = order[j];
    order[j] = temp;
  }
}


int percolate() {
  
  int i,j;
  int s1,s2;
  int r1,r2;
  int big {0};
  int perc_ind {0};

  //Initialize the pointer array:
  for (i=0; i<N; i++) {
    ptr[i] = EMPTY;
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
	}
	else { //the neighbor belongs to the same cluster
	  //Check for percolation:
	  if (abs(dx[s1]-dx[s2]) + abs(dy[s1]-dy[s2]) != 1 && perc_ind==0) {
	    return i+1;
	  }
	}
      }
    }
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


void binomial() {

  /*ofstream fbin;
  fbin.open("./out/binom_distr.txt");
  fbin << Np <<" "<< N <<" "<< PMIN <<" "<< PMAX <<" "<<endl;*/

  double p, norm_const;
  int n1;
  for (int pind {0}; pind<Np; pind++) {
    
    p = PMIN + (PMAX-PMIN)/(Np-1)*pind;
    
    n1 = p*N; //is integer. b(N,n,p) maximum at n=n1 for fixed N and p
    bN[n1][pind] = 1;
    norm_const = 1;
    int n;
    for (n=n1+1; n<=N; n++) {
      bN[n][pind] = bN[n-1][pind]*(N-n+1)/n*p/(1-p);
      norm_const += bN[n][pind];
    }
    for (n=n1-1; n>=0; n--) {
      bN[n][pind] = bN[n+1][pind]*(n+1)/(N-n)*(1-p)/p;
      norm_const += bN[n][pind];
    }
    for (n=0; n<=N; n++) {
      bN[n][pind] /= norm_const;
    }

    /*for (int n=0; n<=N; n++) {
      fbin << bN[n][pind] << " ";
    }
    fbin << endl;*/
  }
  //fbin.close();
}

