for N in $(seq ${1} ${2})
do
cp ~/Dropbox/Dissertation/Data/FullEvolutions/rpi_${N}.evo detected_evolutions.dat
cd ../../Networks/EvoTest/
tar -xzvf syn_shrt_${N}.tar.gz
cd -
./val_evo > ~/Dropbox/Dissertation/Data/FullEvolutions/rpi_${N}.results
done
