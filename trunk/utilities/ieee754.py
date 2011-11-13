#!/usr/bin/env python3
import sys
import argparse

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
		# Determine which way to shift
		if wholePartBin == '0':
			# Find the first one in the fraction part
			firstOne = fracPartBin.index('1')
			
			# And shift, taking only up to 23 bits of precision
			exp = -1 * (firstOne + 1)
			fracPartBin = fracPartBin[firstOne + 1:firstOne + 24]
			wholePartBin = '1'
		else:
			# Shift it down to have wholePart just be '1' every time
			exp = len(wholePartBin) - 1
			fracPartBin = wholePartBin[1:] + fracPartBin
			fracPartBin = fracPartBin[:23]
			wholePartBin = '1'
			
		if len(fracPartBin) == 0:
			fracPartBin = '0'

		print("Normalized = {}.{} * 2^{}".format(wholePartBin, fracPartBin, str(exp)))

		# Determine the exponent to store
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
		print("########### wrong length for final result!! ###########")

	return result

def binaryToDecimal(bin):
	print("Converting {} ({}) to base 10 decimal".format(bin, hex(int(bin, 2))))

	# Incoming sanity check
	if len(bin) != 32:
		print('Binary string must be 32 bits (' + len(bin) + ' receieved). Single precision floats only.')
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
	print("Sign bit = {}".format(signB))
	print("Exponent = {} = {}".format(expB, expD))
	print("Fraction = {} = {}".format(fracB, fracD))
	print('-1^{} * (1 + {}) * 2^({} - 127)'.format(str(signB), str(fracD), str(expD)))

	return signD * (1 + fracD) * pow(2, expD - 127)

def main(argv = None):
	if argv is None:
		argv = sys.argv
		
	# Parse arguments
	parser = argparse.ArgumentParser(description='Converts between decimal and IEEE754 binary format')
	parser.add_argument('num', metavar='number', help='Number to convert to IEEE754 format. If hex (prefix \'0x\')or binary (suffix \'b\'), assumed to be a IEEE754 number to convert to decimal')
	args = parser.parse_args()
	
	# Pass to appropriate parser
	if args.num[:2] == '0x':
		# Convert to appropriately-sized binary string
		hexBin = bin(int(args.num, 16))[2:]
		hexBin = "0" * (32 - len(hexBin)) + hexBin
		
		result = binaryToDecimal(hexBin)
		print("Final result:", result)
	
	elif args.num[-1].lower() == 'b':
		result = binaryToDecimal(args.num[:-1])
		print("Final result:", result)
	
	else:
		if len(args.num) == 32:
			print('WARNING: Treating as decimal. To use as binary, append \'b\' to the number')
			print()
			
		result = decimalToBinary(args.num)
		print("Final result: {} ({})".format(result, hex(int(result, 2))))

# Run main
if __name__ == '__main__':
	main(sys.argv)

