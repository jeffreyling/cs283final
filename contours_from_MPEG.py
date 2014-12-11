import cv2
import numpy as np
import os

all_single_contours = {}

im_path = os.getcwd() + '/using/'
for filename in os.listdir(im_path):
  im_name,im_type = filename.split('.')
  assert(im_type == 'jpeg')

  im = cv2.imread('using/' + filename)
  assert(im != None)
  imgray = cv2.cvtColor(im, cv2.COLOR_BGR2GRAY)
  ret,thresh = cv2.threshold(imgray,127,255,0)
  contours,hierarchy = cv2.findContours(thresh, cv2.RETR_EXTERNAL, cv2.CHAIN_APPROX_SIMPLE)

  cnt = contours[0]
  all_single_contours[im_name] = cnt

  # Test
  # img = np.zeros((512, 512, 3), np.uint8)
  # if len(contours) > 1:
  #   print contours

  # colors = [(0,255,0), (255,0,0), (0,0,255), (255,255,0), (0,255,255), (255,0,255)]
  # for i,c in enumerate(contours):
  #   cv2.drawContours(img, [c], -1, colors[i%6], 1)
  # cv2.imshow(filename, img)
  # cv2.waitKey(0)
  # cv2.destroyAllWindows()

import pickle
write_file = open('using/' + 'mpeg7contours_actual.pkl','wb')
pickle.dump(all_single_contours, write_file)
write_file.close()