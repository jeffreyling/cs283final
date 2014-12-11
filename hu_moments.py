import cv2
import numpy as np

img = cv2.imread('images/frog-6.jpeg')
img = cv2.cvtColor(img, cv2.COLOR_BGR2GRAY)
print cv2.HuMoments(cv2.moments(img)).flatten()