import shape_tree
import numpy
import pickle

# Dictionary of (key,value) as (filename, contour)
contours_file = open('mpeg7contours.pkl','rb')
all_contours = pickle.load(contours_file)
contours_file.close()

num_contours = len(all_contours)
print num_contours

# Let's test the shape tree implementation
contour1 = all_contours.values()[0][0] # 'first' contour of contour list
contour2 = all_contours.values()[1][0]
#st = shape_tree.contour_to_shape_tree(contour)

# Should get a way to merge all the contours!


# # I want to take the "main" contour of each file, and compute the pairwise cost of all files.
# contour_match_cost = [[0 for i in xrange(num_contours)] for j in xrange(num_contours)]

# # Make shape trees
# for filename1 in all_contours:
#   for filename2 in all_contours:
#     # Get main contour somehow
#     c1 = all_contours[filename1][0]
#     c2 = all_contours[filename2][0]

