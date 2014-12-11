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
im = cv2.imread('images/ispy_edges.png')
assert(im != None)
kernel = cv2.getStructuringElement(cv2.MORPH_ELLIPSE, (3,3))
im_closure = cv2.morphologyEx(im, cv2.MORPH_CLOSE, kernel)
im_dilate = cv2.dilate(im, kernel, iterations=1)
imgray = cv2.cvtColor(im, cv2.COLOR_BGR2GRAY)
blur = cv2.GaussianBlur(imgray, (5,5), 0)
ret,gauss_thresh = cv2.threshold(blur,0,255,cv2.THRESH_BINARY+cv2.THRESH_OTSU)

cv2.imshow('closure',im_closure)
cv2.imshow('dilate',im_dilate)
cv2.imshow('thresh,Gaussian',gauss_thresh)

gthresh_closure = cv2.morphologyEx(im, cv2.MORPH_CLOSE, kernel)
cv2.imshow('thresh,closure',gthresh_closure)
cv2.waitKey(0)
cv2.destroyAllWindows()

contours_closure = get_contours(im_closure)
contours_dilate = get_contours(im_dilate)
contours_gthresh = get_contours(gthresh_closure)
#contours = [c for c in contours if len(c) > CONTOUR_THRESH]

# Compute each contour cost_table.



  # Run binary
  # p = subprocess.Popen(['./shape_tree', cur_contour_file, model_file],
  #   stdout=subprocess.PIPE)
  # for line in p.stdout:
  #   args = line.split(' ')
  #   if args[0] == 'best':
  #     s,e,num = args[1:]
  #     cost_tables.append((s,e,float(num)))
  #   elif args[0] == 'overall':
  #     num_overall = args[1:]
  # p_status = p.wait()

  # print num, 'overall', num_overall
  # print 'cv2',cv2.matchShapes(c, model_c, 1, 0.0)

  # img = np.zeros(im.shape, np.uint8)
  # cv2.drawContours(img, c, -1, 255, 1)
  # cv2.drawContours(img, model_c, -1, (0,255,0), 1)
  # cv2.imshow('contour', img)
  # cv2.waitKey(0)
  # cv2.destroyAllWindows()
for contours in [contours_closure, contours_dilate, contours_gthresh]:
  print 'num',len(contours)
  print 'Global image:'
  img = np.zeros(im.shape,np.uint8)
  colors = [(0,255,0), (255,0,0), (0,0,255), (255,255,0), (0,255,255), (255,0,255)]
  for i,c in enumerate(contours):
  #  if len(c) < 10: continue
    cv2.drawContours(img, [c], -1, colors[i%6], 1)
  cv2.imshow('asdf',img)
  cv2.waitKey(0)
  cv2.destroyAllWindows()

# print 'Each contour closure:'
# for i,c in enumerate(contours_closure):
#   img = np.zeros(im.shape,np.uint8)
# #  if len(c) < 10: continue
#   cv2.drawContours(img, [c], -1, colors[i%6], 1)
#   cv2.imshow('img',img)
#   cv2.waitKey(0)
#   cv2.destroyAllWindows()
