import sys
import os



def run2copy(i):
    os.system(" bin/gslSolver.out gsl all 0")
    os.system(" python3 script/oracleMpmath.py")
    os.system(" cp Output.out tempOuts/Output"+str(i)+".out")
    os.system(" cp tempOutput.out tempOuts/tempOutput"+str(i)+".out")
    os.system(" cp FPCC.pkl outs_fpcc/FPCC" + str(i)+".pkl")

import time
st = time.time()
os.system("rm -r outs_fpcc")
os.system("rm -r tempOuts")
os.system("mkdir tempOuts")
os.system("mkdir outs_fpcc")
for i in range(0,100):
    run2copy(i)
os.system("rm -r result_analysis/outs_fpcc")
os.system("cp -r outs_fpcc result_analysis/")
print(time.time()-st)

