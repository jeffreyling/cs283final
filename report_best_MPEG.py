import os
import pickle
import operator

f = open('using/mpeg7contours_costs_cv2.pkl','rb')
costs = pickle.load(f)
f.close()

# CV2 sorted
for k in costs.keys():
  ordered_costs = sorted(costs[k].items(), key=operator.itemgetter(1))
  if k[:6] == 'device': continue
  print 'Best for',k
  for i in xrange(15):
    if ordered_costs[i][1] == 0: continue
    else: print ordered_costs[i]
  print
  raw_input()

# Shape tree
# costs_st = costs
# for k in costs_st.keys():
#   costs_st[k] = sorted(costs_st[k].items(), key=operator.itemgetter(1)[1])