#!/bin/bash
export CCACHE_DIR=/home/cvolo4yzhka/source/.ccache
ccache -M 50G
export USE_CCACHE=1
export _JAVA_OPTIONS="-Xmx3G"
source build/envsetup.sh
lunch full_zte_blade_a476-user
make -j2 2>&1 | tee out/build_k49.log
