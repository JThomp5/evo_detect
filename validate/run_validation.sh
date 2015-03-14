for N in $(seq 1 1)
do
    for T in $(seq 0.1 0.1 0.1)
    do
	D=../../Networks/FullEvoTest/
	for C in $(seq 0 14)
	do
	    echo ${D}/${N}/${T}/Communities${C}.rem
	done &> ground_truth_communities.dat

	for C in $(seq 1 14)
	do
	    echo ${D}/${N}/Transition${C}.dat
	done &> ground_truth_evolutions.dat

	./val_evo -devo ${D}/${N}/${T}/rpi_single.dat &> ${D}/${N}/${T}/rpi_evo.res
	#./val_evo -devo ${D}/${N}/${T}/rpi_evo.dat &> ${D}/${N}/${T}/rpi_evo.res
	#./val_evo -devo ${D}/${N}/${T}/rpi_evo_pr.dat &> ${D}/${N}/${T}/rpi_evo_pr.res
    done
done
