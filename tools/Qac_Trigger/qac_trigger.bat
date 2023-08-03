@echo off
Rem=============================================================================
Rem          FILE: qac_trigger.bat
Rem
Rem   USAGE-1(Branch is automatically get from current folder): 
Rem   Bash qac_trigger.bat
Rem
Rem   DESCRIPTION: To trigger QAC Tool and  
Rem                upload results to QAC dashboard (http://11.1.0.217:8080/)
Rem
Rem        AUTHOR: Kerem Burak Karci
Rem  ORGANIZATION: DAIICHI R&D
Rem=============================================================================

setlocal
call :setESC

set python_request_lib__control=1

FOR /F "tokens=* USEBACKQ" %%F IN (`python -m pip freeze`) DO (
    echo.%%F | findstr /C:"request">nul && (
        set python_request_lib__control=0
    )
)

IF %python_request_lib__control% == 1 (
    echo %ESC%[31mERROR: You have not python request library!%ESC%[0m
    echo Please download request library for python.
    echo     python -m pip install requests
    echo.

    exit
)

echo %ESC%[32mTriggering QAC...%ESC%[0m

for /f "delims=" %%# in ('powershell get-date -format "{ddMMyyyyHHmmss}"') do @set _date=%%#

echo Getting branch name...

FOR /F "tokens=* USEBACKQ" %%F IN (`git rev-parse --abbrev-ref HEAD`) DO (
    SET branch_name=%%F
)

echo Getting github username...

FOR /F "tokens=* USEBACKQ" %%F IN (`git config user.name`) DO (
    SET user_name=%%F
)

echo Working with BRANCH NAME:%ESC%[33m%BRANCH_NAME%%ESC%[0m

echo Hi dear %ESC%[35m%user_name%!%ESC%[0m Please control the branch name, if it is wrong please stop the script! If it is okey, then press enter to continue..

pause >nul

tools\Qac_Trigger\plink.exe -no-antispoof -ssh -t -t buildadmin@11.1.0.217 -pw 123 bash /home/buildadmin/Projects/ford_avas.sh %branch_name% %user_name% %_date%

IF %ERRORLEVEL% == 1 (
    python tools\Qac_Trigger\get_qac_result\main.py FORD_AVAS
    start http://11.1.0.217:8080/viewer.html?^&db=FORD_AVAS^&ss=^&bl=^&sv=0^&sps=#JmFubj0mbWxpc3Q9JnRyZWVpdGVtPSZ0cmVldGFiPTImYW5ubXNnPSZhbm5saW5lPSZzcz0mYmw9JnNoPTAmc3Y9MCZocD0mc209JnhhPSZobj0mc3BzPSZtZj0=
    exit
    echo Your dashboard url is open with default browser.
)

:setESC
for /F "tokens=1,2 delims=#" %%a in ('"prompt #$H#$E# & echo on & for %%b in (1) do rem"') do (
  set ESC=%%b
  exit /B 0
)