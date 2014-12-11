""" Run shape_tree C++ binary which takes in two files and compute the contour
matching cost between them """

import subprocess
import os

cur_path = os.getcwd()
path = cur_path + '/data'

all_data = os.listdir(path)
for i in xrange(len(all_data)):
  match_file = all_data[i]
  im_name1,file_type = match_file.split('.')
  if im_name1 == 'apple-10_contour': continue
  if file_type != 'txt':
    continue

  match_cost = {}
  for j in xrange(i,len(all_data)):
    other_file = all_data[j]
    if other_file == match_file:
      continue
    im_name2,file_type = other_file.split('.')
    if file_type != 'txt':
      continue

    p = subprocess.Popen("./shape_tree data/%s data/%s" % (match_file, other_file), stdout=subprocess.PIPE, shell=True)
    (output, err) = p.communicate('')
    print match_file, other_file, output
    match_cost[other_file] = float(output)

    p_status = p.wait()

  print 'results for %s' % match_file
  for k,v in match_cost.items():
    print k,v

  raw_input('press enter')