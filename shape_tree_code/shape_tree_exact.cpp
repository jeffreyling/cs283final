#include "shape_tree.h"
#include <cmath>
#include <iostream>
#include <assert.h>
//#include <opencv2/opencv.hpp>
#include <Eigen/Dense>
#include <Eigen/SVD>
#include <Eigen/Eigenvalues>

//using namespace cv;
using namespace std;

double cost_table[300][300][300];


Node::Node(Point left_point, Point right_point,
  Point center_point) {
  leftEndpoint = left_point;
  rightEndpoint = right_point;
  midpoint = center_point;
}

ShapeTree::ShapeTree(Node* rootObj, int N_pts, double root_length) {
  key = rootObj;
  leftChild = rightChild = nullptr;
  num_pts = N_pts;
  length = root_length;
}

void ShapeTree::insertLeft(ShapeTree* newNode) {
  if (leftChild == nullptr)
    leftChild = newNode;
  else {
    ShapeTree* t = newNode;
    t->leftChild = leftChild;
    leftChild = t;
  }
}

void ShapeTree::insertRight(ShapeTree* newNode) {
  if (rightChild == nullptr)
    rightChild = newNode;
  else {
    ShapeTree* t = newNode;
    t->rightChild = rightChild;
    rightChild = t;
  }
}

// Recursively builds shape tree from contour.
// I can speed this up by not creating new Contour objects at every level!!!
int global_idx = 0;
ShapeTree* contour_to_shape_tree(const Contour& contour, int s, int e) {
  int n = e - s;

  if (s+2 == e) {
    Node* root_node = new Node(contour[0], contour[1], NONE_POINT);
    double x = contour[0].first-contour[1].first;
    double y = contour[0].second-contour[1].second;
    double cur_len = sqrt(x*x + y*y);
    
    ShapeTree* ret = new ShapeTree(root_node, n, cur_len);
    ret->cost_idx = global_idx;
    ++global_idx;
    return ret;
  } else if (s+1 == e || s == e)
    return nullptr;

  Node* root_node = new Node(contour[s], contour[e], contour[(s+e)/2]);
  ShapeTree* cur_tree = new ShapeTree(root_node, n, 0);
//j //this function is wrong!

//  Contour left_tree(contour.begin(), contour.begin() + n/2 + 1);
//  Contour right_tree(contour.begin() + n/2, contour.end());
  cur_tree->insertLeft(contour_to_shape_tree(contour, 0, n/2 + 1));
  cur_tree->insertRight(contour_to_shape_tree(contour, n/2, n));
  cur_tree->length = cur_tree->leftChild->length + cur_tree->rightChild->length;
  cur_tree->cost_idx = global_idx;
  ++global_idx;
  return cur_tree;
}

// Contour* shape_tree_to_contour(ShapeTree* shape_tree) {
//   int n = shape_tree->num_pts;
//   Node* root = shape_tree->key;
//   if (n == 2) {
//     Contour* ret = new Contour();
//     ret->push_back(root->leftEndpoint);
//     ret->push_back(root->rightEndpoint);
//     return ret;
//   }

//   ShapeTree* left_child = shape_tree->leftChild;
//   ShapeTree* right_child = shape_tree->rightChild;

//   Contour* left_list = shape_tree_to_contour(left_child);
//   Contour* right_list = shape_tree_to_contour(right_child);
//   Contour* ret = new Contour(*left_list);
//   for (int i=0;i<right_list->size();++i)
//     ret->push_back((*right_list)[i]);
//   delete left_list;
//   delete right_list;
//   return ret;
// }

double dif(Point a1, Point a2, Point a3, Point b1, Point b2, Point b3) {
  return procrustes(a1,a2,a3,b1,b2,b3); // needs work
}

// Point bookstein(Point left_point, Point center_point, Point right_point) {
//   double ax = left_point.first;
//   double ay = left_point.second;
//   double bx = right_point.first;
//   double by = right_point.second;
//   double cx = center_point.first;
//   double cy = center_point.second;

//   double ret_x = ((bx-ax)*(cx-ax) + (by-ay)*(cy-ay)) / ((bx-ax)*(bx-ax) + (by-ay)*(by-ay));
//   double ret_y = ((bx-ax)*(cy-ay) - (by-ay)*(cx-ax)) / ((bx-ax)*(bx-ax) + (by-ay)*(by-ay));
//   return make_pair(ret_x, ret_y);
// }

// double dif2(Point a1, Point a2, Point a3, Point b1, Point b2, Point b3) {
//   Point s = bookstein(a1, a2, a3);
//   Point t = bookstein(b1, b2, b3);
// //  printf("%g %g %g %g %g %g\n", a1.first,a1.second,a2.first,a2.second,a3.first,a3.second);
// //  printf("%g %g %g %g\n", s.first,s.second,t.first,t.second);
//   return sqrt((s.first-t.first)*(s.first-t.first) + (s.second-t.second)*(s.second-t.second));
// }

// This is way too slow
// Determines a linear transformation that minimizes sum of squared errors
// We use it only for a triangle. Vectors should have size 3
double procrustes(Point a1, Point a2, Point a3, Point b1, Point b2, Point b3) {
  Eigen::MatrixXd vX(3,2);
  Eigen::MatrixXd vY(3,2);

  vX << a1.first, a1.second,
    a2.first, a2.second,
    a3.first, a3.second;

  vY << b1.first, b1.second,
    b2.first, b2.second,
    b3.first, b3.second;

  Eigen::MatrixXd X0 = vX.rowwise() - vX.colwise().mean();
  Eigen::MatrixXd Y0 = vY.rowwise() - vY.colwise().mean();

  double normX = X0.norm();
  double normY = Y0.norm();

  X0 /= normX;
  Y0 /= normY;

  Eigen::MatrixXd A = X0.transpose() * Y0;

  Eigen::JacobiSVD<Eigen::MatrixXd> svd(A, Eigen::ComputeThinU | Eigen::ComputeThinV);
  //Eigen::MatrixXd U = svd.matrixU();
  //Eigen::MatrixXd V = svd.matrixV();
  //Eigen::MatrixXd T = V * U.transposed();

  double traceTA = svd.singularValues().sum();

  double d = 1 - traceTA * traceTA;
  return d;
}

// double base_cost(const Contour& contour_segment) {
//   int n = contour_segment.size();
//   if (n <= 2)
//     return 0;

//   Point pt_start = contour_segment[0];
//   Point pt_end = contour_segment[n-1];
//   double a = pt_end.second - pt_start.second;
//   double b = pt_start.first - pt_end.first;
//   double c = pt_end.first * pt_start.second - pt_end.second * pt_start.first;
//   if (a == 0 && b == 0)
//     return 0;
//   else {
//     double ret = 0;
//     for (int i=0;i<n;++i) {
//       double x = contour_segment[i].first;
//       double y = contour_segment[i].second;
//       ret += abs(a*x + b*y + c) / sqrt(a*a + b*b);
//     }
//     return ret;
//   }
// }

double base_cost(const Contour& contour_segment, int s, int e) {
  int n = abs(e-s);
  int m = contour_segment.size();
  if (n <= 2)
    return 0;

  Point pt_start = contour_segment[s];
  Point pt_end = contour_segment[e];
  double a = pt_end.second - pt_start.second;
  double b = pt_start.first - pt_end.first;
  double c = pt_end.first * pt_start.second - pt_end.second * pt_start.first;
  if (a == 0 && b == 0)
    return 0;
  else {
    double ret = 0;
    for (int i=s;i != e;i=(i+1)%m) {
      double x = contour_segment[i].first;
      double y = contour_segment[i].second;
      ret += abs(a*x + b*y + c) / sqrt(a*a + b*b);
    }
    return ret;
  }
}

double rec_base_cost(ShapeTree* tree_node, double a, double b, double c) {
  if (tree_node->key->midpoint == NONE_POINT || tree_node == nullptr)
    return 0;

  double x = tree_node->key->midpoint.first;
  double y = tree_node->key->midpoint.second;
  return abs(a*x+b*y+c) / sqrt(a*a + b*b) +
    rec_base_cost(tree_node->leftChild, a,b,c) +
    rec_base_cost(tree_node->rightChild, a,b,c);
}

double base_cost(ShapeTree* tree_node) {
//  return base_cost(*shape_tree_to_contour(tree_node));
  int n = tree_node->num_pts;
  if (n <= 2)
    return 0;

  ShapeTree* leftmost = tree_node;
  while (leftmost->leftChild)
    leftmost = leftmost->leftChild;

  ShapeTree* rightmost = tree_node;
  while (rightmost->rightChild)
    rightmost = rightmost->rightChild;

  double x1 = leftmost->key->leftEndpoint.first;
  double y1 = leftmost->key->leftEndpoint.second;
  double x2 = rightmost->key->rightEndpoint.first;
  double y2 = rightmost->key->rightEndpoint.second;

  double a = y2-y1;
  double b = x1-x2;
  double c = x2*y1-y2*x1;
  if (a == 0 && b == 0)
    return 0;
  else
    return rec_base_cost(tree_node,a,b,c);
}

double rec_compute_cost(ShapeTree* tree_node, const Contour& contour2, int s, int e) {
  int m = contour2.size();

  // Base cases.
  if (s+1 == e || e+1 == s)
    //return base_cost(tree_node);
    return 0;
  else if (s == e)
    return 0;
  else if (tree_node->key->midpoint == NONE_POINT) {
    // Contour c;
    // for (int i=s;i!=e;i=(i+1)%m)
    //   c.push_back(contour2[i]); //(contour2.begin() + s, contour2.begin() + e + 1);
    //return base_cost(contour2, s, e);
    return 0;
  }


  Node* root_node = tree_node->key;


  double& best = cost_table[tree_node->cost_idx][s][e];
  if (best != -1)
    return best;

  best = INF; // SUBJECT TO CHANGE
  double scalar = tree_node->length; // length of contour, subject to change

  // s < e
  for (int i=s; i != e; i = (i+1)%m) {
    double x = procrustes(root_node->leftEndpoint, root_node->midpoint, root_node->rightEndpoint,
      contour2[s], contour2[i], contour2[e]);
    best = min(best, rec_compute_cost(tree_node->leftChild, contour2, s, i) +
      rec_compute_cost(tree_node->rightChild, contour2, i+1, e) +
      scalar * x);
  }

//  cout << s << " " << e << ";" << tree_node->key->points[0].first << " " << tree_node->key->points[0].second << ";" << best << endl;
//  cout << tree_node->cost_idx << " " << s << " " << e << " best:" << best << endl;
  return best;
}

// Main wrapper for dynamic program
double compute_cost(const Contour& contour1, const Contour& contour2) {
  int n = contour1.size();

  global_idx = 0;
  for(int i=0;i<300;++i)
    for(int j=0;j<300;++j)
      for(int k=0;k<300;++k)
        cost_table[i][j][k] = -1;

  ShapeTree* shape_tree1 = contour_to_shape_tree(contour1, 0, n);
  // ShapeTree* asdf = shape_tree1;
  // while (asdf) {
  //   cout << asdf->key->leftEndpoint.first << " " << asdf->key->leftEndpoint.second
  //    << " " << asdf->key->midpoint.first << " " << asdf->key->midpoint.second <<
  //    " " << asdf->key->rightEndpoint.first << " " << asdf->key->rightEndpoint.second << 
  //    " " << asdf->length << " " << asdf->num_pts << endl;
  //    asdf = asdf->leftChild;
  // }

  int m = contour2.size();
  return rec_compute_cost(shape_tree1, contour2, 0, m-1);
}

// REAL function we want to use! Assuming contour1 is closed, split it at its midpoint
// then match two contours on rest of contour2
double compute_cost_closed(const Contour& contour1, const Contour& contour2) {
  int n = contour1.size();
  Contour c1_left(contour1.begin(), contour1.begin() + n/2);
  Contour c1_right(contour1.begin() + n/2, contour1.end());

  global_idx = 0;
  for(int i=0;i<300;++i)
    for(int j=0;j<300;++j)
      for(int k=0;k<300;++k)
        cost_table[i][j][k] = -1;

  ShapeTree* shape_tree_left = contour_to_shape_tree(c1_left, 0, n/2);
  ShapeTree* shape_tree_right = contour_to_shape_tree(c1_right, 0, c1_right.size());
  int m = contour2.size();

  // double best = INF;
  // for (int s=0;s<m;++s) {
  //   for (int e=0;e<m;++e) {
  //     if (e == s) continue; 
  //     double res = rec_compute_cost(shape_tree_left, contour2, s,e) + 
  //       rec_compute_cost(shape_tree_right, contour2, e,s);
  //     if (res < best && res != 0)
  //       best = res;
  //   }
  // }
  // return best;
  return 0;
}
