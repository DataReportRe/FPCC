import sys
import os
idx = [0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 21, 22, 23, 24, 25, 32, 40, 54, 55, 56, 57, 58, 59, 61, 64, 73, 77, 78, 80, 81, 83, 88, 90, 99, 100, 101, 102, 104, 105, 106]

os.system("rm errorTrace.out")
for i in idx:
    os.system(" bin/gslSolver.out gsl " + str(i) + " 1.0")
os.system("cp errorTrace.out result_analysis/")

