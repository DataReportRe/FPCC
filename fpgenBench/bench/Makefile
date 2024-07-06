#CFLAGS=-O0 -fno-slp-vectorize -std=c11  -L$(FPROOT)/obj -lfpsanitizer -lmpfr -lm -pthread -ltbb -lstdc++
CFLAGS=-O0  -fno-unroll-loops -fno-vectorize -fno-slp-vectorize -mno-x87 -g -std=c11 -I/usr/local/include -I$(EFT_HOME)/runtime -I$(EFT_HOME)/bench/meschach  -L$(MPFR_HOME)/mpfr-4.1.0/src/.libs -L/opt/DiRect/lib -L. -ldirect_opt -lmpfr -lm  -lpthread 
#CFLAGS=-O0 -fno-vectorize -fno-slp-vectorize -g -std=c11 -L/root/gslbuild/lib -L$(EFT_HOME)/runtime/obj -leftsanitizer -I$(EFT_HOME)/runtime  -L$(MPFR_HOME)/mpfr-4.1.0/src/.libs  -lmpfr -lm  -lpthread -lgsl -lgslcblas 
OPT = opt -load $(EFT_HOME)/llvm_pass/build/EFTSan/libEFTSanitizer.so -eftsan 
#OPT = opt -load $(EFT_HOME)/llvm_pass/build/EFTSan/libEFTSanitizer.so -eftsan -eftsan-detect-all-rounding-errors -eftsan-detect-all-exceptions -eftsan-enable-debugging
CSRC = $(wildcard *.c)
COUT = $(patsubst %.c,%.c.bc,$(CSRC))
OCOUT = $(patsubst %.c.bc,%.opt.bc,$(COUT))
OUT = $(patsubst %.opt.bc,%.c.o,$(OCOUT))
FOUT = $(patsubst %.c.o,%.fp.o,$(OUT))
OOUT = $(patsubst %.c,%.orig.o,$(CSRC))
OOUT_OP = $(patsubst %.c,%.orig.op.o,$(CSRC))
CC_NO_OP = clang -O0 -g -lm -fno-unroll-loops -L$(EFT_HOME)/runtime/obj -mlong-double-128 -mno-x87 -leftsanitizer -I /root/gslbuild/include -I$(EFT_HOME)/bench/meschach -I$(EFT_HOME)/runtime  -I/usr/local/include -L/opt/DiRect/lib -L. -ldirect_opt  -L$(MPFR_HOME)/mpfr-4.1.0/src/.libs -lpthread -lmpfr -lgmp 
CC = clang -O0 -g -lm  -fno-unroll-loops -L$(EFT_HOME)/runtime/obj -mlong-double-128 -mno-x87 -I$(EFT_HOME)/runtime  -leftsanitizer  -L$(MPFR_HOME)/mpfr-4.1.0/src/.libs -lmpfr -I$(EFT_HOME)/bench/meschach -I/usr/local/include -L/opt/DiRect/lib -L. -ldirect_opt -lmpfr -lgmp -lm -lpthread
CCE = clang -O0 -g  -fno-unroll-loops -fno-inline -fno-vectorize -mlong-double-128 -mno-x87 -fno-slp-vectorize -emit-llvm -I /root/gslbuild/include -I$(EFT_HOME)/runtime -I$(EFT_HOME)/bench/meschach -I/usr/local/include -L/opt/DiRect/lib -L. -ldirect_opt -lmpfr -lm -lgmp -lpthread 

#all: $(OOUT) 
all: $(FOUT) $(OOUT)  

.PRECIOUS: %.opt.bc %.c.bc

%.orig.o: %.c
	$(CC_NO_OP) $^ -o $@  $(CFLAGS) 
	chmod u+x $@

%.orig.op.o: %.c
	$(CC) $^ -o $@ $(CFLAGS) 
	chmod u+x $@

#%.fp.o: %.c.o
%.fp.o: %.c.o
	$(CC) $^ -o $@  $(CFLAGS) 
	chmod u+x $@

%.c.o: %.opt.bc
	# llvm-dis *.opt.bc
	llc $< -filetype=obj -o $@  

%.opt.bc: %.c.bc
	$(OPT) < $< > $@

%.c.bc: %.c
	$(CCE) -mlong-double-128 -c -o $@ $< 

# llvm-link *.bc -o $@

clean:
	rm -rf *.ll *.c.bc *.opt.bc *.o *.sout *.dSYM *~ *.log functions.txt  *.out
