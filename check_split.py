import sys
import os
from subprocess import call

def Jaccard ( a, b ):
    total_members = 0.0
    matches = 0.0
    for m in a:
        total_members = total_members + 1
        if m in b:
            matches = matches + 1

    for m in b:
        if m not in a:
            total_members = total_members + 1.0

    #print sorted ( a )
    #print sorted ( b )
    #print float ( matches ) / float ( total_members )
    #print "\n"
            
    return float(matches) / float ( total_members )

#Global helpers
threshold = 0.85 #for matching communities
accurate_runs = 0.0 #counting successes
continuation = 0.0
total_runs = 0.0

def isFloat ( x ):
    try:
        float ( x )
    except ValueError:
        return False

    return True

def run_single ( communities1, communities2 ):
    global continuation
    global accurate_runs
    global total_runs
        
    netout = open ( "communities.dat", "w" )
    netout.write ( communities1 + "\n")
    netout.write ( communities2 + "\n")    
    netout.close()
    
    #Run CPM
    os.system ( "./evo_det2 > evolutions.dat" )
    
    #Read real and detected communities
    gr1 = []
    gr2 = []
    gr3 = []
    
    f = open ( communities1 )
    gr1 = [x.strip(" ") for x in f.readline().rstrip("\n").split(" ") if x.strip(" ")]
    #gr1[0] = (gr1[0].split(":"))[1]
    f.close()
    
    f = open ( communities2 )
    gr2 = [x.strip(" ") for x in f.readline().rstrip("\n").split(" ") if x.strip(" ")]
    gr3 = [x.strip(" ") for x in f.readline().rstrip("\n").split(" ") if x.strip(" ")]
    #gr2[0] = (gr2[0].split(":"))[1]
    f.close()
    
    #If they are close, increase accurate count
    f = open ( "evolutions.dat" )

    evo = []
    line = [x.strip(" ") for x in f.readline().rstrip("\n").split(" ") if x.strip(" ")]

    mrg1 = False
    mrg2 = False
    while (True):
        if not line: break
           
        if ( ( len(line) > 1 ) & ( isFloat(line[0]) ) ):
            evo.append ( line )

        #Check accuracy
        else:            
            if  ( len ( evo ) == 2 ):
                j1 = Jaccard ( evo[0], gr1 )
                j2 = Jaccard ( evo[1], gr2 )
                j3 = Jaccard ( evo[1], gr3 )
                f1 = ( j1 > threshold )
                f2 = ( j2 > threshold )
                f3 = ( j3 > threshold )
                if ( f1 & f2 ):
                    mrg1 = True
                if ( f1 & f3 ):
                    mrg2 = True
                    
            evo = []
        line = [x.strip(" ") for x in f.readline().rstrip("\n").split(" ") if x.strip(" ")]

    if ( mrg1 & mrg2 ):
        print communities1
        accurate_runs = accurate_runs + 1
    elif ( mrg1 | mrg2 ):
        continuation = continuation + 1
        
    total_runs = total_runs + 1
    f.close()

#Print out the network files to network.dat
for i in range (1, 31, 1):
        #for mp in range (0.5, 1, 0.1):
    dr = "../Networks/SplitTest/20_60_0.8_2.5_0_0_1_med/"+ str(i) + "/"
    communities1 = str(dr) + "Communities0.tr"
    communities2 = str(dr) + "Communities1.tr"
    run_single ( communities1, communities2 )        
        
print total_runs
print float ( accurate_runs ) / float ( total_runs )
print float ( continuation ) / float ( total_runs )
