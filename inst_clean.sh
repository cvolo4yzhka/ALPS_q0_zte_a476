#!/bin/bash
#export USE_CCACHE=1
#export _JAVA_OPTIONS="-Xmx4G"
source build/envsetup.sh
lunch full_zte_blade_a476-user
make installclean
#make clean
