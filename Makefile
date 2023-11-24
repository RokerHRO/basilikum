.PHONY: all clean

CC=gcc -Wall -g -O0
CXX=g++ -Wall -std=c++14 -g

all: test_basilikum basilikum

test_basilikum : test_basilikum.o basilikum.o
	${CXX} -o $@ $^ -lgtest -lpthread

basilikum : main.o basilikum.o
	${CXX} -o $@ $^ -lgtest -lpthread

basilikum.o : basilikum.c basilikum.h
	${CC} -c -o $@ $<

test_basilikum.o : test_basilikum.cc
	${CXX} -c $^

clean:
	rm -vf *.o test_basilikum basilikum
