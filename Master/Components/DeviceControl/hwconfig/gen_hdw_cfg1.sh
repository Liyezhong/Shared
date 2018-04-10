#!/bin/sh

../../../../ExternalPackages/xsde/xsde-3.2.0-64/bin/xsde cxx-hybrid --output-dir "./" --no-exceptions --generate-parser --generate-serializer --generate-aggregate --cxx-suffix ".cpp" --hxx-suffix ".hpp" hwconfig.xsd
