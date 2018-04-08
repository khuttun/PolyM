
optimize_debug_flags=-O3 #-g
comp_flags=-std=c++11 $(optimize_debug_flags)
comp_flags_lib=$(comp_flags) -fPIC -DIS_LIB

link_flags_shared_lib=-fPIC -shared

default: all

all: bin libpolym.a libpolym.so

install: libpolym.a
	sudo cp libpolym.a /usr/local/lib; sudo ldconfig

bin: bin_objs
	g++ -o test.exe *.o -lpthread


lib: polym.a

libpolym.a: lib_objs
	ar rvs libpolym.a *.o

libpolym.so: lib_objs
	g++ $(link_flags_shared_lib) -o libpolym.so *.o


bin_objs:
	g++ $(comp_flags) -c *.cpp

lib_objs:
	g++ $(comp_flags_lib) -c *.cpp

clean:
	rm -f *.o *.a *.so test.exe


