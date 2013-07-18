import os,sys
from optparse import OptionParser

parser = OptionParser(usage="")
(opts, args) = parser.parse_args()

inputtxt = args[0]




lines = []
fin = open(inputtxt)
lines.append(fin.readlines())
fin.close()
print lines
if ((lines) != [['2\n']]):
 print "no match"
 sys.exit(1)
else :
 print "match"
 sys.exit(0)
 

  


