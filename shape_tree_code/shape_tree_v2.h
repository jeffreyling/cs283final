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
  Node();
  Node(Point left_point, Point right_point, Point center_point, int N_pts, double root_length);

  Point leftEndpoint, rightEndpoint, midpoint;
  int num_pts;
  double length;
};

// Recursively builds shape tree from contour.
void contour_to_shape_tree(const Contour& contour, int s, int e, int cur_idx);

//Contour* shape_tree_to_contour(ShapeTree* shape_tree);

// double dif(Point a1, Point a2, Point a3, Point b1, Point b2, Point b3);

// double base_cost(const Contour& contour_segment);

// double procrustes(Point a1, Point a2, Point a3, Point b1, Point b2, Point b3);

// double rec_compute_cost(ShapeTree* tree_node, const Contour& contour2, int s, int e);

// Main function
double compute_cost(const Contour& contour1, const Contour& contour2);
double compute_cost_closed(const Contour& contour1, const Contour& contour2);


#endif // SHAPE_TREE_H