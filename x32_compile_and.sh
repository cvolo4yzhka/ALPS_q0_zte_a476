#!/bin/bash
#export USE_CCACHE=true
#export CCACHE_DIR=/home/cvolo4yzhka/.ccache
#export CCACHE_EXEC=/usr/bin/ccache
#ccache -M 50G
#export _JAVA_OPTIONS="-Xmx6G"
export WITHOUT_CHECK_API=true
source build/envsetup.sh
lunch full_zte_blade_a476-user
make
#make 2>&1 | tee out/build_k49.log
