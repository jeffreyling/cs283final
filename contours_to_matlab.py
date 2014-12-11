import numpy as np, scipy.io
import cv2

from PIL import Image
import os
import pickle

DRAW_CONTOURS = False

path = '/Users/dna/Dropbox/waverley/Jeffrey.Ling/Work Documents/Harvard/Fall 2014/CS 283/Final project/MPEG7dataset'
os.chdir(path)

work_path = '/Users/dna/Dropbox/waverley/Jeffrey.Ling/Work Documents/Harvard/Fall 2014/CS 283/Final project/src/data/'

nfiles = len(os.listdir(path))

# Dictionary of filename to contour matrix, C by n by 2 array, where C is number of contours and n is length of each contour
all_contours = {}

# Dictionary to single longest contour to represent image
all_single_contours = {}

for filename in os.listdir(path):
  im_name,im_type = filename.split('.')
  if im_type != 'jpeg':
    continue

  im = cv2.imread(filename)
  assert(im != None)
  imgray = cv2.cvtColor(im, cv2.COLOR_BGR2GRAY)
  ret,thresh = cv2.threshold(imgray,127,255,0)
  contours, hierarchy = cv2.findContours(thresh,cv2.RETR_TREE,cv2.CHAIN_APPROX_NONE)

  # Make each contour an n by 2 array
  contours = [c.reshape((len(c),2)) for c in contours]
  all_contours[filename] = contours

  # Find longest contour
  bestlen = 0
  besti = -1
  for i in xrange(len(contours)):
    bestlen = max(len(contours[i]), bestlen)
    besti = i

  all_single_contours[filename] = contours[besti]

  # Write to C++ readable text format:
  f = open(work_path + im_name + '_contour.txt', 'w')
  for x,y in contours[besti]:
    f.write('%d %d\n' % (x,y))
  f.close()

  # Draw contours
  if DRAW_CONTOURS:
    img = np.zeros((512, 512, 3), np.uint8)
    print '%s has %d contours' % (filename,len(contours))
    if len(contours) > 1:
      print contours

    colors = [(0,255,0), (255,0,0), (0,0,255), (255,255,0), (0,255,255), (255,0,255)]
    for i,c in enumerate(contours):
      cv2.drawContours(img, [c], -1, colors[i%6], 1)
    cv2.imshow(filename,img)
    cv2.waitKey(0)
    cv2.destroyAllWindows()

  # Print to matlab format
  #var_name = im_name + '-contours'
  #var_name = var_name.replace('-', '_')
  #scipy.io.savemat(work_path + im_name + '.mat', mdict={var_name: contours})

# Print to python data format with pickle
write_file = open(work_path + 'mpeg7contours_single.pkl','wb')
pickle.dump(all_single_contours, write_file)
write_file.close()