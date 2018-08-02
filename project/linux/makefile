testall: testslib

testslib: slib
	g++ ../../test/testSLibCore.cpp \
		-std=c++11 -L. -lslibcore -lpthread -ggdb -ldl -o testslib
        
slib:
	g++ ../../src/slibCore.cpp -c -std=c++11 -o slibcorecpp.o
	ar crv libslibcore.a slibcorecpp.o

clean:
	rm -f *.a
	rm -f *.o
	rm -f *.so
	rm -f *.out
	rm -f *.db
	rm -f *.log
	rm -f *.db
	rm -rf *.dSYM
