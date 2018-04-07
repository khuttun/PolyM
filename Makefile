
optimize_debug_flags=-O3 #-g
comp_flags=-std=c++11
shared_lib_flags=-fPIC -shared -DIS_LIB

default: bin

bin_objs:
	g++ $(comp_flags) -c *.cpp


lib_objs:
	g++ $(optimize_debug_flags) $(comp_flags) $(shared_lib_flags) -c *.cpp



bin: bin_objs
	g++ -o test.exe *.o -lpthread



lib: lib_objs
	ar rvs polym.a *.o

