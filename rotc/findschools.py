#!/usr/bin/env python
import sys
import getopt
import csv
import urllib.request

def createSchoolList(filename):
    school = csv.reader(open(filename, newline=''))

    students = []
    for line in school:
        if line[0] != '':
            students.append(line[0].strip())

        if len(line) > 5 and line[4].strip() != '':
            students.append(line[4].strip())
            
        if len(line) > 9 and line[8].strip() != '':
            students.append(line[8].strip())
            
        if len(line) > 13 and line[12].strip() != '':
            students.append(line[12].strip())

    students.sort()

    return students

def readInMissingBelts(filename):
    belts = csv.reader(open(filename, newline=''))

    missing = []
    for belt in belts:
        if len(belt) < 9 or belt[8] != 'x':
            # Found someone without a return
            # Do they have an assigned number?
            if belt[3].strip() == '':
                continue
            
            if len(belt) >= 5 and belt[4].strip() != '':
                missing.append((belt[3].strip(), belt[4].strip()))
            else:
                missing.append((belt[3].strip(), -1))

    return missing

def filterStudents(belts, students):
    missing = []
    for belt in belts:
        # Is this student in the list we're looking at?
        # Be somewhat forgiving in search, allow the end of string to differ
        for student in students:
            if (student.startswith(belt[0]) or belt[0].startswith(student)) and belt[0] != '' and student != '':
                missing.append(belt)

    return missing

def main(argv=None):
    if argv is None:
        argv = sys.argv

    # Create lists of cadets
    print("Reading in cadets...")
    cadets = createSchoolList(argv[2])
    #for i in cadets:
    #    print(i)
    print("Cadet count (estimate, likely too high):", len(cadets))

    # Read in the current unreturned belts
    print("Reading in missing belts...")
    missing = readInMissingBelts(argv[1])
    #for i in missing:
    #    print(i)
    print("Found", len(missing), "missing belts")

    # Create school-specific lists for missing belts
    print("Filtering list to school...")
    missing = filterStudents(missing, cadets)
    #for i in missing:
    #    print(i)
    print("Missing belt count for given studets:", len(missing))

    # Write out students
    print("Cadets with missing belt:")
    for student in missing:
        if student[1] == -1:
            print("\t\t", student[0])
        else:
            print("\t", student[1], "\t", student[0])
        pass

    return 0

if __name__ == "__main__":
    sys.exit(main(['find.py', 'belts.csv', 'curoster.csv']))
    
