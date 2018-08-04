testall: testsc

testsc: sc
	g++ ../../test/testSc.cpp \
		-std=c++11 -L. -lsc -lpthread -ggdb -ldl -o testsc
        
sc:
	g++ ../../src/sc.cpp -c -std=c++11 -o sc.o
	ar crv libsc.a sc.o

clean:
	rm -f *.a
	rm -f *.o
	rm -f *.so
	rm -f *.out
	rm -f *.db
	rm -f *.log
	rm -f *.db
	rm -rf *.dSYM
