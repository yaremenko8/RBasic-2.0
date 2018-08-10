from sys import stdin

f = open("lt.txt", "r")
a = f.read().replace("\n", "").replace(" ", "").split(',')
f.close()

f0 = lambda x: [a[i] for i in list(map(int, x.split()))]

while(1):
	temp = stdin.readline()
	if(temp != "" and temp != "\n"):
		print(" ".join(map(str, f0(temp))))

