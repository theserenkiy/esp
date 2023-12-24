import os
import re

fp = open('list.txt', 'r', encoding="utf-8")

lines = fp.read().split('\n')

data = []
for line in lines:
	if line[0]=='-': break
	m = re.match(r'\s+(\d+)\s+(\d+)\s+([\d\,]+)',line)
	if not m: continue
	data.append([int(m[1]), int(m[2]), [int(v) for v in m[3].split(',')]])

#print(data)

lists = [[[],[]],[[],[]],[[],[]]]
times = []
for rec in data:
	#print(rec[0]-1, rec[1])
	times.append([rec[0],rec[1]])
	lvls = rec[2]
	
	for lvl in lvls:
		if lvl > 3: continue
		lists[lvl-1][0 if len(lvls) == 1 else 1].append(rec[0])
	
#print(lists)
#print(times)

#check times
for n,t in enumerate(times):
	if t[0] != n+1:
		print('ERROR: not in order at '+(n+1))
		break

c = ''
varnames = []

for lvln, lvl in enumerate(lists):
	lvarnames = []
	for ordn, order in enumerate(lvl):
		varname = 'lvl_'+str(lvln+1)+'_'+str(ordn)
		c += 'static int '+varname+'[] = {'+(','.join([str(x) for x in order]))+'};\n'
		lvarnames.append(varname)
	varnames.append(lvarnames)

c += 'int *lvlmsgs[3][2] = {'+(','.join(['{'+(','.join(vv))+'}' for vv in varnames]))+'};\n'
c += 'int lvlsizes[3][2] = {'+(','.join(['{'+(','.join([str(len(v)) for v in vv]))+'}' for vv in lists]))+'};\n'
c += 'int msg_durations[] = {'+(','.join([str(x[1]) for x in times]))+'};\n'
print(c)
	
#print(varnames)