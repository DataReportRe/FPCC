#CFLAGS=-O0 -fno-slp-vectorize -std=c11  -L$(FPROOT)/obj -lfpsanitizer -lmpfr -lm -pthread -ltbb -lstdc++
CFLAGS=-O0 -fno-vectorize -fno-slp-vectorize -g -std=c11 -L/fpcc/gslbuild/lib -I$(EFT_HOME)/runtime  -L$(MPFR_HOME)/mpfr-4.1.0/src/.libs  -lmpfr -lm  -lpthread -lgsl -lgslcblas 
#CFLAGS=-O0 -fno-vectorize -fno-slp-vectorize -g -std=c11 -L/root/gslbuild/lib -L$(EFT_HOME)/runtime/obj -leftsanitizer -I$(EFT_HOME)/runtime  -L$(MPFR_HOME)/mpfr-4.1.0/src/.libs  -lmpfr -lm  -lpthread -lgsl -lgslcblas 
#OPT = opt -load $(EFT_HOME)/llvm_pass/build/EFTSan/libEFTSanitizer.so -eftsan 
OPT = opt -load $(EFT_HOME)/llvm_pass/build/EFTSan/libEFTSanitizer.so -eftsan -eftsan-detect-all-rounding-errors -eftsan-detect-all-exceptions -eftsan-enable-debugging
CSRC = $(wildcard *.c)
COUT = $(patsubst %.c,%.c.bc,$(CSRC))
OCOUT = $(patsubst %.c.bc,%.opt.bc,$(COUT))
OUT = $(patsubst %.opt.bc,%.c.o,$(OCOUT))
FOUT = $(patsubst %.c.o,%.fp.o,$(OUT))
OOUT = $(patsubst %.c,%.orig.o,$(CSRC))
OOUT_OP = $(patsubst %.c,%.orig.op.o,$(CSRC))
CC_NO_OP = clang -O0 -g -lm -L$(EFT_HOME)/runtime/obj -leftsanitizer -L/fpcc/gslbuild/lib -I /fpcc/gslbuild/include -I$(EFT_HOME)/runtime   -L$(MPFR_HOME)/mpfr-4.1.0/src/.libs -lmpfr -lgsl -lgslcblas 
CC = clang -O0 -g -lm -L$(EFT_HOME)/runtime/obj -I$(EFT_HOME)/runtime  -leftsanitizer  -L$(MPFR_HOME)/mpfr-4.1.0/src/.libs -lmpfr 
CCE = clang -O0 -g -fno-inline -fno-vectorize -fno-slp-vectorize -emit-llvm -I /fpcc/gslbuild/include -I$(EFT_HOME)/runtime

#all: $(OOUT) 
all: $(FOUT) $(OOUT)  

.PRECIOUS: %.opt.bc %.c.bc

%.orig.o: %.c
	$(CC_NO_OP) $^ -o $@  
	chmod u+x $@

%.orig.op.o: %.c
	$(CC) $^ -o $@  
	chmod u+x $@

%.fp.o: %.c.o
	$(CC) $^ -o $@  $(CFLAGS) 
	chmod u+x $@

%.c.o: %.opt.bc
	llc $< -filetype=obj -o $@  
	llvm-dis *.opt.bc

%.opt.bc: %.c.bc
	$(OPT) < $< > $@

%.c.bc: %.c
	$(CCE) -c -o $@ $< 
	llvm-link-10 *.bc -o $@

clean:
	rm -rf *.ll *.c.bc *.opt.bc *.o *.sout *.dSYM *~ *.log functions.txt  *.out
