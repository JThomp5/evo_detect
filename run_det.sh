for N in $(seq 1 10)
do 
    for T in 0.6 0.7 0.8 0.9 #0.1 0.2 0.3 0.4 0.5
    do 
	echo ${N} ${T}
	for C in $(seq 0 14)
	do 
	    echo ../Networks/FullEvoTest/${N}/${T}/Communities${C}.tr.rem
	done &> communities.dat
	./evo_det &> ../Networks/FullEvoTest/${N}/${T}/rpi_evo.dat
    done
done
