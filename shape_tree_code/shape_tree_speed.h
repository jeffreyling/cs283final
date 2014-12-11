#ifndef SHAPE_TREE_H
#define SHAPE_TREE_H

#include <string>
#include <vector>
#include <cstdlib>
#include <cstring>

#define INF 900000
#define NONE_POINT (make_pair(-1.0, -1.0))

using namespace std;

typedef pair<double, double> Point;
typedef vector<Point> Contour;

struct Node {
  Node(Point left_point, Point right_point, Point center_point);

  Point leftEndpoint, rightEndpoint, midpoint;
};

class ShapeTree {
public:
  ShapeTree(Node* rootObj, int N_pts, double root_length);
  void insertLeft(ShapeTree* newNode);
  void insertRight(ShapeTree* newNode);

  Node* key;
  ShapeTree* leftChild;
  ShapeTree* rightChild;
  int num_pts;
  double length;

  // DP cost table
//  double cost_table[110][110]; // size....
  int cost_idx;
};

// Recursively builds shape tree from contour.
ShapeTree* contour_to_shape_tree(const Contour& contour);

Contour* shape_tree_to_contour(ShapeTree* shape_tree);

double dif(const vector<Point>& coords1, const vector<Point>& coords2);

double base_cost(const Contour& contour_segment);

double procrustes(const vector<Point>& X, const vector<Point>& Y);

double rec_compute_cost(ShapeTree* tree_node, const Contour& contour2, int s, int e);

// Main function
double compute_cost(const Contour& contour1, const Contour& contour2);

#endif // SHAPE_TREE_H