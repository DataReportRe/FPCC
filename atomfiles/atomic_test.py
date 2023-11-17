import sys
import os



def run2copy(i):
    os.system(" bin/gslSolver.out gsl all 1.0")
    os.system(" python3 script/oracleMpmath.py")
    os.system(" cp Output.out tempOuts/Output"+str(i)+".out")
    os.system(" cp tempOutput.out tempOuts/tempOutput"+str(i)+".out")
    os.system(" cp ATOM.pkl outs_atom/ATOM" + str(i)+".pkl")

import time
st = time.time()
os.system("mkdir outs_atom")
os.system("mkdir tempOuts")
for i in range(0,100):
    run2copy(i)
print(time.time()-st)

