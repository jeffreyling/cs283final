import subprocess
import os
import sys

MAXN = 20
if len(sys.argv) > 1:
  MAXN = int(sys.argv[1])

file1 = 'testc1.txt'
file2 = 'testc2.txt'
file3 = 'testc3.txt'

f1 = open(file1, 'w')
f2 = open(file2, 'w')
f3 = open(file3, 'w')
for i in range(1,MAXN):
  f1.write('%d %d\n' % (i,i))

for i in range(2*MAXN, MAXN+1, -1):
  f2.write('%d %d\n' % (i,i))

for i in range(1,MAXN):
  f3.write('%d %d\n' % (i,i*i))

closed_c1 = zip([1]*(MAXN-1), range(1,MAXN)) + zip(range(1,MAXN), [MAXN]*(MAXN-1)) + \
  zip([MAXN]*(MAXN-1), range(MAXN-1, 1,-1)) + zip(range(MAXN-1,1,-1), [1]*(MAXN-1))

file_closed = 'closed_testc1.txt'
f_closed1 = open(file_closed, 'w')
for x,y in closed_c1:
  f_closed1.write('%d %d\n' % (x,y))
# p = subprocess.Popen(['./shape_tree', file1, file2],
#   stdout=subprocess.PIPE)
# (output, err) = p.communicate()
# print output

# p_status = p.wait()