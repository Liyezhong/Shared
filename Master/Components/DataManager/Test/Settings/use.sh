#!/bin/bash

echo use hw_specification.xml with $1

ln -s -f ../../../../../../Settings/hw_specification.xml .
rm Instrument
ln -s ../../../../../../Settings/$1 Instrument

