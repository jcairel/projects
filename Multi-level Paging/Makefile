CXX=g++
CXXFLAGS=-std=c++11 -g

pagingwithtlb : main.o pageTable.o tracereader.o output_mode_helpers.o tlb.o
	$(CXX) $(CXXFLAGS) -o pagingwithtlb $^

main.o : main.cpp main.h

pageTable.o : pageTable.cpp pageTable.h

tlb.o : tlb.cpp tlb.h

tracereader.o : tracereader.cpp tracereader.h

output_mode_helpers.o : output_mode_helpers.cpp output_mode_helpers.h

clean :
	rm *.o

