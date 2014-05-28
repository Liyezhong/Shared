#! /bin/bash

# author : arthur li
# date   : 2014.5.26

lcov -z -d ..
lcov -c -i -d .. -b $PWD -o base.info

cwd=$PWD
cd bin_rel
find . -name 'ut*' -exec {} \;
cd ..

lcov -c -d .. -b $PWD -o coverage.info

lcov -a base.info -a coverage.info -o total.info

lcov -r total.info  '*DataManager*' '*NetCommands/Include*' '*DataLogging/Include*' '*Threads/Include*' '*Global/Include*' '/usr/*' '*opensource-src*' 'moc_*' '*Test/*' '*HimalayaDataContainer*' -o total.info

genhtml -o html total.info

rm -f *.info
