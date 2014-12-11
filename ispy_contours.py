import numpy as np
import cv2
import argparse
#from scipy import ndimage
#import matplotlib.pyplot as plt

kernel = cv2.getStructuringElement(cv2.MORPH_ELLIPSE, (3,3))

ap = argparse.ArgumentParser()
ap.add_argument('-m','--model',required=True,
  help="Path to model image")
ap.add_argument('-n','--num_matches',type=int,required=False, default=10)
ap.add_argument('--debug', dest='debug', action='store_true')
ap.add_argument('--print', dest='print_contours', action='store_true')
ap.set_defaults(debug=False, print_contours=False)
args = vars(ap.parse_args())

def get_contours(im, method=cv2.RETR_TREE):
  assert(im != None)
  if len(im.shape) == 3:
    imgray = cv2.cvtColor(im, cv2.COLOR_BGR2GRAY)
  else:
    imgray = im
  ret,thresh = cv2.threshold(imgray,127,255,0)
  contours, hierarchy = cv2.findContours(thresh,method,cv2.CHAIN_APPROX_SIMPLE)
  return contours


model_im = cv2.imread(args['model'])
filename = args['model'].split('/')[-1]
assert(model_im != None)
dilation = cv2.dilate(model_im, kernel, iterations=1)
# The model contour we work with.
model_c = get_contours(dilation, method=cv2.RETR_EXTERNAL)[0]


# Replace with Matlab? get edges
im = cv2.imread('images/ispy_edges.png')
im_orig = cv2.imread('images/ISPY_silhouettes_spread.jpg')
assert(im != None)
assert(im_orig != None)

# Multiple things you can do:
im_closure = cv2.morphologyEx(im, cv2.MORPH_CLOSE, kernel)
im_dilate = cv2.dilate(im, kernel, iterations=1)

# Fill contours?
# fig, ax = plt.subplots(figsize=(1,3))
# fill_plain = ndimage.binary_fill_holes(im)
# fill_closure = ndimage.binary_fill_holes(im_closure)
# fill_dilate = ndimage.binary_fill_holes(im_dilate)
# ax.imshow(fill_plain)
# ax.imshow(fill_closure)
# ax.imshow(fill_dilate)
# plt.show()

# Two versions of threshold
imgray = cv2.cvtColor(im_orig, cv2.COLOR_BGR2GRAY)
thresh = cv2.adaptiveThreshold(imgray,255,cv2.ADAPTIVE_THRESH_GAUSSIAN_C,
  cv2.THRESH_BINARY,11,7)
thresh_closure = cv2.morphologyEx(thresh, cv2.MORPH_CLOSE, kernel)

cv2.imwrite('ISpy_thresh.jpg', thresh)

# cv2.imshow('closure',im_closure)
# cv2.imshow('dilate',im_dilate)
# #cv2.imshow('thresh,Gaussian,edges',gauss_thresh_edges)
# #cv2.imshow('thresh,Gaussian',gauss_thresh)

# #gthresh_closure_edges = cv2.morphologyEx(gauss_thresh_edges, cv2.MORPH_CLOSE, kernel)
# #gthresh_closure = cv2.morphologyEx(gauss_thresh, cv2.MORPH_CLOSE, kernel)
# cv2.imshow('thresh',thresh)
# cv2.imshow('thresh,closure',thresh_closure)
# cv2.waitKey(0)
# cv2.destroyAllWindows()

contours_closure = get_contours(im_closure)
contours_dilate = get_contours(im_dilate)
contours_thresh = get_contours(thresh)

if args['print_contours']:
  for contours in [contours_closure, contours_dilate, contours_thresh]:
    print 'num',len(contours)
    print 'Global image:'
    img = np.zeros(im_orig.shape,np.uint8)
    colors = [(0,255,0), (255,0,0), (0,0,255), (255,255,0), (0,255,255), (255,0,255)]
    for i,c in enumerate(contours):
      if cv2.contourArea(c) < 30: continue
      cv2.drawContours(img, [c], -1, colors[i%6], 1)
    cv2.imshow('asdf',img)
  cv2.imwrite('images/ISpy_contours.jpg',img)
  cv2.waitKey(0)
  cv2.destroyAllWindows()

costs = []
for cnt in contours_thresh:
  if cv2.contourArea(cnt) < 30: continue
  costs.append((cv2.matchShapes(cnt, model_c, 2,0), cnt))

costs.sort(key=lambda x: x[0])
if args['debug']:
  for i in xrange(len(costs)):
    img = im_orig.copy()
    x,y,w,h = cv2.boundingRect(costs[i][1])
    cv2.rectangle(img, (x,y), (x+w,y+h),255,2)
    cv2.imshow('Debug',img)
    cv2.waitKey(0)
    cv2.destroyAllWindows()
else:
  img = im_orig.copy()
  for i in xrange(args['num_matches']):
    x,y,w,h = cv2.boundingRect(costs[i][1])
    cv2.rectangle(img, (x,y), (x+w,y+h),255,2)
  cv2.imwrite('images/best_matches_%s' % filename, img)
  cv2.imshow('Best matches',img)
  #cv2.waitKey(0)
  #cv2.destroyAllWindows()


img_model = np.zeros(model_im.shape,np.uint8)
cv2.drawContours(img_model, [model_c], -1, 255, 3)
cv2.imshow('model',img_model)
cv2.waitKey(0)
cv2.destroyAllWindows()
