#!/bin/bash
# make directory for release key
mkdir security
# jump to root of source
cd ../../..
# add cert info
subject='/C=RU/ST=Tatarstan/L=Almetevsk/O=cvolo4yzhka,Inc./OU=cvolo4yzhka Mobility/CN=cvolo4yzhka/emailAddress=cvolo4yzhka@gmail.com'
# generate all cert and keys, 
# on password promt, press enter (empty/none password)
# put key and cert to security folder
for x in releasekey platform shared media networkstack testkey verity verity_key; do \
./development/tools/make_key ./device/zte/zte_blade_a476/security/$x "$subject"; \
done
cd ./device/zte/zte_blade_a476/
