CXX=clang++-3.9
LD=clang++-3.9
#CXXFLAGS=-std=c++11 -O2 -fPIC -fno-rtti
CXXFLAGS=-std=c++11 -O2 -fPIC

CLANG=clang-3.9
OPT=opt-3.9
CLANGFLAGS=-fPIC 

LLVMCFG=llvm-config-3.9
LLVMCXXFLAGS=$(shell $(LLVMCFG) --cxxflags)
LLVMCXXFLAGS+=-fPIC
LLVMLDFLAGS=$(shell $(LLVMCFG) --ldflags)

GSLCFG=/fpcc/gslbuild/bin/gsl-config
GSLCXXFLAGS=$(CXXFLAGS) $(shell $(GSLCFG) --cflags)
GSLLDFLAGS=-static $(shell $(GSLCFG) --libs)


OS_NAME=$(shell uname -s)
ifeq ($(OS_NAME), Darwin)
	# For MacOS: https://stackoverflow.com/questions/37210682/building-and-using-a-pass-for-llvm-3-8-on-osx
	LLVMLDFLAGS+=-Wl,-flat_namespace -Wl,-undefined -Wl,suppress
endif

DEPS = src/fpUtil.h src/communicator.h src/opcode.h

.PHONY: clean passModule handler util target solver

all: solver
prep: passModule handler util target

# the passModule
#passModule: libPassModule.o
#	$(CXX) -shared -fPIC `llvm-config-3.9 --ldflags` passModule.o -o lib/libPassModule.so
#	rm passModule.o
#libPassModule.o: src/passModule.cpp $(DEPS)
#	$(CXX) -c -fPIC -std=c++11 `llvm-config-3.9 --cxxflags` -I../llvm_env/llvm391/include src/passModule.cpp -o passModule.o

# The handler start
handler: build/handlers.o
build/handlers.o: src/handlers.cpp $(DEPS)
	@mkdir -p build
	$(CXX) -c $(CXXFLAGS) -o $@ $<
# The handler finish

util: build/fpUtil.o
build/fpUtil.o: src/fpUtil.cpp src/fpUtil.h
	@mkdir -p build
	$(CXX) $(CXXFLAGS) -c -o $@ $<

# Compile the target under analysis start.
targetObjs = build/targetExample.o
target: build/all_target.a
build/all_target.a: $(targetObjs)
	ar crv $@ $(targetObjs)
build/targetExample.o: src/targetExample.c lib/libPassModule.so
	@mkdir -p build
	$(CLANG) -S -emit-llvm -g -o build/targetExample.ll $<
	$(OPT) -S -load lib/libPassModule.so -funcpass -o build/targetExample_i.ll build/targetExample.ll
	$(CLANG) $(CLANGFLAGS) -g -c -o $@ build/targetExample_i.ll
# Compile the target under analysis finish.

# Generate GSL Solver
solver: bin/gslSolver.out
bin/gslSolver.out: build/gslSolver.o build/fpUtil.o build/handlers.o build/all_target.a 
	@mkdir -p bin
	$(LD) -o $@ $^ -I/usr/local/include -L/usr/local/lib -L/opt/DiRect/lib -L. -ldirect_opt $(GSLLDFLAGS) 
build/gslSolver.o: src/gslSolver.cpp $(DEPS)
	@mkdir -p build
	$(CXX) $(GSLCXXFLAGS) -c -I/usr/local/include -Isrc/ -o $@ $<
clean:
	rm -f *.o *.so *.out
	rm -f *.ll *.bc *.s
	rm -f *.dwo
	rm -rf *.dSYM
	rm -rf build/
	rm -rf bin/
	rm -rf data/
