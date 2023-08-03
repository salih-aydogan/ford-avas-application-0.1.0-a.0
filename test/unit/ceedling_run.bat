@echo off
Rem=============================================================================
Rem          FILE: ceedling_run.sh
Rem
Rem         USAGE: Bash ceedling_run.sh
Rem
Rem   DESCRIPTION: To generate Unit Test and Code Coverage outputs.
Rem
Rem=============================================================================

echo "Ceedling Testing..."
Rem call ceedling gcov:all

for /f "delims=" %%# in ('powershell get-date -format "{dd_MM_yyyy_HH_mm_ss}"') do @set _date=%%#

call ceedling clobber

IF [%1] == [] (
    
    call ceedling > "output/log_%_date%.txt"

) ELSE (

    call ceedling test:%1
    
)
 
echo "XML Test Outputs generating..."
gcovr.exe --xml-pretty --output output/output.xml
 
echo "HTML Test Outputs generating..."
gcovr.exe -r ../.. --exclude ../../source/util/string_lib.c --filter ../../source/ --html-details --output output/index.html

echo "HTML Test Outputs generating... (lcov)"
Rem cd build/gcov/out/ && lcov --capture --directory . --output-file coverage.info && genhtml coverage.info --output-directory coverage_html

echo "Ceedling done!..."
