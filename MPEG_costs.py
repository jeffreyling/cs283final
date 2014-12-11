import subprocess
import os
import pickle
import cv2

f = open('using/mpeg7contours_actual.pkl','rb')
all_single_contours = pickle.load(f)
f.close()

# Key: filename, value: dict of filename, (cost_cv2, cost_shape_tree)
costs = {}
for filename, cnt in all_single_contours.items():
  for f2, c2 in all_single_contours.items():
    if f2 == filename: continue

    # CV2 cost
    cost_cv2 = cv2.matchShapes(cnt, c2, 1,0.0)
    if filename not in costs.keys():
      costs[filename] = {f2: cost_cv2}
    else:
      costs[filename][f2] = cost_cv2

    # Shape tree
    # c1_file = open('using/%s.txt' % filename, 'w')
    # c2_file = open('using/%s.txt' % f2, 'w')
    # for x,y in cnt.reshape(len(cnt),2):
    #   c1_file.write('%d %d\n' % (x,y))
    # c1_file.close()
    # for x,y in c2.reshape(len(c2),2):
    #   c2_file.write('%d %d\n' % (x,y))
    # c2_file.close()
    # p = subprocess.Popen(['./shape_tree', 'using/%s.txt' % filename, 'using/%s.txt' % f2],
    #   stdout = subprocess.PIPE)
    # for line in p.stdout:
    #   args = line.split(' ')
    #   if args[0] == 'overall':
    #     num = float(args[1])
    #     if filename not in costs.keys():
    #       costs[filename] = {f2: (cost_cv2, num)}
    #     else:
    #       costs[filename][f2] = (cost_cv2, num)

    # p_status = p.wait()
    print filename,f2,cost_cv2

f = open('using/mpeg7contours_costs_cv2.pkl','wb')
pickle.dump(costs, f)
f.close()