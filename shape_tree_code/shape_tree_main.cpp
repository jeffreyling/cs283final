#include <iostream>
#include <fstream>
#include <cstdio>
#include <cmath>
#include <iomanip>
#include "shape_tree.h"

int main(int argc, char** argv) {
  if (argc != 3) {
    cerr << "incorrect usage\n";
    return -1;
  }

  string file1(argv[1]);
  string file2(argv[2]);

  ifstream fin1(file1);
  ifstream fin2(file2);

  Contour c1, c2, actual_c1, actual_c2;

  // We need to limit contour size to 100 or so.
  int x,y;

  while (fin1 >> x >> y) {
    c1.push_back(make_pair((double)x,(double)y));
  }
  int inc = c1.size()/75;
  if (inc == 0) inc = 1;
  for (int i=0;i<c1.size();i+=inc)
    actual_c1.push_back(c1[i]);

  while (fin2 >> x >> y) {
    c2.push_back(make_pair((double)x,(double)y));
  }
  inc = c2.size()/75;
  if (inc == 0) inc = 1;
  for (int i=0;i<c2.size();i+=inc)
    actual_c2.push_back(c2[i]);

  fin1.close();
  fin2.close();
  cout << setprecision(17) << compute_cost(actual_c1,actual_c2) << endl;
  // cout << "closed: " << 
  //   setprecision(17) << compute_cost_closed(actual_c1, actual_c2) << endl;
  return 0;
}