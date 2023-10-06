a.out: database_final.o
	g++ database_final.o -o a.out

database_final.o: database_final.cpp
	g++ -c database_final.cpp

target: dependancies
	action

clean: 
	rm *.o a.out