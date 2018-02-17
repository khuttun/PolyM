

default: bin

objs:
	g++ -std=c++11 -c *.cpp

bin: objs
	g++ -o test.exe *.o -lpthread

	


