import sys
import re

try:
	f = open(sys.argv[1])
except Exception:
	print("Cannot open file or empty file name")
	exit()

svg = f.read()

def parseSVG(s):
	mm = re.findall(r"<path.*?\sd\=\"([^\"]+)",s)
	print(mm)
	for m in mm:
		parsePath(m)

def parsePath(s):
	mm = re.findall(r"([a-zA-Z])")

parseSVG(svg)