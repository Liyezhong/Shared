#! /bin/bash

# arthor : arthur li
# date   : 2014.4.20

lcov -z -d ..
lcov -c -i -d .. -b $PWD -o base.info

find bin_rel -name 'ut*' -exec {} \;

lcov -c -d .. -b $PWD -o coverage.info

lcov -a base.info -a coverage.info -o total.info

lcov -r total.info '/usr/*' 'moc_*' '*Test*' '*Global/Include*' -o total.info

genhtml -o html total.info

rm -f *.info
