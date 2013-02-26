"NIRAL lab python utility package"

from vtk import *
import os, os.path 
import fnmatch 

def colorize(text, color_code): return color_code + text + "\033[0m" 
def red(t): return colorize(t, "\033[1m\033[31m")
def green(t): return colorize(t, "\033[1m\033[32m")
def darkgreen(t): return colorize(t, "\033[32m")
def yellow(t): return colorize(t, "\033[1m\033[33m")
def blue(t): return colorize(t, "\033[1m\033[34m")
def darkblue(t): return colorize(t, "\033[34m")
def pur(t): return colorize(t, "\033[1m\033[35m")

def findFilesByName(dir, fargs):
  "find all files under the dir with given name list"
  freturns = []
  for root, dir, files in os.walk(dir):
    for f in fargs:
      if f in files:
        freturns.append(os.path.join(root,f))

  freturns.sort()
  return freturns

def findFilesByPattern(dir, fpattern):
  "find all files with the pattern from dir"
  freturns = []
  for root, dir, files in os.walk(dir):
    freturns.extend(map(lambda x: os.path.join(root,x), fnmatch.filter(files, fpattern)))

  freturns.sort()
  return freturns

def readStructuredPoints(vtk):
  "read structured dataset and return structured data object"
  r = vtkStructuredPointsReader()
  r.SetFileName(vtk)
  r.Update()
  return r.GetOutput()

def readUnstructuredGrid(vtk):
  "read unstructured grid"
  r = vtkUnstructuredGridReader()
  r.SetFileName(vtk)
  r.Update()
  return r.GetOutput()

def readVTK(vtkfile):
  "read vtkfile file and return polydata object"

  print "Reading: [%s]" % (vtkfile)
  vtkfile = vtkfile.strip()
  if (vtkfile.endswith(".vtk")):
    r = vtkPolyDataReader()
  elif (vtkfile.endswith(".vtp")):
    r = vtkXMLPolyDataReader()
  elif (vtkfile.endswith(".vts")):
    r = vtkXMLStructuredGridReader()
  elif (vtkfile.endswith(".vtu")):
    r = vtkXMLUnstructuredGridReader()
  else:
    print "error reading: " + vtkfile
  r.SetFileName(vtkfile)
  r.Update()
  return r.GetOutput()

def readBYU(byu):
  "read byu file and return polydata object"
  r = vtkBYUReader()
  r.SetFileName(byu)
  r.Update()
  return r.GetOutput()


def writeVTK(name, vtkmesh):
  "write a given vtk mesh into file"
  name = name.strip()
  if (name.endswith(".vtk")):
    w = vtkPolyDataWriter()
    w.SetFileName(name)
    w.SetInput(vtkmesh)
    w.Write()
    return
  elif (name.endswith(".vtp")):
    w = vtkXMLPolyDataWriter()
  elif (name.endswith(".vts")):
    w = vtkXMLStructuredGridWriter()
  elif (name.endswith(".vtu")):
    w = vtkXMLUnstructuredGridWriter()
  w.SetFileName(name)
  w.SetInput(vtkmesh)
  w.SetDataModeToAppended()
  w.EncodeAppendedDataOff()
  w.Write()


def writeBYU(name, byu):
  "write a given mesh into byu format file"
  w = vtkBYUWriter()
  w.SetGeometryFileName(name)
  w.SetInput(byu)
  w.Write()

def readPoints(mesh):
  "return an array of points (which is also 3-value array) from given mesh"
  pointList = []
  points = mesh.GetPoints()
  for i in range(0, points.GetNumberOfPoints()):
    p = points.GetPoint(i)
    pointList.append(p)
  return pointList


def readLPTS(file):
  pointList = []
  f = open(file, "r")
  for l in f:
    l = l.strip()
    p = map(float, l.split(" "))
    pointList.append(p)
  return pointList

def readArray(file):
  values = []
  f = open(file, "r")
  for l in f:
    l = l.strip()
    values.append(l)
  return values


def convertLPTS2vtkPoints(lpts):
  points = vtkPoints()
  points.SetNumberOfPoints(len(lpts))
  for (idx, p) in enumerate(lpts):
    points.SetPoint(idx, p[0], p[1], p[2])
  return points

def readAsString(fname):
  f = open(fname, "r")
  lines = f.readlines()
  s = reduce(lambda x, y: x + y, lines, "")
  return s

def writeIntoFile(fname, contents):
  f = open(fname, "w")
  f.write(contents)
  f.close()
