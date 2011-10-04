#!/usr/bin/env python3
import sys

def decimalToBinary(dec):
	print("Converting", dec, "to binary")

	# Special case(s)
	if dec == "0":
		signB = "0"
		fracPartBin = "0"
		expBin = "0"
	elif dec == "NaN":
		signB = "0"
		fracPartBin = "0"
		expBin = "11111111"
	else:
		# Normal conversion
		dec = float(dec)

		# Sign?
		if dec < 0:
			signB = "1"
		else:
			signB = "0"
	
		# Convert dec to basic binary
		# Do in two steps, whole part first
		wholePartBin = bin(int(abs(dec)))[2:]

		# Then fraction, adding up to our intended fraction
		fracPartBin = ""
		fracGoal = dec - int(dec)
		fracTot = 0
		i = -1
		while fracTot < fracGoal:
			if pow(2, i) + fracTot <= fracGoal:
				fracTot += pow(2, i)
				fracPartBin += "1"
			else:
				fracPartBin += "0"
			i -= 1

		print("Decimal in binary = " + wholePartBin + "." + fracPartBin)

		# Normalize the result
		exp = len(wholePartBin) - 1
		fracPartBin = wholePartBin[1:] + fracPartBin
		fracPartBin = fracPartBin[:23]

		print("Normalized = 1." + fracPartBin + " * 2^" + str(exp))

		expWithBias = exp + 127
		expBin = bin(expWithBias)[2:]

	# Tell the user what we're working with
	print("Sign bit =", signB)
	print("Exponent =", expBin)
	print("Stored fraction =", fracPartBin)

	# Pad fraction and exponent
	fracPartBin = fracPartBin + ("0" * (23 - len(fracPartBin)))
	expBin = ("0" * (8 - len(expBin))) + expBin

	# Sanity check
	result = signB + expBin + fracPartBin
	if len(result) != 32:
		print("########### wrong length!! ###########")

	return result

def binaryToDecimal(bin):
	print("Converting", bin, "(" + hex(int(bin, 2)) + ") to base 10 decimal")

	# Incoming sanity check
	if len(bin) != 32:
		print("Binary string not long enough (only", len(bin), "bits)")
		exit()

	# Sign?
	signB = bin[0]
	if signB == "1":
		signD = -1
	else:
		signD = 1

	# Pull out exponent and convert to decimal
	expB = bin[1:9]
	expD = int(expB, 2)

	# Pull out fractional part and convert to fractional decimal
	fracB = bin[9:32]
	fracD = 0
	for i in range(0, len(fracB) - 1):
		if fracB[i] == "1":
			fracD += pow(2, (-1 * (i + 1)))

	# Info for user
	print("Sign bit =", signB)
	print("Exponent =", expB, "=", expD)
	print("Fraction =", fracB, "=", fracD)
	print("-1^" + str(signB) + " * (1 + " + str(fracD) + ") * 2^(" + str(expD) + " - 127)")

	return signD * (1 + fracD) * pow(2, expD - 127)

def main(argv = None):
	if argv is None or len(argv) != 3:
		print("Usage:", argv[0], "<b|h|d> <number>")
		exit()
	
	if argv[1] == "d":
		result = decimalToBinary(argv[2])
	elif argv[1] == "h":
		hexBin = bin(int(argv[2], 16))[2:]
		result = binaryToDecimal("0" * (32 - len(hexBin)) + hexBin)
	elif argv[1] == "b":
		result = binaryToDecimal(argv[2])
	else:
		print("Number type must be one of (b)inary, (h)ex, or (d)ecimal")
		exit()

	print("Final result:", result)

if __name__ == '__main__':
	main(sys.argv)

