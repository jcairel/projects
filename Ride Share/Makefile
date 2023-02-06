CXX=g++
CXXFLAGS=-std=c++11 -g -pthread -lrt

rideshare : main.o producer.o consumer.o io.o
	$(CXX) $(CXXFLAGS) -o rideshare $^

main.o : main.cpp main.h 

producer.o : producer.cpp producer.h

consumer.o : consumer.cpp consumer.h

io.o : io.cpp io.h ridesharing.h

clean :
	rm *.o
