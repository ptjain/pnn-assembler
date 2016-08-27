#! /usr/bin/python3.4

reg3 = 12
reg2 = 11
reg1 = 18
reg0 = 25
total_reg = 16
count3 = 0
count2 = 0
count1 = 0
count0 = 0

print("<General Purpose Reg>")
for i in range(total_reg):
	print("= " + "{0:04b}".format(i))
print("<3 reg Ins>")
for i in range(reg3):
	print("= " + "{0:04b}".format(count3))
	count3 += 1
#count3 = count3 + 1
print("<2 reg Ins>")
for i in range(reg2):
	if count2 >= 16:
		count2 = 0
		count3 = count3 + 1
	print("= " + "{0:04b}".format(count3) + " " + "{0:04b}".format(count2))
	count2 += 1
#count2 = count2 + 1
print("<1 reg Ins>")
for i in range(reg1):
	if count1 >= 16:
		count1 = 0
		count2 += 1
	if count2 >= 16:
		count2 = 0
		count3 += 1
	print("= " + "{0:04b}".format(count3) + " " + "{0:04b}".format(count2)
	+ " " + "{0:04b}".format(count1))
	count1 += 1
#count1 = count1 + 1
print("<0 reg Ins>")
for i in range(reg0):
	if count0 >= 16:
		count0 = 0
		count1 += 1
	if count1 >= 16:
		count1 = 0
		count2 += 1
	if count2 >= 16:
		count2 = 0
		count3 += 1
	print("= " + "{0:04b}".format(count3) + " " + "{0:04b}".format(count2)
	+ " " + "{0:04b}".format(count1) + " " + "{0:04b}".format(count0))
	count0 += 1
