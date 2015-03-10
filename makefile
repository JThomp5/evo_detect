all: evo_det evo_det2

evo_det: main.cc
	g++ main.cc -L../Libraries/Files -lfiles -L../Libraries/Params -lParams -o evo_det -g -std=c++11

ext: extend_evolutions.cc
	g++ extend_evolutions.cc -L../Libraries/Files -lfiles -L../Libraries/Params -lParams -o ext -g -std=c++

#evo_det2: mult_main.cc
#	g++ mult_main.cc -L../MyLibraries/Files -lfiles -L../MyLibraries/Params -lParams -o evo_det2 -g -std=c++11
