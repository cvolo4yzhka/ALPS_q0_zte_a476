#!/bin/bash
cd ../../../..

subject='/C=RU/ST=Tatarstan/L=Almetevsk/O=cvolo4yzhka,Inc./OU=cvolo4yzhka Mobility/CN=cvolo4yzhka/emailAddress=cvolo4yzhka@gmail.com'
for x in releasekey platform shared media networkstack testkey verity verity_key; do \
./development/tools/make_key ./device/zte/zte_blade_a476/security/$x "$subject"; \
done

cd ./device/zte/zte_blade_a476/security/
