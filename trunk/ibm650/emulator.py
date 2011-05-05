#! /usr/bin/env python3.0
import sys
from math import sqrt

class Emulator:
    """Implements a simple emulator similar to the IBM650"""
    # Actually combined in the real machine, but simpler broken
    # apart as we have here. Each will be 1000 words long
    data_memory = None
    prog_memory = None

    # Instruction pointer
    ip = None

    # Pre-given inputs
    inputs = None

    # Returns for tick
    SUCCESS, END_PROGRAM = range(2)

    def tick(self):
        """Executes the next instruction. Essentially a clock cycle.
        Returns:
            string - Anyout output from a 'write' instruction
            SUCCESS - Instruction run, nothing to report
            END_PROGRAM - The end of the program has been reached"""
        # Make sure the emulator has been initialize so we can actually
        # do some work
        if(self.ip == None or self.data_memory == None or self.prog_memory == None):
            raise EmulatorError("Emulator not initialized before use")

        # What's the next instruction?
        ins, op1, op2, dest = self.get_next_instruction()
        
        if(ins == '+0'): # move
            self.set_data(dest, self.get_data(op1))            
        elif(ins == '+1'): # +
            self.set_data(dest, self.get_data(op1) + self.get_data(op2))
        elif(ins == '+2'): # *
            self.set_data(dest, self.get_data(op1) * self.get_data(op2))
        elif(ins == '+3'): # square
            self.set_data(dest, self.get_data(op1) * self.get_data(op1))
        elif(ins == '+4'): # == 
            if(self.get_data(op1) == self.get_data(op2)):
                self.ip = dest
                return self.SUCCESS
        elif(ins == '+5'): # >=
            if(self.get_data(op1) >= self.get_data(op2)):
                self.ip = dest
                return self.SUCCESS
        elif(ins == '+6'): # x(y) -> z
            self.set_data(dest, self.get_data(op1 + self.get_data(op2)))
        elif(ins == '+7'): # increment and test
            self.set_data(op1, self.get_data(op1) + 1)
            if(self.get_data(op1) < self.get_data(op2)):
                self.ip = dest
                return self.SUCCESS
        elif(ins == '+8'): # read input into destination
            self.set_data(dest, self.get_next_input())
        elif(ins == '+9'): # stop - end program
            return self.END_PROGRAM
        elif(ins == '-0'): # negate and move
            self.set_data(dest, self.get_data(op1) * -1)
        elif(ins == '-1'): # -
            self.set_data(dest, self.get_data(op1) - self.get_data(op2))
        elif(ins == '-2'): # /
            try:
                self.set_data(dest, self.get_data(op1) / self.get_data(op2))
            except ZeroDivisionError:
                raise EmulatorError('Division by 0')
        elif(ins == '-3'): # square root
            self.set_data(dest, sqrt(self.get_data(op1)))
        elif(ins == '-4'): # !=
            if(self.get_data(op1) != self.get_data(op2)):
                self.ip = dest
                return self.SUCCESS
        elif(ins == '-5'): # <
            if(self.get_data(op1) < self.get_data(op2)):
                self.ip = dest
                return self.SUCCESS
        elif(ins == '-6'): # x -> y(z)
            self.set_data(op2 + self.get_data(dest), self.get_data(op1))
        elif(ins == '-7'): # unused
            raise EmulatorError(ins + ' is an unused instruction')
        elif(ins == '-8'): # print data
            # Note we use the mem directly because we want the string,
            # not the int that get_data() would return
            self.ip += 1
            return self.get_data_string(op1)
        elif(ins == '-9'): # unused
            raise EmulatorError(ins + ' is an unused instruction')
        else:
            raise EmulatorError(ins + ' is an unrecognized instruction')

        # If the instruction didn't do something different and return on its
        # own, then assume we go to next and return success
        self.ip += 1
        return self.SUCCESS

    def get_next_input(self):
        """Returns the next input or raises an exception if there is none."""
        if(self.inputs == None or len(self.inputs) == 0):
            raise EmulatorError('Not enough inputs given')
        return self.inputs.pop(0)

    def get_next_instruction(self, ip = None):
        """Returns the instruction as a tuple, broken into instruction
        and operands."""
        if(ip == None):
            ip = self.ip
        if(ip == None or self.prog_memory[ip] == None):
            raise EmulatorError('Instruction pointer is currently invalid.')
        x = self.prog_memory[ip]
        return (x[:2], int(x[2:5]), int(x[5:8]), int(x[8:11]))

    def get_data(self, addr):
        """Returns the data at the given address as an integer."""
        return int(self.get_data_string(addr))
        
    def get_data_string(self, addr):
        """Returns the data at the given address as an string. If
        the memory location hasn't been used before or initialize,
        it is zeroed before being returned. Helps with debugging/printing
        to not have them all set initially."""
        if(self.data_memory == None):
            raise EmulatorError('Data memory has not been initialized')
        
        if(self.data_memory[addr] == None):
            self.data_memory[addr] = '+0000000000'
        return self.data_memory[addr]

    def set_data(self, addr, data):
        """Sets the data at the given address to the data. If the data
        is an integer, it is first converted to the 11 character
        string."""
        if(self.data_memory == None):
            raise EmulatorError('Data memory has not been initialized')

        if(isinstance(data, str)):
            self.data_memory[addr] = data[:11]
        else:
            # Convert to int, then string. Extra int step so we
            # don't try to store floating points
            data = str(int(data))

            # Add + sign if needed
            if(data[0] != '-'):
                data = '+' + data

            # Add 0s to make it 11 characters long/truncate
            # if it is longer than 11
            data = (data[0] + '0' * (11 - len(data)) + data[1:])[:11]

            # Store
            self.data_memory[addr] = data

    def init_data(self, data = []):
        """Sets the start of the data memory to the list given in
        mem. Fills the remainder with 0. Restarts the emulator."""
        self.data_memory = [None for x in range(1000)]
        self.data_memory[:len(data)] = data
        self.restart()

    def init_prog(self, prog):
        """Sets the start of the data memory to the list given in
        mem. Fills the remainder with 0. Restarts the emulator."""
        self.prog_memory = [None for x in range(1000)]
        self.prog_memory[:len(prog)] = prog

        # Place an end program instruction after the rest of the
        # instructions if there isn't already one there and there's
        # room. Make it a little crazy so it's easy to tell it was added
        op, a, b, c = self.get_next_instruction(len(prog) - 1)
        if(op != '+9' and len(prog) != 1000):
            self.prog_memory[len(prog)] = '+9111111111'
        
        self.restart()

    def init_inputs(self, ins):
        """Sets/appends the given inputs to the current inputs. Does
        _not_ reset the emulator."""
        if(self.inputs == None):
            self.inputs = ins
        else:
            self.inputs.expand(ins)

    def add_input(self, ins):
        """Merely a different name for init_inputs. Makes it more
        obvious that you can just add a single input to the queue."""
        self.init_inputs(ins)
    
    def restart(self):
        """Resets the emulator so that the next tick will begin the
        currently loaded program again."""
        self.ip = 0

    def load(self, program_file):
        """Loads a program file, initializing the emulator"""
        # Read in file line by line. First section is initial data,
        # second section is the program, third is any inputs to be used
        # Note after each while we remove the +999999999 that got added and
        # clear line
        data = []
        prog = []
        inputs = []
        
        # Initial data
        line = None
        while(line != '+9999999999'):
            line = program_file.pop(0)[:11]
            data.append(line)
        data.pop()
        
        # Program
        line = None
        while(line != '+9999999999'):
            line = program_file.pop(0)[:11]
            prog.append(line)
        prog.pop()

        # Get inputs
        while(len(program_file) > 0):
            line = program_file.pop(0)[:11]
            inputs.append(line)

        # Save everything
        self.init_data(data)
        self.init_prog(prog)
        self.init_inputs(inputs)

        # Restart emulator
        self.restart()

    def __init__(self, prog = None):
        """Initialize the emulator"""
        if(prog != None):
            self.load(prog)
            self.restart()

    def __str__(self):
        """Returns a nice string showing the state of the emulator."""
        # If we run out of anything to display just stop
        displayed = True

        # Header
        out = '       Program:        Data:        Inputs:\n'

        curr = 0
        while(displayed == True):
            displayed = False

            out += '0' * (3 - len(str(curr))) + str(curr) + ':'
            
            # Pointer to current instruction
            if(self.ip == curr):
                out += ' > '
            else:
                out += '   '

            # Program memory
            if(curr < len(self.prog_memory) and self.prog_memory[curr] != None):
                # Would be nice to break this into 3 sections
                displayed = True
                x = self.prog_memory[curr]
                out += ' '.join((x[:2], x[2:5], x[5:8], x[8:11])) + '  '
            else:
                out += '                '

            # Data memory
            if(curr < len(self.data_memory) and self.data_memory[curr]):
                displayed = True
                out += self.data_memory[curr] + '  '
            else:
                out += '             '

            # Inputs
            if(curr < len(self.inputs)):
                displayed = True
                out += self.inputs[curr]

            # Finish line
            out += '\n'

            # Next instruction
            curr += 1

        # Chop off last line (it's blank except for the address)
        return out[:out.rfind('\n', 0, -1)] + '\n'

class EmulatorError(Exception):
    """Small class the emulator can raise for exceptions. Allows
    a string description to be included."""
    def __init__(self, desc):
        self.description = desc

    def __str__(self):
        return self.description

# Main
def main(in_name = '-', out_name = '-', debug = False):
    """Main"""
    # Is there an input file or are we reading from the console?
    if(in_name == '-'):
        in_file = sys.stdin    
    else:
        try:
            in_file = open(in_name, 'r')
        except IOError as e:
            print("Failed to open input '" + e.filename + "' for reading: " + e.strerror)
            sys.exit()
    contents = in_file.readlines()

    # Output either goes to a file or standard out
    if(out_name == '-'):
        out_file = sys.stdout
    else:
        try:
            out_file = open(out_name, 'w')
        except IOError as e:
            print("Failed to open output '" + e.filename + "' for writing: " + e.strerror)
            sys.exit()
        
    try:
        # Create and initialize emulator
        emu = Emulator(contents)
        
        while True:
            # Run an instruction
            if(debug):
                out_file.write('-' * 47 + '\n')
                out_file.write(str(emu))
            result = emu.tick()

            # Need to act on it in any way?
            if(result == emu.END_PROGRAM):
                if(debug):
                    out_file.write('\nNormal emulation end')
                break
            elif(isinstance(result, str)):
                if(debug):
                    out_file.write('\nOutput: ')
                out_file.write(result + '\n')
                
    except EmulatorError as e:
        out_file.write('Emulation ended with error: ' + str(e) + '\n')
        out_file.write('Internal state:\n' + str(emu))
        
    # I would close the files at this point just to be nice and
    # clean, but the program is ending and closing stdin/out
    # causes some Python implementations to crash, so I would have
    # to check what we have...

# Call main() after processing args really quickly
if __name__ == "__main__":
    in_arg = '-'
    out_arg = '-'
    debug = False
    if(len(sys.argv) >= 2):
        in_arg = sys.argv[1]
    if(len(sys.argv) >= 3):
        out_arg = sys.argv[2]
    if(len(sys.argv) == 4):
        debug = True
    
    # Does this poor sap need help figuring out how to call us?
    if(len(in_arg) > 1 and in_arg[0] == '-'):
        print("""Usage: emulator.py [input program file] [output file] [debug]
            Defaults to console input and output. Use a hyphen (-)
            in each place to those explicitly.
            
            Placing 'debug' as the third argument causes the emulator internals
            to be shown before each instruction is executed.""")
        sys.exit()

    # Call main    
    main(in_arg, out_arg, debug)
