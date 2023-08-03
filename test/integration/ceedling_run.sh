#!/bin/sh

echo "Ceedling Testing..."
# call ceedling gcov:all

export LANG=C.UTF-8

ceedling clobber

if [ "$1" = "ALL" ]; then
    ceedling > "output/unit_test_log.txt"
else
    ceedling test:$1 > "output/unit_test_log.txt"
fi

echo "XML Test Outputs generating..."
gcovr --xml-pretty --output output/output.xml
 
echo "HTML Test Outputs generating..."
gcovr -r ../.. --filter ../../source/ --html-details --output output/index.html

echo "HTML Test Outputs generating... (lcov)"
cd build/test/out/ && lcov --capture --directory . --output-file coverage.info && genhtml coverage.info --output-directory coverage_html

echo "Ceedling done!..."

