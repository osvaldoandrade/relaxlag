import os

path = '../../../tp1-instancias/'
for dirname, dirnames, filenames in os.walk(path):
    for filename in filenames:
        fname = os.path.join(dirname, filename)
        print filename
        for i in range(1,4):
            cmd = "./relaxlag " + str(i) + " " + fname + "  " + "./result/" + filename+"_"+str(i)+".out 1"
            print "Results for: " + filename+"_"+str(i)+".out"            
            os.system(cmd);       
            print "\n"     
            #proc = subprocess.Popen(cmd.split(), stdout=subprocess.PIPE)
            #output = proc.communicate()
            
