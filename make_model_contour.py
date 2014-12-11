import numpy as np
import cv2
import argparse

kernel = cv2.getStructuringElement(cv2.MORPH_RECT, (3,3))

ap = argparse.ArgumentParser()
ap.add_argument('-m','--model',required=True,
  help="Path to model image")
args = vars(ap.parse_args())

# Model step
im_orig = cv2.imread(args['model'])
assert(im_orig != None)
imgray = cv2.cvtColor(im_orig, cv2.COLOR_BGR2GRAY)
#thresh = cv2.adaptiveThreshold(imgray,255,cv2.ADAPTIVE_THRESH_GAUSSIAN_C,
#  cv2.THRESH_BINARY,11,7)
ret,thresh = cv2.threshold(imgray,127,255,cv2.THRESH_BINARY)

im = cv2.morphologyEx(thresh, cv2.MORPH_CLOSE, kernel)
#im = cv2.erode(thresh, kernel, iterations=1)
#cv2.imshow('asdf',im)
#cv2.waitKey(0)
im = cv2.bitwise_not(thresh)
contours,hierarchy = cv2.findContours(im,cv2.RETR_CCOMP,cv2.CHAIN_APPROX_SIMPLE)

best = 0
cnt = contours[0]
for c in contours:
  if cv2.contourArea(c) > best and len(c) > 5:
    best = cv2.contourArea(c)
    cnt = c

mask = np.zeros(im.shape, np.uint8)
cv2.drawContours(mask,[cnt],0,255,-1)

cv2.imshow('asdf',mask)
cv2.waitKey(0)

name = args['model'].split('.')[-2].split('/')[-1]
cv2.imwrite('images/%s2.jpg' % name, mask)