#!/work/joohwi/Python/Python-build/bin/python

import os,sys
from optparse import OptionParser

def countLines(files):
  lines = []
  for file in files:
    fin = open(file)
    lines.append(len(fin.readlines()))
    fin.close()
  return lines

def readAllFiles(files):
  lines = []
  for file in files:
    fin = open(file)
    lines.append(fin.readlines())
    fin.close()
  return lines

def file2string(f):
  fin = open(f)
  lines = fin.readlines()
  lines = map(lambda x: x.strip(), lines)
  return lines
   

def main(opts, args):
  if (opts.file != ""):
    args = file2string(opts.file)
  lines = countLines(args)
  if (len(lines) == 0):
    print "Empty files"
    return
  if (min(lines) != max(lines)):
    print "Some of input files have different line numbers: ", lines
    return
  lineStrings = readAllFiles(args)
  if (opts.output != ""):
    fout = open(opts.output, "w")
  else:
    fout = sys.stdout
  for i in range(0,lines[0]):
    tempOutputLine = [ x[i].strip() for x in lineStrings]
    outputLineString = "\t".join(tempOutputLine)
    fout.write(outputLineString)
    fout.write("\n")
  fout.close()

if __name__ == "__main__":
  parser = OptionParser(usage="concatenate files into columns in output")
  parser.add_option("-o", "--output", help="output file name", dest="output", default="")
  parser.add_option("-f", "--file", help="a text file containing file list to concatenate", dest="file", default="")
  (opts, args) = parser.parse_args()
  main(opts,args)
