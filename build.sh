mkdir -p build
cd build || return
CC="ccache clang" CXX="ccache clang++" cmake -G Ninja ..
#make -j"$(nproc)"
NINJA_STATUS="[%r processes, %f/%t @ %o/s : %es ] " CLICOLOR_FORCE=1 ninja
