#include "shape_tree.h"
#include <cmath>
#include <iostream>
#include <assert.h>
//#include <opencv2/opencv.hpp>
#include <Eigen/Dense>
#include <Eigen/SVD>

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
int idx = 0;
ShapeTree* contour_to_shape_tree(const Contour& contour, int s, int e) {
  int n = e - s;

  if (s+2 == e) {
    Node* root_node = new Node(contour[0], contour[1], NONE_POINT);
    double x = contour[0].first-contour[1].first;
    double y = contour[0].second-contour[1].second;
    double cur_len = sqrt(x*x + y*y);
    
    ShapeTree* ret = new ShapeTree(root_node, n, cur_len);
    ret->cost_idx = idx;
    ++idx;
    return ret;
  } else if (s+1 == e || s == e)
    return nullptr;

  Node* root_node = new Node(contour[0], contour[n-1], contour[n/2]);
  ShapeTree* cur_tree = new ShapeTree(root_node, n, 0);

//  Contour left_tree(contour.begin(), contour.begin() + n/2 + 1);
//  Contour right_tree(contour.begin() + n/2, contour.end());
  cur_tree->insertLeft(contour_to_shape_tree(contour, 0, n/2 + 1));
  cur_tree->insertRight(contour_to_shape_tree(contour, n/2, n));
  cur_tree->length = cur_tree->leftChild->length + cur_tree->rightChild->length;
  cur_tree->cost_idx = idx;
  ++idx;
  return cur_tree;
}

Contour* shape_tree_to_contour(ShapeTree* shape_tree) {
  int n = shape_tree->num_pts;
  Node* root = shape_tree->key;
  if (n == 2) {
    Contour* ret = new Contour();
    ret->push_back(root->leftEndpoint);
    ret->push_back(root->rightEndpoint);
    return ret;
  }

  ShapeTree* left_child = shape_tree->leftChild;
  ShapeTree* right_child = shape_tree->rightChild;

  Contour* left_list = shape_tree_to_contour(left_child);
  Contour* right_list = shape_tree_to_contour(right_child);
  Contour* ret = new Contour(*left_list);
  for (int i=0;i<right_list->size();++i)
    ret->push_back((*right_list)[i]);
  delete left_list;
  delete right_list;
  return ret;
}

double dif(Point a1, Point a2, Point a3, Point b1, Point b2, Point b3) {
//   Point a_mean = make_pair((a1.first+a2.first+a3.first)/3, (a1.second+a2.second+a3.second)/3);
//   Point b_mean = make_pair((b1.first+b2.first+b3.first)/3, (b1.second+b2.second+b3.second)/3);
// //  return procrustes(coords1, coords2); // needs work

//   a1 = make_pair(a1.first-a_mean.first, a1.second-a_mean.second);
//   a2 = make_pair(a2.first-a_mean.first, a2.second-a_mean.second);
//   a3 = make_pair(a3.first-a_mean.first, a3.second-a_mean.second);
//   b1 = make_pair(b1.first-b_mean.first, b1.second-b_mean.second);
//   b2 = make_pair(b2.first-b_mean.first, b2.second-b_mean.second);
//   b3 = make_pair(b3.first-b_mean.first, b3.second-b_mean.second);

//   double normX = sqrt(a1.first*a1.first + a1.second*a1.second +
//    a2.first*a2.first + a2.second*a2.second +
//    a3.first*a3.first + a3.second*a3.second);
//   double normY = sqrt(b1.first*b1.first + b1.second*b1.second +
//     b2.first*b2.first + b2.second*b2.second +
//     b3.first*b3.first + b3.second*b3.second);

//   a1 = make_pair(a1.first/normX, a1.second/normX);
//   a2 = make_pair(a2.first/normX, a2.second/normX);
//   a3 = make_pair(a3.first/normX, a3.second/normX);
//   b1 = make_pair(b1.first/normY, b1.second/normY);
//   b2 = make_pair(b2.first/normY, b2.second/normY);
//   b3 = make_pair(b3.first/normY, b3.second/normY);

//   //want: sum of singular values
//   double numer = (b1.first*a1.second - b1.second*a1.first) +
//     (b2.first*a2.second - b2.second*a2.first) +
//     (b3.first*a3.second - b3.second*a3.first);
//   double denom = (b1.first*a1.second + b1.second*a1.first) +
//     (b2.first*a2.second + b2.second*a2.first) +
//     (b3.first*a3.second + b3.second*a3.first);
//   double theta = atan(numer/denom);

//   double costheta = cos(theta);
//   double sintheta = sin(theta);
//   Point c1 = make_pair(costheta*b1.first - sintheta*b1.second,
//     sintheta*b1.first + costheta*b1.second);
//   Point c2 = make_pair(costheta*b2.first - sintheta*b2.second,
//     sintheta*b2.first + costheta*b2.second);
//   Point c3 = make_pair(costheta*b3.first - sintheta*b3.second,
//     sintheta*b3.first + costheta*b3.second);

//   double ans = sqrt((a1.first-c1.first)*(a1.first-c1.first) +
//     (a2.first-c2.first)*(a2.first-c2.first) +
//     (a3.first-c3.first)*(a3.first-c3.first));
  double ax = a1.first, ay = a1.second;
  double bx = a2.first, by = a2.second;
  double cx = a3.first, cy = a3.second;

  double sx = ((bx-ax)*(cx-ax) + (by-ay)*(cy-ay)) / ((bx-ax)*(bx-ax) + (by-ay)*(by-ay));
  double sy = ((bx-ax)*(cy-ay) - (by-ay)*(cx-ax)) / ((bx-ax)*(bx-ax) + (by-ay)*(by-ay));

  ax = b1.first, ay = b1.second;
  bx = b2.first, by = b2.second;
  cx = b3.first, cy = b3.second;
  double tx = ((bx-ax)*(cx-ax) + (by-ay)*(cy-ay)) / ((bx-ax)*(bx-ax) + (by-ay)*(by-ay));
  double ty = ((bx-ax)*(cy-ay) - (by-ay)*(cx-ax)) / ((bx-ax)*(bx-ax) + (by-ay)*(by-ay));

  return sqrt((sx-tx)*(sx-tx) + (sy-ty)*(sy-ty));
}

// This is way too slow
// Determines a linear transformation that minimizes sum of squared errors
// We use it only for a triangle. Vectors should have size 3
double procrustes(const vector<Point>& X, const vector<Point>& Y) {
  assert(X.size() == Y.size());

  Eigen::MatrixXd vX(3,2);
  Eigen::MatrixXd vY(3,2);
  for (int i=0;i<X.size();++i)
    vX.row(i) << X[i].first, X[i].second;
  for (int i=0;i<Y.size();++i)
    vY.row(i) << Y[i].first, Y[i].second;

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

double base_cost(const Contour& contour_segment) {
  int n = contour_segment.size();
  if (n <= 2)
    return 0;

  Point pt_start = contour_segment[0];
  Point pt_end = contour_segment[n-1];
  double a = pt_end.second - pt_start.second;
  double b = pt_start.first - pt_end.first;
  double c = pt_end.first * pt_start.second - pt_end.second * pt_start.first;
  if (a == 0 && b == 0)
    return 0;
  else {
    double ret = 0;
    for (int i=0;i<n;++i) {
      double x = contour_segment[i].first;
      double y = contour_segment[i].second;
      ret += abs(a*x + b*y + c) / sqrt(a*a + b*b);
    }
    return ret;
  }
}

double base_cost(ShapeTree* tree_node) {
  return base_cost(*shape_tree_to_contour(tree_node));
}


double rec_compute_cost(ShapeTree* tree_node, const Contour& contour2, int s, int e) {
  // Base cases.
  if (s+1 == e)
    //return base_cost(tree_node);
    return 0;
  else if (s == e)
    return 0;
  else if (tree_node->key->midpoint == NONE_POINT) {
    //Contour c(contour2.begin() + s, contour2.begin() + e + 1);
    //return base_cost(c);
    return 0;
  }

  int m = contour2.size();
  Node* root_node = tree_node->key;

//  cerr << tree_node->cost_idx << endl;
  double& best = cost_table[tree_node->cost_idx][s][e];
  if (!isnan(best))
    return best;

//  cerr << s << " " << e << endl;

  best = INF; // SUBJECT TO CHANGE
  double scalar = tree_node->length; // length of contour, subject to change
  for (int i=s; i < e; ++i) {
    // vector<Point> coords1;
    // coords1.assign(root_node->points, root_node->points + 3);
    // Point c2[] = {contour2[s], contour2[i], contour2[e]};
    // vector<Point> coords2(c2, c2+3);

    double x = dif(root_node->leftEndpoint, root_node->midpoint, root_node->rightEndpoint,
        contour2[s], contour2[i], contour2[e]);
    best = min(best, rec_compute_cost(tree_node->leftChild, contour2, s, i) +
      rec_compute_cost(tree_node->rightChild, contour2, i+1, e) +
      scalar * x);
  }

//  cout << s << " " << e << ";" << tree_node->key->points[0].first << " " << tree_node->key->points[0].second << ";" << best << endl;

  return best;
}

// Main wrapper for dynamic program
double compute_cost(const Contour& contour1, const Contour& contour2) {
  int n = contour1.size();
  ShapeTree* shape_tree1 = contour_to_shape_tree(contour1, 0, n);

  for(int i=0;i<300;++i)
    for(int j=0;j<300;++j)
      for(int k=0;k<300;++k)
        cost_table[i][j][k] = NAN;
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
