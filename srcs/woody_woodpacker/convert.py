
print("\t\t", end="")
for i in range(0, 256):
	if (i + 1) % 12 != 0 and i != 255:
		print("0x{:02x}".format(i), end=", ")
	elif (i + 1) % 12 == 0 and i != 255:
		print("0x{:02x}".format(i), end=",\n\t\t")
	else:
		print("0x{:02x}".format(i))
