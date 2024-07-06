export PATH=/usr/bin:$PATH
export CXX=clang++
export CC=clang
export LD_LIBRARY_PATH=/usr/local/lib:$LD_LIBRARY_PATH
export LD_LIBRARY_PATH=/opt/DiRect/lib:$LD_LIBRARY_PATH
export FPCC_HOME=/fpcc/fpgenBench
export LD_LIBRARY_PATH=$FPCC_HOME/runtime/obj:$LD_LIBRARY_PATH
export LLVM_PASS_LIB="/fpcc/fpgenBench/llvm_pass/build/FPCCSan/libFPCCSanitizer.so -fpccsan "
