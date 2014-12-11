import numpy as np
import procrustes

INF = 90000 # Large enough?

class Node:
  """
  A node in the binary shape tree
  Contains the left, right, and midpoint of a subcurve
  """
  def __init__(self, left_point, right_point, center_point):
    self.leftEndpoint = left_point
    self.rightEndpoint = right_point
    self.midpoint = center_point
    
    # Necessary?
    # self.coordinates = bookstein(left_point, right_point, center_point)
    self.points = np.array([left_point, center_point, right_point])

  def __str__(self):
    return self.points.__str__()

class ShapeTree:
  def __init__(self, rootObj, num_pts, c_length=0):
    self.key = rootObj
    self.leftChild = None
    self.rightChild = None
    self.num_pts = num_pts
    self.length = c_length

    # DP cost table
    self.cost_table = [[-1 for i in xrange(500)] for j in xrange(500)]

  # Assume newNode is a ShapeTree already, with rootObj a Node
  def insertLeft(self, newNode):
    if self.leftChild == None:
      self.leftChild = newNode
    else:
      t = newNode
      t.leftChild = self.leftChild
      self.leftChild = t

  def insertRight(self, newNode):
    if self.rightChild == None:
      self.rightChild = newNode
    else:
      t = newNode
      t.rightChild = self.rightChild
      self.rightChild = t


def bookstein(left_point, center_point, right_point):
  """A function for computing bookstein coordinates"""
  ax,ay = [float(x) for x in left_point]
  bx,by = [float(x) for x in right_point]
  cx,cy = [float(x) for x in center_point]

  scx = ((bx-ax)*(cx-ax) + (by-ay)*(cy-ay)) / ((bx-ax)*(bx-ax) + (by-ay)*(by-ay))
  scy = ((bx-ax)*(cy-ay) - (by-ay)*(cx-ax)) / ((bx-ax)*(bx-ax) + (by-ay)*(by-ay))
  return (scx,scy)

def list_avg(l1, l2):
  assert len(l1) == len(l2)
  return [float(sum(x))/len(l1) for x in zip(l1,l2)]

def contour_to_shape_tree(contour):
  """Recursively builds shape tree from contour"""
  n = len(contour) # number of rows

  # Base case: make it a line segment of 2 points
  if n == 2:
    root_node = Node(contour[0], contour[1], None) #list_avg(contour[0], contour[1]))
    cur_length = np.linalg.norm(np.array(contour[0]) - np.array(contour[1]))
    return ShapeTree(root_node, n, cur_length)
  elif n == 1 or n == 0:
    return None

  root_node = Node(contour[0], contour[n-1], contour[n/2])
  cur_tree = ShapeTree(root_node, n)
  cur_tree.insertLeft(contour_to_shape_tree(contour[0:(n/2+1)]))
  cur_tree.insertRight(contour_to_shape_tree(contour[(n/2):n]))
  cur_tree.length = cur_tree.leftChild.length + cur_tree.rightChild.length
  return cur_tree

def shape_tree_to_contour(shape_tree):
  """Reconstructs contour from shape tree"""
  n = shape_tree.num_pts
  root = shape_tree.key
  if n == 2:
    return np.vstack((root.leftEndpoint, root.rightEndpoint))

  left_child = shape_tree.leftChild
  right_child = shape_tree.rightChild
  return np.vstack((shape_tree_to_contour(left_child)[:-1], shape_tree_to_contour(right_child)))

### Dynamic programming functions: uses top-down memoization

def dif(coords1, coords2):
  """Input: two np arrays"""
  return procrustes.procrustes(coords1, coords2)[0]

def dif2(coords1, coords2):
  """Another attempt at speed........... """
  coords1 = coords1.tolist()
  coords2 = coords2.tolist()
  (sx,sy) = bookstein(coords1[0], coords1[1], coords1[2])
  (tx,ty) = bookstein(coords2[0], coords2[1], coords2[2])

#  print coords1
 # print sx,sy,tx,ty
  return np.sqrt((sx-tx)*(sx-tx) + (sy-ty)*(sy-ty))

def dif_arr(coords1, coords2):
  """Input: block diagonal of 3 by 2 shape coords of two shapes"""
  # Each block diagonal will have 2 singular values
  d_list = procrustes.procrustes2(coords1, coords2)
  return d_list

# def euclidean_dist(pt1, pt2):
#   x1,y1 = pt1
#   x2,y2 = pt2
#   return np.sqrt((x1-x2)*(x1 - x2) + (y1-y2)*(y1-y2))

def rec_base_cost(contour_segment, a,b,c):
  if contour_segment.key.midpoint == None or contour_segment == None:
    return 0

  x,y = contour_segment.key.midpoint
  return abs(a*x+b*y+c) / np.sqrt(a*a + b*b) + \
    rec_base_cost(contour_segment.leftChild,a,b,c) + \
    rec_base_cost(contour_segment.rightChild,a,b,c)


# Doesn't quite give 0 for some cases (has to do with even/odd indexing)
def base_cost(contour_segment):
  """Given a contour segment, compute cost with line segment"""
  # contour_list = contour_segment
  # if contour_segment.__class__.__name__ == 'ShapeTree':
  #   if contour_segment.num_pts <= 2:
  #     return 0
  #   contour_list = shape_tree_to_contour(contour_segment)

  n = 0
  if contour_segment.__class__.__name__ == 'ShapeTree':
    n = contour_segment.num_pts
  else:
    n = len(contour_segment)

  if n <= 2:
    return 0

  # Find a transformation
  x1,y1,x2,y2 = 0,0,0,0
  if contour_segment.__class__.__name__ == 'ShapeTree':      
    leftmost = contour_segment
    while leftmost.leftChild != None:
      leftmost = leftmost.leftChild
    rightmost = contour_segment
    while rightmost.rightChild != None:
      rightmost = rightmost.rightChild
    x1,y1 = leftmost.key.leftEndpoint
    x2,y2 = rightmost.key.rightEndpoint
  else:
    x1,y1 = contour_segment[0]
    x2,y2 = contour_segment[-1]

  a = y2 - y1
  b = -(x2-x1)
  c = x2*y1 - y2*x1
  if a == 0 and b == 0:
    return 0
  else:
    if contour_segment.__class__.__name__ == 'ShapeTree':
      return rec_base_cost(contour_segment, a,b,c)
    else:
      return sum([abs(a*x+b*y+c) / np.sqrt(a*a + b*b) for x,y in contour_segment])

  # return base_cost([left_pt, mid_pt], contour_segment[0:(n/2 + 1)]) + \
  # base_cost([mid_pt, right_pt], contour_segment[(n/2):n]) + \
  # dif(coords1, np.array([contour_segment[0], contour_segment[n/2], contour_segment[n-1]]))

def rec_compute_cost(tree_node, contour2, s, e):
  """s to e inclusive"""

  # Base case: when we are comparing a segment to a contour
  if s == e:
    return 0
  if s+1 == e:
    return base_cost(tree_node)
  elif tree_node.key.midpoint == None:
    return base_cost(contour2[s:(e+1)])

  m = len(contour2)
  root_node = tree_node.key
  if not hasattr(root_node, 'cost_table'):
    root_node.cost_table = [[-1 for i in xrange(m)] for j in xrange(m)]

  best = root_node.cost_table[s][e]
  if best != -1:
    return best
  
  best = INF
  scalar = tree_node.length # SUBJECT TO CHANGE! should actually be length, not number of points

  # Need to compute difs in one shot
#  coords_mat1 = np.array([root_node.points for i in xrange(s,e)]).reshape((2*(e-s),3)).T
 # coords_mat2 = np.array([[contour2[s], contour2[i], contour2[e]] for i in xrange(s,e)]).reshape((2*(e-s),3)).T
  
  #x_arr = dif_arr(coords_mat1, coords_mat2)

  for i in xrange(s,e):
#    x = dif(root_node.points, np.array([contour2[s], contour2[i], contour2[e]]))
    x = dif2(root_node.points, np.array([contour2[s], contour2[i], contour2[e]]))
    #print s,e,i,x, root_node.points, contour2[s], contour2[i], contour2[e]
    #raw_input()
    best = min(best, rec_compute_cost(tree_node.leftChild, contour2, s, i)
      + rec_compute_cost(tree_node.rightChild, contour2, i+1, e) +
      scalar * x)
  root_node.cost_table[s][e] = best
  #print s,e,root_node.points,best
  #raw_input()
  return best


def get_length(shape_tree):
  """Computes the length of a shape tree"""
  if hasattr(shape_tree, 'length'):
    return shape_tree.length

  if shape_tree.key.midpoint == None:
    x1,y1 = shape_tree.key.leftEndpoint
    x2,y2 = shape_tree.key.rightEndpoint
    shape_tree.length = np.sqrt((x1 - x2)*(x1-x2) + (y1-y2)*(y1-y2))
    return shape_tree.length

  shape_tree.length = get_length(shape_tree.leftChild) + get_length(shape_tree.rightChild)
  return shape_tree.length

def compute_cost(contour1, contour2):
  """Wrapper for recursive dynamic program"""
  # Use dp!!!!
  # Each node v in contour1 shape tree should have a table dp[i][j] that holds the
  # cost of matching v to contour2[i:j]
  shape_tree1 = contour_to_shape_tree(contour1)
  get_length(shape_tree1)

  m = len(contour2)
  return rec_compute_cost(shape_tree1, contour2, 0, m-1)

  # Recursion:
  # dp[A,B] = min 
