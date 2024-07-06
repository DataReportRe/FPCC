export CXX=clang++
export CC=clang
export EFT_HOME=/fpcc/eftTest/
export LD_LIBRARY_PATH=$EFT_HOME/runtime/obj:$LD_LIBRARY_PATH
export LLVM_PASS_LIB="/fpcc/eftTest/llvm_pass/build/EFTSan/libEFTSanitizer.so -eftsan "
