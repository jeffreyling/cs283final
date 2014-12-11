#include "shape_tree_v2.h"
#include <cmath>
#include <iostream>
#include <assert.h>
//#include <opencv2/opencv.hpp>
#include <Eigen/Dense>
//#include <Eigen/SVD>
#include <Eigen/Eigenvalues>

#define DEBUG 0

// Do I even want to use sqrt at any point?
using namespace std;

double cost_table[300][300][300];
Node shape_tree[300]; // Limit to 300 contour shape trees, length < 100 each

int total_ncontours = 0;

Node::Node() : leftEndpoint(NONE_POINT), rightEndpoint(NONE_POINT), midpoint(NONE_POINT),
  num_pts(0), length(-1) {}

Node::Node(Point left_point, Point right_point,
  Point center_point, int N_pts, double root_length) :
  leftEndpoint(left_point), rightEndpoint(right_point), midpoint(center_point),
  num_pts(N_pts), length(root_length) {}

// Recursively builds shape tree (global var) from contour.
// e is not inclusive
void contour_to_shape_tree(const Contour& contour, int s, int e, int cur_idx) {
  int n = e - s;
  Node& cur_node = shape_tree[cur_idx];
  if (n == 2) {
    cur_node.leftEndpoint = contour[s];
    cur_node.rightEndpoint = contour[e-1];
    double x = contour[s].first-contour[e-1].first;
    double y = contour[s].second-contour[e-1].second;
    cur_node.length = sqrt(x*x + y*y); // I'll just have length is squared length
    return;
  } else if (n == 1 || n == 0)
    return;

  cur_node.leftEndpoint = contour[s];
  cur_node.rightEndpoint = contour[e-1];
  cur_node.midpoint = contour[(s+e)/2];
  contour_to_shape_tree(contour, s, (s+e)/2 + 1, 2*cur_idx);
  contour_to_shape_tree(contour, (s+e)/2, e, 2*cur_idx+1);
  cur_node.length = shape_tree[2*cur_idx].length + shape_tree[2*cur_idx+1].length;
}

double x1,y1p,x2,y2,x3,y3,w1,z1,w2,z2,w3,z3;
double muAx,muAy,muBx,muBy;
double normA,normB;
double r1,r2,theta;
double ct,st,ans;
double dif(Node& node_x, const Contour& c, int s,int i,int e) {
  x1 = node_x.leftEndpoint.first;
  y1p = node_x.leftEndpoint.second;
  x2 = node_x.midpoint.first;
  y2 = node_x.midpoint.second;
  x3 = node_x.rightEndpoint.first;
  y3 = node_x.rightEndpoint.second;
  w1 = c[s].first;
  z1 = c[s].second;
  w2 = c[i].first;
  z2 = c[i].second;
  w3 = c[e].first;
  z3 = c[e].second;
  if(DEBUG)
    cout << x1 << " " << y1p << " " << x2 << " " << y2 << " " << x3 <<
    " " << y3 << ";" << w1 << " " << z1 << " " << w2 << " " << z2 <<
    " " << w3 << " " << z3 << endl;
  // cout << "nnn: " << ((w1*y1p-z1*x1)+(w2*y2-z2*x2)+(w3*y3-z3*x3)) << endl;

  // Center and scale
  muAx = (x1+x2+x3)/3;
  muAy = (y1p+y2+y3)/3;
  muBx = (w1+w2+w3)/3;
  muBy = (z1+z2+z3)/3;
  x1 -= muAx;  x2 -= muAx;  x3 -= muAx;
  y1p -= muAy;  y2 -= muAy;  y3 -= muAy;
  w1 -= muBx;  w2 -= muBx;  w3 -= muBx;
  z1 -= muBy;  z2 -= muBy;  z3 -= muBy;

  normA = sqrt(x1*x1+y1p*y1p+x2*x2+y2*y2+x3*x3+y3*y3);
  normB = sqrt(w1*w1+z1*z1+w2*w2+z2*z2+w3*w3+z3*z3);
  x1 /= normA;  x2 /= normA;  x3 /= normA;
  y1p /= normA;  y2 /= normA;  y3 /= normA;
  w1 /= normB;  w2 /= normB;  w3 /= normB;
  z1 /= normB;  z2 /= normB;  z3 /= normB;
  if(DEBUG)
  cout << "scaled: " << x1 << " " << y1p << " " << x2 << " " << y2 << " " << x3 <<
  " " << y3 << ";" << w1 << " " << z1 << " " << w2 << " " << z2 <<
  " " << w3 << " " << z3 << endl;

  r1 = (w1*y1p-z1*x1)+(w2*y2-z2*x2)+(w3*y3-z3*x3);
  r2 = (w1*x1+z1*y1p)+(w2*x2+z2*y2)+(w3*x3+z3*y3);
  theta = atan(r1/r2);
  if(DEBUG)
  cout << "ratio:" << r1/r2 << " theta:" << theta << endl;

  // Theta or theta+pi: both become negative
  ct = cos(theta);
  st = sin(theta);
  if(DEBUG)
  cout << "transformed:" << (ct*w1 - st*z1) << " " << (st*w1 + ct*z1) <<
  " " << (ct*w2 - st*z2) << " " << (st*w2 + ct*z2) <<
  " " << (ct*w3 - st*z3) << " " << (st*w3 + ct*z3) << endl;
  ans = (x1 - ct*w1 + st*z1)*(x1 - ct*w1 + st*z1) +
    (y1p - st*w1 - ct*z1)*(y1p - st*w1 - ct*z1) +
    (x2 - ct*w2 + st*z2)*(x2 - ct*w2 + st*z2) +
    (y2 - st*w2 - ct*z2)*(y2 - st*w2 - ct*z2) +
    (x3 - ct*w3 + st*z3)*(x3 - ct*w3 + st*z3) +
    (y3 - st*w3 - ct*z3)*(y3 - st*w3 - ct*z3);
  double ans2 = (x1 + ct*w1 - st*z1)*(x1 + ct*w1 - st*z1) +
    (y1p + st*w1 + ct*z1)*(y1p + st*w1 + ct*z1) +
    (x2 + ct*w2 - st*z2)*(x2 + ct*w2 - st*z2) +
    (y2 + st*w2 + ct*z2)*(y2 + st*w2 + ct*z2) +
    (x3 + ct*w3 - st*z3)*(x3 + ct*w3 - st*z3) +
    (y3 + st*w3 + ct*z3)*(y3 + st*w3 + ct*z3);
  if(DEBUG)
  cout << "ans:" << ans << endl;
  return min(ans,ans2);
}

// Main wrapper for dynamic program
double compute_cost(const Contour& contour1, const Contour& contour2) {
  int n = contour1.size();
  int m = contour2.size();
  contour_to_shape_tree(contour1, 0, n, 1);

  // We'll do it iteratively.
  double scalar = 0;
  double x = 0, y = 0;

  // Test:
  // Node test_node(make_pair(1,1),make_pair(3,3),make_pair(2,2),3,1);
  // vector<Point> test_c;
  // test_c.push_back(make_pair(1,1));
  // test_c.push_back(make_pair(2,2));
  // test_c.push_back(make_pair(3,3));
  // cout << dif(test_node, test_c, 0,1,2) << endl;

  // For contour2
  double len_prefix[300];
  len_prefix[0] = 0;
  for (int i=1;i<m;++i) {
    x = contour2[i].first - contour2[i-1].first;
    y = contour2[i].second - contour2[i-1].second;
    len_prefix[i] = len_prefix[i-1] + sqrt(x*x + y*y);
  }


  for (int v=290;v >= 1;--v) {
    Node& cur_node = shape_tree[v];

    // Base case
    if (cur_node.midpoint == NONE_POINT) {
      if (cur_node.leftEndpoint != NONE_POINT) {
        for (int s=0;s<m;++s) {
          for (int e=0;e<m;++e) {
            cost_table[v][s][e] = 0;
            if (e-s != 2 || e-s != 1 || e-s != 0) {
              // Base case when contour1 is a line segment
              if (s < e)
                cost_table[v][s][e] = len_prefix[e] - len_prefix[s];
              else
                cost_table[v][s][e] = len_prefix[m-1] - len_prefix[s] + len_prefix[e];
            }
          }
        }
      }
      continue;
    }

    for (int s=0;s<m;++s) {
      for (int e=0;e<m;++e) {
        if (e-s == 0) {
          cost_table[v][s][e] = 0;
          continue;
        } else if (e-s == 1) {
          cost_table[v][s][e] = 0;
          if (cur_node.num_pts > 2) {
            // Base case when contour2 is a line segment
            cost_table[v][s][e] = cur_node.length;
          }
          continue;
        }
        cost_table[v][s][e] = INF;
        scalar = cur_node.length; // for now
        for (int i=s;i!=e;i=(i+1)%m) {
          x = dif(shape_tree[v], contour2, s,i,e);
          y = cost_table[2*v][s][i] + cost_table[2*v+1][i][e] +
            scalar * x;
          if (y < cost_table[v][s][e]) {
            cost_table[v][s][e] = y;
            // set parent?
          }
        }
      }
    }


  }

  // Communicate result to stdout
  // The cost of matching contour1 to the subcontour [s,e]
  double best_cost = INF;
  int best_s=0,best_e=m-1;
  for (int s=0;s<m;++s) {
    for (int e=0;e<m;++e) {
      printf("%d %d %g\n",s,e,cost_table[1][s][e]);
      if (cost_table[1][s][e] < best_cost &&
        cost_table[1][s][e] > 0) {
        best_cost = cost_table[1][s][e];
        best_s = s; best_e = e;
      }
    }
  }

  printf("best %d %d %g\n",best_s,best_e,best_cost);

  double best_overall = cost_table[1][0][m-1];
  int best_i=0;
  for (int i=1;i<m;++i) {
    if (cost_table[1][i][i-1] < best_overall) {
      best_i = i;
      best_overall = cost_table[1][i][i-1];
    }
  }
  printf("overall %d %g\n", best_i, best_overall);

  return best_overall;
}

// REAL function we want to use! Assuming contour1 is closed, split it at its midpoint
// then match two contours on rest of contour2
// double compute_cost_closed(const Contour& contour1, const Contour& contour2) {
//   int n = contour1.size();
//   Contour c1_left(contour1.begin(), contour1.begin() + n/2);
//   Contour c1_right(contour1.begin() + n/2, contour1.end());

//   global_idx = 0;
//   for(int i=0;i<300;++i)
//     for(int j=0;j<300;++j)
//       for(int k=0;k<300;++k)
//         cost_table[i][j][k] = -1;

//   ShapeTree* shape_tree_left = contour_to_shape_tree(c1_left, 0, n/2);
//   ShapeTree* shape_tree_right = contour_to_shape_tree(c1_right, 0, c1_right.size());
//   int m = contour2.size();

//   double best = INF;
//   for (int s=0;s<m;++s) {
//     for (int e=0;e<m;++e) {
//       if (e == s) continue; 
//       double res = rec_compute_cost(shape_tree_left, contour2, s,e) + 
//         rec_compute_cost(shape_tree_right, contour2, e,s);
//       if (res < best && res != 0)
//         best = res;
//     }
//   }
//   return best;
// }
