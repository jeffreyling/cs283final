import numpy as np

import sys

MAXN = 20
if len(sys.argv) > 1:
  MAXN = int(sys.argv[1])

c1 = np.array(zip(range(1,MAXN), range(1,MAXN)))
c2 = np.array(zip(range(2*MAXN,MAXN+1,-1), range(2*MAXN,MAXN+1,-1)))
c3 = np.array(zip(range(1,MAXN), [x*x for x in range(1,MAXN)]))

closed_c1 = zip([1]*(MAXN-1), range(1,MAXN)) + zip(range(1,MAXN), [MAXN]*(MAXN-1)) + \
  zip([MAXN]*(MAXN-1), range(MAXN-1, 1,-1)) + zip(range(MAXN-1,1,-1), [1]*(MAXN-1))

import shape_tree

print shape_tree.compute_cost(c1, c2)
print c1,c2
print shape_tree.compute_cost(c3,c2)
print c3,c2