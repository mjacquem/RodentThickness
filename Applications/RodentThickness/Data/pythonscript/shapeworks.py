import sys
import os
from optparse import OptionParser
from vtk import vtkPolyDataReader, vtkPolyDataWriter, vtkThinPlateSplineTransform, vtkGeneralTransform, vtkTransformPolyDataFilter

imageMathPath = " "
shapeWorksRun = " "

def warp(srcLandmark,dstLandmark,subj):
  tps = vtkThinPlateSplineTransform()
  tps.SetSourceLandmarks(srcLandmark.GetPoints())
  tps.SetTargetLandmarks(dstLandmark.GetPoints())
  tps.SetBasisToR()

  t1 = vtkGeneralTransform()
  t1.SetInput(tps)

  tf = vtkTransformPolyDataFilter()
  tf.SetInput(subj)
  tf.SetTransform(t1)
  tf.Update()

  warped = tf.GetOutput()
  return warped

def readvtk(fin):
  r = vtkPolyDataReader()
  r.SetFileName(fin)
  r.Update()
  mesh = r.GetOutput()
  return mesh

def writevtk(fin, mesh):
  r = vtkPolyDataWriter()
  r.SetFileName(fin)
  r.SetInput(mesh)
  r.Write()


def vtk2lpts(inputImages,inputSurfaces, outputPoints):
  for (fim,fin, fout) in zip(inputImages, inputSurfaces, outputPoints):
    mesh = readvtk(fin)
    points = mesh.GetPoints()
    fo = open(fout, "w")
    print "Converting %s into %s [%d points] ..." % (fin, fout, points.GetNumberOfPoints())
    for i in range(0, points.GetNumberOfPoints()):
      p = points.GetPoint(i)
      pixsize=voxelSize(fim)
      print pixsize
      fo.write("%f %f %f\n" % (-1.25/pixsize[0]*p[0],-1.25/pixsize[1]*p[1],1.25/pixsize[2]*p[2]))
    fo.close()

def file2string(fn):
  f = open(fn, "r")
  lines = f.readlines()
  return [ x.strip() for x in lines ]

def string2file(fn,str):
  f = open(fn, "w")
  f.write(str)
  f.close()

def execute(cmd):
  print "Executing %s ..." % (cmd)
  os.system(cmd)

def xml2dom(xml):
  return xml

def lpts2vtk(inputImages,lptsIn, vtkTmpl, vtkOut):
  mesh = readvtk(vtkTmpl)
  for (fim,fin, fout) in zip(inputImages,lptsIn,vtkOut):
    print "Processing", fin, fout
    lines = file2string(fin) 
    pixsize=voxelSize(fim)
    points = [ [ float(f)*pixsize[0]/1.25 for f in l.split(" ") ] for l in lines ]
    meshPoints = mesh.GetPoints()
    if (meshPoints.GetNumberOfPoints() != len(points)):
      print "Mismatch between the template surface model and the corresponding points (%s) [%d:%d]" % (fin, meshPoints.GetNumberOfPoints(), len(points))
      continue
    for i in range(0, len(points)):
      meshPoints.SetPoint(i, points[i])
    mesh.SetPoints(meshPoints)
    writevtk(fout, mesh)
  return

def tpsWarp(sourceLandmark,targetLandmarks,deformationInput,finalSurfaces):
  src = readvtk(sourceLandmark)
  deforming = readvtk(deformationInput)
  for (fin,fout) in zip(targetLandmarks,finalSurfaces):
    print "warping [%s => %s] : [%s => %s] ..." % (sourceLandmark, fin, deformationInput, fout)
    dst = readvtk(fin)
    warped = warp(src, dst, deforming)
    print "done!!"
    writevtk(fout, warped)
  return

def groom(imagesIn,imagesOut):
  tmpl = """<?xml version="1.0" ?>
<background>0.0</background>
<foreground>1.0</foreground>
<pad>0</pad>
<antialias_iterations>20</antialias_iterations>
<blur_sigma>0.0625</blur_sigma>
<fastmarching_isovalue>0.0</fastmarching_isovalue>
<verbose>0</verbose>
<inputs>
##INPUTS##
</inputs>
<outputs>
##OUTPUTS##
</outputs>
"""
  tmpl = tmpl.replace("##INPUTS##", "\n".join(imagesIn))
  tmpl = tmpl.replace("##OUTPUTS##", "\n".join(imagesOut))
  string2file("groom.xml", tmpl)
  execute("%s %s isolate hole_fill antialias fastmarching blur" % (shapeWorksGroom, "groom.xml"))

def voxelSize(inputImage):
  pstat = os.popen("%s %s -info" % ( "ImageStat", inputImage))
  lines = pstat.read()
  pstat.close()
  pixdims = [ i for i in lines.split("\n") if i.startswith("Pixdims") ]
  spacing = [ float(i) for i in pixdims[0].split(" ")[1:] ]
  return spacing


def run(shapeWorksCmd,inputImages,inputPoints):
  tmpl = """<?xml version="1.0" ?>
<inputs>
###INPUTS###
</inputs>
<point_files>
###POINT_FILES###
</point_files>
<number_of_particles> 1002 </number_of_particles>
<starting_regularization> 10.0 </starting_regularization>
<ending_regularization> 0.1 </ending_regularization>
<optimization_iterations> 300 </optimization_iterations>
<checkpointing_interval> 20 </checkpointing_interval>
<output_points_prefix> output </output_points_prefix>
"""
  tmpl = tmpl.replace("###INPUTS###", "\n".join(inputImages)).replace("###POINT_FILES###", "\n".join(inputPoints))
  string2file("run.xml", tmpl)
  execute("%s %s" % (shapeWorksCmd, "run.xml"))


def executePreProcessing(opts, args, inputImages, preprocessedImages):
  for (imgIn, imgOut) in zip(inputImages, preprocessedImages):
    execute("%s %s -type short -changeSp 1.25,1.25,1.25 -outfile %s" % (opts.pathImageMath, imgIn, imgOut))
    execute("%s --binaryInput %s --smoothing 1.25 --output %s" % (opts.pathBinaryToDistanceMap, imgOut, imgOut))

def executePostProcessing(opts, args, inputSurfaces,inputImages):
  if (opts.correspondingOutputList == ""):
    outputSurfaces = [ f.replace(".vtk", ".out.vtk") for (i,f) in enumerate(inputSurfaces) ]
  else:
    print "Using correspondence output to be", opts.correspondingOutputList
    outputSurfaces = file2string(opts.correspondingOutputList)
  if (opts.warpedOutputList == ""):
    warpedSurfaces = [ f.replace(".vtk", ".warped.vtk") for (i,f) in enumerate(inputSurfaces) ]
  else:
    warpedSurfaces = file2string(opts.warpedOutputList)
  if (len(inputSurfaces) < 10):
    correspondingPoints = [ "output.%01d.lpts" % (i) for (i,f) in enumerate(inputSurfaces) ]
  elif (len(inputSurfaces) < 100):
    correspondingPoints = [ "output.%02d.lpts" % (i) for (i,f) in enumerate(inputSurfaces) ]
  else:
    correspondingPoints = [ "output.%03d.lpts" % (i) for (i,f) in enumerate(inputSurfaces) ]
  lpts2vtk(inputImages,correspondingPoints, inputSurfaces[0], outputSurfaces)
  if (not opts.isNoTPS):
    tpsWarp(inputSurfaces[0],outputSurfaces,inputSurfaces[0],warpedSurfaces)

def main(opts, args):
  # toolPath = opts.pathShapeWorks
  #shapeWorksRun = "%s/ShapeWorksRun" % (toolPath)
  #shapeWorksGroom = "%s/ShapeWorksGroom" % (toolPath)
  shapeWorksRun = opts.pathShapeWorksRun
  shapeWorksGroom =opts.pathShapeWorksGroom
  imageMathPath = opts.pathImageMath
  inputImages = file2string(args[0])
  inputSurfaces = file2string(args[1])
  if (len(inputImages) != len(inputSurfaces)):
    print "Number of subjects doesn't match [%d:%d]" % (len(inputImages), len(inputSurfaces))
    return
  preprocessedImages = [ "%03d.DT.nrrd" % (i) for (i,f) in enumerate(inputImages) ]
  inputPoints = [ "%03d.initial.lpts" % (i) for (i,f) in enumerate(inputSurfaces) ]
  if (not opts.isNoPreProcessing):
    executePreProcessing(opts, args, inputImages, preprocessedImages)
  if (not opts.isNoShapeWorks):
    vtk2lpts(inputImages,inputSurfaces, inputPoints)
    print "shapeWorksRun = [", shapeWorksRun, "]"
    run(shapeWorksRun,preprocessedImages, inputPoints)
  executePostProcessing(opts, args, inputSurfaces,inputImages)
  return


# add minus to x and y coordinate of all points
def correctPoints(opts, args):
  f = open(args[0], "r")
  lins = f.readlines()
  lins = [ "%f %f %f" % ( -float(y[0]), -float(y[1]), float(y[2]) ) for y in (x.split() for x in lins) ]
  lins = "\n".join(lins)
  f.close()
  # write processed points
  f = open(args[1], "w")
  f.write(lins)
  f.close()


if (__name__ =="__main__"):
  parser = OptionParser(usage="%prog [options] images_input surfaces_input")
  parser.add_option("--workingDir", dest="workingDirectory", help="change working directory where intermediate files such as parameters.xml and output.*.lpts are created.", default="")
  parser.add_option("-w", "--warpedOutputList", help="text file containing the list of warped output", dest="warpedOutputList", default="")
  parser.add_option("-c", "--correspondingOutputList", help="text file containing the list of corresponding surfaces", dest="correspondingOutputList", default="")
  parser.add_option("--pathShapeWorksRun", help="tool path for shape Works",dest="pathShapeWorksRun", default="/NIRAL/tools/bin_linux64/ShapeWorksRun")
  parser.add_option("--pathShapeWorksGroom", help="tool path for shape Works",dest="pathShapeWorksGroom", default="/NIRAL/tools/bin_linux64/ShapeWorksGroom")
  parser.add_option("--pathImageMath", help="path for ImageMath", dest="pathImageMath", default="/NIRAL/tools/bin_linux64/")
  parser.add_option("--pathBinaryToDistanceMap", help="path for BinaryToDistanceMap", dest="pathBinaryToDistanceMap", default="/tools/bin_linux64/BinaryToDistanceMap")
  parser.add_option("--fixpoints", help="fix points by flipping x,y against the origin (adding minus", action="store_true", dest="isFixingPoint")
  parser.add_option("--nopreprocessing", action="store_true", help="do not create distance map", dest="isNoPreProcessing")
  parser.add_option("--noshapeworks", action="store_true", help="do not run ShapeWorks", dest="isNoShapeWorks")
  parser.add_option("--notps", action="store_true", help="do not run TPS warping", dest="isNoTPS")

  (opts, args) = parser.parse_args()

  if (opts.workingDirectory != ""):
    os.chdir(opts.workingDirectory)

  if (opts.isFixingPoint):
    correctPoints(opts, args)
  elif (len(args) == 2):
    main(opts, args)
  else:
    parser.print_help()
