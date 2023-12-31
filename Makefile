
fpArithmetic.out: MyFloat.o main.o
	g++ -g -Wall -std=c++11 -m32 -o fpArithmetic.out MyFloat.o main.o

main.o: main.cpp MyFloat.h
	g++ -g -Wall -std=c++11 -c -m32 -o main.o main.cpp
	
MyFloat.o: MyFloat.h MyFloat.cpp
	g++ -g -Wall -std=c++11 -c -m32 -o MyFloat.o MyFloat.cpp
	
clean:
	rm main.o MyFloat.o fpArithmetic.out
