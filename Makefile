CXX := g++
CXXFLAGS := -Wall

.PHONY: clean

bc_miner: Block.o miner_source.o
	$(CXX) $(CXXFLAGS) -o $@ $^ -pthread

Block.o: Block.cpp Block.hpp
	$(CXX) $(CXXFLAGS) -c $<

miner_source.o: miner_source.cpp ConsoleStall.h timer.hpp Array.hpp Block.hpp
	$(CXX) $(CXXFLAGS) -c $<

clean:
	$(RM) *.o


