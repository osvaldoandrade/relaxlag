import os
import subprocess

path = './'
for dirname, dirnames, filenames in os.walk(path):
    for filename in filenames:
        fname = os.path.join(dirname, filename)
        for i in range(1,4):
            cmd = "./relaxlag " + str(i) + " " + fname + "  " + fname+"_"+str(i)+".out"
            print  cmd
            proc = subprocess.Popen(cmd.split(), stdout=subprocess.PIPE)
            output = proc.communicate()
            
