
import sys

# path="/tools/Python/Python-2.7.3/lib/python2.7/site-packages/setuptools-0.6c11-py2.7.egg:/tools/Python/Python-2.7.3/lib/python2.7/site-packages/pip-1.0-py2.7.egg:/tools/Python/Python-2.7.3/lib/python2.7/site-packages/VTK-6.0-py2.7.egg:/tools/Python/Python-2.7.3/lib/python2.7/site-packages/distribute-0.6.28-py2.7.egg:/tools/Python/Python-2.7.3/lib/python2.7/site-packages/SimpleITK-0.6.0.dev208-py2.7.egg:/tools/Python/Python-2.7.3/lib/python27.zip:/tools/Python/Python-2.7.3/lib/python2.7:/tools/Python/Python-2.7.3/lib/python2.7/plat-linux2:/tools/Python/Python-2.7.3/lib/python2.7/lib-tk:/tools/Python/Python-2.7.3/lib/python2.7/lib-old:/tools/Python/Python-2.7.3/lib/python2.7/lib-dynload:/tools/Python/Python-2.7.3/lib/python2.7/site-packages".split(":")

# for p in path:
#     sys.path.insert(0, p)



from vtk import *
from optparse import OptionParser
import csv,sys
import niralvtk as nv
import numpy as np

def removeAttributes(opts, args):
  print args
  inputvtk = args[0]
  outputvtk = args[1]

  inVTK = nv.readVTK(inputvtk);
  nArrays = inVTK.GetPointData().GetNumberOfArrays()
  arrayNames = []
  for k in range(0,nArrays):
    arrayNames.append(inVTK.GetPointData().GetArrayName(k))

  print arrayNames
  for name in arrayNames:
    inVTK.GetPointData().RemoveArray(name)

  nv.writeVTK(outputvtk, inVTK)

def main(opts, argv):
  inputvtk = argv[0]
  outputvtk = argv[1]

  inVTK = nv.readVTK(inputvtk);
  if (opts.sep == "0"):
    csvreader = csv.reader(open(opts.csvfile, "r"))
  elif (opts.sep == "1"):
    csvreader = csv.reader(open(opts.csvfile, "r"), delimiter=',')
  elif (opts.sep == "2"):
    csvreader = csv.reader(open(opts.csvfile, "r"), delimiter='\t')
  


  first = csvreader.next()
  if (opts.header):
    header = first
    first = csvreader.next()

  if (opts.names != ""):
    header = opts.names.split(",")

  print header
  
  nCols = len(first)
  nPoints = inVTK.GetNumberOfPoints() 

  data = np.zeros([nPoints,nCols])
  for k in range(0,nCols):
    data[0,k] = float(first[k])

  print "# points:", nPoints
  for j in range(1,nPoints):
    print j
    first = csvreader.next()
    for k in range(0,nCols):
      data[j,k] = float(first[k])

  for k in range(0,nCols):
    arr = vtkDoubleArray()
    if (len(header) > 0):
      arr.SetName(header[k])
    arr.SetNumberOfTuples(nPoints)
    for j in range(0,nPoints):
      arr.SetValue(j,data[j,k])
    inVTK.GetPointData().AddArray(arr)
    
  nv.writeVTK(outputvtk, inVTK)

if (__name__ == "__main__"):
  parser = OptionParser(usage="usage: %prog [options] input-vtk output-vtk")
  parser.add_option("-i", "--input", dest="csvfile", help="input attribute csv file", metavar="CSVFILE")
  parser.add_option("-t", "--title", dest="header", help="use first line as header", action="store_true", default=False);
  parser.add_option("-n", "--columnNames", dest="names", help="use this as column names", metavar="NAME1,NAME2,...", default="");
  parser.add_option("-r", "--removeAttributes", dest="removeAttrs", help="remove all attributes", action="store_true", default=False);
  parser.add_option("-s", "--separator", dest="sep", help="separator (0=space, 1=comma, 2=tab)", default="0")
  (opts, args) = parser.parse_args()

  if (len(args) < 2):
    parser.print_help()
  else:
    if (opts.removeAttrs):
      removeAttributes(opts, args)
    else:
      main(opts, args)
