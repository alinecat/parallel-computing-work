#include <iostream>
#include <vector>
#include <fstream>
#include <string>
#include <unordered_map>
#include <algorithm>
#include <unordered_set>
#include <iomanip>
#include <omp.h>
#include <chrono>
#include <cmath>
using namespace std;


struct CSR {
  int _n;
  int _m;
  int *_xadj;
  int *_adj;
  int *_is;
  CSR(int n, int m, int* xadj, int* adj, int* is):
    _n(n), _m(m), _xadj(xadj), _adj(adj), _is(is){}
};

CSR create_csr_from_file(string filename);
void print_jaccards(string filename, int n, int *xadj, int *adj, float *jacc);


int main(int argc, char *argv[]){
  if (argc < 3){
    cout << "Parameters: input_file output_file\n";
    return 1;
  }
  string input_file = argv[1];
  string output_file = argv[2];

  CSR graph = create_csr_from_file(input_file); // Creates a graph in the Compressed Sparse Row format from the input file
  int n = graph._n, m = graph._m, *xadj = graph._xadj, *adj = graph._adj, *is = graph._is;
  cout << "Read the graph into CSR format" << endl;

  float * jaccard_values = new float[m];
  // jaccard_values[j] = Jaccard between vertices (a, b) where adj[j] = b and adj[a] < j < adj[a+1]
  // i.e. the edge located in index j of the adj array

  auto start = chrono::steady_clock::now();

  /////// BEGIN CALCULATION CODE

  int thread_cnt = 40;
  int u,v,v_ptr,num_union, u_nbr_ptr, u_nbr, v_nbr_ptr,v_nbr ;

#pragma omp parallel for num_threads(thread_cnt) schedule(dynamic,16) private(num_union,u,v,v_ptr, u_nbr_ptr, u_nbr, v_nbr_ptr,v_nbr)
  for (u = 0; u < n; u++){
    int p_xadj_bnd_0 = xadj[u] ;
    int p_xadj_bnd_1 = xadj[u+1] ;
    for (v_ptr = p_xadj_bnd_0; v_ptr < p_xadj_bnd_1; v_ptr++){
      v = adj[v_ptr]; // v is a neighbor of
      int p_xadj_bnd_2 = xadj[v] ;
      int p_xadj_bnd_3 = xadj[v+1] ;
      num_union = (p_xadj_bnd_1-p_xadj_bnd_0)+(p_xadj_bnd_3-p_xadj_bnd_2); // The set contains all the non-repeated neighbors of u and v
      int num_intersections = 0;
      for (u_nbr_ptr = p_xadj_bnd_0; u_nbr_ptr < p_xadj_bnd_1; u_nbr_ptr++){ // Go over all neighbors of u
        u_nbr = adj[u_nbr_ptr];
        num_intersections = num_intersections ;
        for (v_nbr_ptr = p_xadj_bnd_2; v_nbr_ptr < p_xadj_bnd_3; v_nbr_ptr++){ // Go over all neighbors of v
          v_nbr = adj[v_nbr_ptr];
          num_intersections = num_intersections ;
          if (u_nbr == v_nbr){ // Neighbors of u and v match. Increment the intersections
            num_intersections++;

          }
        }
      }
      num_union -= num_intersections; // The set contains all the non-repeated neighbors of u and v
      jaccard_values[v_ptr] = num_intersections/float(num_union) ;

    }
  }

  /////// END CALCULATION CODE
  auto end = chrono::steady_clock::now();
  auto diff = end - start;

  cout << "Finished calculating the Jaccards in " << chrono::duration <double> (diff).count() << " seconds" << endl;

  print_jaccards(output_file, n, xadj, adj, jaccard_values);
  cout << "Finished printing the Jaccards" << endl;

  return 0;
}

void print_jaccards(string output_file, int n, int *xadj, int *adj, float *jacc){
  ofstream fout(output_file);
  // Save flags/precision.
  ios_base::fmtflags oldflags = cout.flags();
  streamsize oldprecision = cout.precision();

  cout << fixed;
  for (int u = 0; u < n; u++){
    for (int v_ptr = xadj[u]; v_ptr < xadj[u+1]; v_ptr++){
      fout << "(" << u << ", " << adj[v_ptr] << "): " << fixed << setprecision(3) << jacc[v_ptr] << endl;
      std::cout.flags (oldflags);
      std::cout.precision (oldprecision);
    }
  }
}

CSR create_csr_from_file(string filename){
  ifstream fin(filename);
  if (fin.fail()){
    cout << "Failed to open graph file\n";
    throw -1;
  }
  int n=0, m=0, *xadj, *adj, *is;

  fin >> n >> m;
  vector<vector<int>> edge_list(n);
  int u, v;
  int read_edges = 0;
  while (fin >> u >> v){
    if (u < 0){
      cout << "Invalid vertex ID - negative ID found: " << u << endl;
      throw -2;
    }
    if (u >= n){
      cout << "Invalid vertex ID - vertex ID > number of edges found. VID: " << u << " and n: " << n << endl;
      throw -2;
    }
    edge_list[u].push_back(v);
    read_edges+=1;
  }
  if (read_edges!=m){
    cout << "The edge list file specifies there are " << m << " edges but it contained " << read_edges << "instead" << endl;
    throw -3;
  }
  
    /////// If CSR is sorted
  for (auto & edges : edge_list){
    sort(edges.begin(), edges.end());
  }
  ///////
  xadj = new int[n+1];
  adj = new int[m];
  is = new int[m];
  int counter = 0;
  for (int i = 0; i<n; i++){
    xadj[i] = counter;
    copy(edge_list[i].begin(), edge_list[i].end(), adj+counter);
    counter+=edge_list[i].size();
  }
  xadj[n] = counter;
  for (int i =0; i<n; i++){
    for (int j =xadj[i]; j<xadj[i+1];j++){
      is[j] = i;
    }
  }
  CSR graph(n, m, xadj, adj, is);
  return graph;
}
