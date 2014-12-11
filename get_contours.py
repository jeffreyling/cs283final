import numpy as np
import cv2
import pprint
import subprocess

CONTOUR_THRESH = 10 # size of minimum allowed contour

def get_contours(im, method=cv2.RETR_TREE):
  assert(im != None)
  imgray = cv2.cvtColor(im, cv2.COLOR_BGR2GRAY)
  ret,thresh = cv2.threshold(imgray,127,255,0)
  contours, hierarchy = cv2.findContours(thresh,method,cv2.CHAIN_APPROX_SIMPLE)
  return contours


model_im = cv2.imread('images/swan.jpg')
# The model contour we work with.
model_c = get_contours(model_im, method=cv2.RETR_CCOMP)[0]
model_c_out = model_c.reshape((len(model_c),2))

model_file = 'cur_model_contour.txt'
f_model = open(model_file,'w')
for x,y in model_c_out:
  f_model.write('%d %d\n' % (x,y))
f_model.close()


# print model_contours, len(model_contours)
# print hierarchy

# img_model = np.zeros(model_im.shape,np.uint8)
# colors = [(0,255,0), (255,0,0), (0,0,255), (255,255,0), (0,255,255), (255,0,255)]
 #cv2.drawContours(img_model, model_c, -1, colors[i%6], 1)
#cv2.drawContours(img_model, [model_c], -1, (0,255,0), -1)
# cv2.imshow('model',img_model)
# cv2.waitKey(0)
# cv2.destroyAllWindows()

# Replace with Matlab? get edges
im = cv2.imread('images/black_edges.tif')
assert(im != None)
contours = get_contours(im)
contours = [c for c in contours if len(c) > CONTOUR_THRESH]

# Compute each contour cost_table.

cost_tables = []
cur_contour_file = 'cur_contour.txt'
for c in contours:
  f = open(cur_contour_file, 'w')
  for x,y in c.reshape(len(c), 2):
    f.write('%d %d\n' % (x,y))
  f.close()

  # Run binary
  p = subprocess.Popen(['./shape_tree', cur_contour_file, model_file],
    stdout=subprocess.PIPE)
  for line in p.stdout:
    args = line.split(' ')
    if args[0] == 'best':
      s,e,num = args[1:]
      cost_tables.append((s,e,float(num)))
    elif args[0] == 'overall':
      num_overall = args[1:]
  p_status = p.wait()

  print num, 'overall', num_overall
  print 'cv2',cv2.matchShapes(c, model_c, 1, 0.0)

  img = np.zeros(im.shape, np.uint8)
  cv2.drawContours(img, c, -1, 255, 1)
  cv2.drawContours(img, model_c, -1, (0,255,0), 1)
  cv2.imshow('contour', img)
  cv2.waitKey(0)
  cv2.destroyAllWindows()


# img = np.zeros(im.shape,np.uint8)
# colors = [(0,255,0), (255,0,0), (0,0,255), (255,255,0), (0,255,255), (255,0,255)]
# for i,c in enumerate(contours):
# #  if len(c) < 10: continue
#   cv2.drawContours(img, [c], -1, colors[i%6], -1)
# cv2.imshow('img',img)
# cv2.waitKey(0)
# cv2.destroyAllWindows()