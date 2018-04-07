
optimize_debug_flags=-O3 #-g
comp_flags=-std=c++11 $(optimize_debug_flags)
comp_flags_lib=$(comp_flags) -fPIC -DIS_LIB

link_flags_shared_lib=-fPIC -shared

default: bin

all: bin polym.a poly.so

bin: bin_objs
	g++ -o test.exe *.o -lpthread


lib: polym.a

polym.a: lib_objs
	ar rvs polym.a *.o

polym.so: lib_objs
	g++ $(link_flags_shared_lib) -o polym.so *.o


bin_objs:
	g++ $(comp_flags) -c *.cpp

lib_objs:
	g++ $(comp_flags_lib) -c *.cpp

clean:
	rm -f *.o *.a *.so test.exe


