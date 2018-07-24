import sys

def ones(v):
	mask = 1
	cnt = 0
	while mask < 0x100:
		if v & mask <> 0:
			cnt = cnt + 1
		mask = mask << 1
	return cnt

for i in xrange(0, 16):
	for j in xrange(0, 16):
		sys.stdout.write("%d, " % ones((i << 4) | j))
	sys.stdout.write("\n")
