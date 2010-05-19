CXX = g++
CXXFLAGS=-DNDEBUG -O2 -Wall

LIBS=-lcxcore \
     -lcv \
     -lhighgui \
     -lcvaux \
     -lhdf5_hl \
     -lhdf5 \
     -lpthread \
     -lboost_filesystem-mt

SRCDIR=src

.PHONY: all
all: demo

demo: demo.o hdf5opencv.o
	$(CXX) $^ $(CXXFLAGS) $(LIBS) -o $@

demo.o: $(SRCDIR)/demo.cc
	$(CXX) $(CXXFLAGS) -c $< 

hdf5opencv.o: $(SRCDIR)/hdf5opencv.cc
	$(CXX) $(CXXFLAGS) -c $< 

clean:
	rm -f *.o demo
