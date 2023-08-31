#INC_boost=/cvmfs/sphenix.sdcc.bnl.gov/gcc-12.1.0/release/release_new/new.7/include/boost
#INC_python=/cvmfs/sphenix.sdcc.bnl.gov/gcc-12.1.0/opt/sphenix/core/stow/Python-3.10.8/include/python3.10
#INC_PYTHON=$(eval $(shell python -c "from sysconfig import get_paths as gp; print(gp()['include'])" ))

#INC_python=-I/cvmfs/sphenix.sdcc.bnl.gov/gcc-12.1.0/opt/sphenix/core/stow/Python-3.10.8/include/python3.10 -I/cvmfs/sphenix.sdcc.bnl.gov/gcc-12.1.0/opt/sphenix/core/stow/Python-3.10.8/include/python3.10

#LD_FLAGS=-L/cvmfs/sphenix.sdcc.bnl.gov/gcc-12.1.0/opt/sphenix/core/boost/lib \
#	-lboost_system -lboost_program_options -lboost_filesystem -lboost_date_time \
#	-lboost_context -lboost_coroutine -lboost_chrono -lboost_log -lboost_thread \
#	-lboost_log_setup -lboost_regex -lboost_atomic \
#	-lpthread -lcrypt -lpthread -ldl  -lutil -lm -lm


#LD_FLAGS=-L/cvmfs/sphenix.sdcc.bnl.gov/gcc-12.1.0/opt/sphenix/core/stow/Python-3.10.8/lib -lpython3.10 -lcrypt -lpthread -ldl  -lutil -lm -lm
#                                                                                         -lpython3.10 -lcrypt -lpthread -ldl  -lutil -lm -lm

#CFLAGS=-I/cvmfs/sphenix.sdcc.bnl.gov/gcc-12.1.0/opt/sphenix/core/stow/Python-3.10.8/include/python3.10 -I/cvmfs/sphenix.sdcc.bnl.gov/gcc-12.1.0/opt/sphenix/core/stow/Python-3.10.8/include/python3.10  -Wno-unused-result -Wsign-compare  -DNDEBUG -g -fwrapv -O3 -Wall



CXXFLAGS=-std=c++17

CXX_INCLUDES=-I/cvmfs/sphenix.sdcc.bnl.gov/gcc-12.1.0/release/release_new/new.3/include \
	-I. -I./include 

LD_FLAGS=-L/cvmfs/sphenix.sdcc.bnl.gov/gcc-12.1.0/opt/sphenix/core/boost/lib \
	-lz -lssl -lcrypto -lpthread \
	-lboost_system -lboost_program_options \
	-lboost_filesystem -lboost_date_time \
	-lboost_context -lboost_coroutine \
	-lboost_chrono -lboost_log -lboost_thread \
	-lboost_log_setup -lboost_regex \
	-lboost_atomic -lpthread

CXX=g++
#CXX=clang -ferror-limit=1

src/%.o: src/%.cpp include/%.h
	$(CXX) $(CXXFLAGS) -c -o $@ $< \
	$(CXX_INCLUDES)

src/%.o: src/%.cpp 
	$(CXX) $(CXXFLAGS) -c -o $@ $< \
	$(CXX_INCLUDES)

sancho: src/Sancho.o src/sancho.o 
	$(CXX) -o $@ src/Sancho.o src/sancho.o $(LD_FLAGS)


