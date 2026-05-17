#!/bin/bash

# Check we hve a header file name as argument
if [ $# -ne 1 ]; then
    >&2 echo "Missing header-file argument"
    exit 1
fi

# Check if the header file exists
header=$1
if [ ! -f $1 ]; then
    >&2 echo "File not found: $1"
    exit 1
fi

# Generate simple XML output, enough to generate the index.html file
echo "<translation-unit>"
echo "  <filename>$header</filename>"
echo "  <includes>"
grep "\#include" $header | sed s'|.*<\(.*\)>|    <include>\1</include>|'
echo "  </includes>"
echo "</translation-unit>"
