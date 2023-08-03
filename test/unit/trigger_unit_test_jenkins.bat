@echo off
Rem=============================================================================
Rem          FILE: trigger_unit_test_jenkins.bat
Rem
Rem   USAGE-1(Branch is automatically get from current folder): 
Rem   Bash trigger_unit_test_jenkins.bat
Rem
Rem   USAGE-2(Branch is manually selected): 
Rem   Bash trigger_unit_test_jenkins.bat master
Rem
Rem   DESCRIPTION: To trigger Unit Tests on Jenkins and  
Rem                upload results to Codebeamer
Rem
Rem        AUTHOR: Sercan Agacdograyan, Yigit Cabas, Enes AYDIN
Rem  ORGANIZATION: DAIICHI R&D
Rem=============================================================================

echo "Triggering Jenkins Unit Tests..."

setlocal EnableDelayedExpansion

SET JENKINS_URL=http://11.1.0.217:8090/fuego/job
SET JENKINS_JOB_NAME=FORD-AVAS
SET JENKINS_JOB_TOKEN=daiichi-ford-avas-token
SET JENKINS_API_USER=jenkins
SET JENKINS_API_TOKEN=11b2fec20b6c990df24f1effb2c544feed
SET TEST_TYPE=unit
SET BRANCH_NAME=
SET CUSTOM_TEST_CASE=

Rem ------Setting Repo and Branch Names------

ECHO "Getting Branch Name from current checked-out branch"
FOR /F "tokens=* USEBACKQ" %%F IN (`git rev-parse --abbrev-ref HEAD`) DO (
    SET BRANCH_NAME=%%F
)

FOR %%a IN (%*) DO (
    CALL SET "%%~1=%%~2"
    shift
)

IF [%--userid%] == [] (
    ECHO "Please, input user id. Example : --userid=18"
    EXIT /b
)

IF [%--task%] == [] (
    ECHO "Please, input task id. Example : --task=123456"
    EXIT /b
)

IF [%--release%] == [] (
    ECHO "Please, input release. Example : --release=54758"
    EXIT /b
)

cd test

IF [%--test%] == [] (
    ECHO "Setting CUSTOM_TEST_CASE ALL"

    ECHO "Sending jenkins job request"

    FOR /F "delims=" %%F IN ('dir /B test_*') DO (
        SET CUSTOM_TEST_CASE=%%F

        curl -u %JENKINS_API_USER%:%JENKINS_API_TOKEN% --location --request GET "%JENKINS_URL%/%JENKINS_JOB_NAME%/buildWithParameters?token=%JENKINS_JOB_TOKEN%&BRANCH_NAME=%BRANCH_NAME%&TEST_TYPE=%TEST_TYPE%&CUSTOM_TEST_CASE=!CUSTOM_TEST_CASE!&ASSIGNED_TO=%--userid%&TASK=%--task%&RELEASE=%--release%"
    )
) ELSE (
    ECHO "Getting CUSTOM_TEST_CASE from your batch input"
    SET CUSTOM_TEST_CASE=%--test%

    ECHO "Sending jenkins job request"

    curl -u %JENKINS_API_USER%:%JENKINS_API_TOKEN% --location --request GET "%JENKINS_URL%/%JENKINS_JOB_NAME%/buildWithParameters?token=%JENKINS_JOB_TOKEN%&BRANCH_NAME=%BRANCH_NAME%&TEST_TYPE=%TEST_TYPE%&CUSTOM_TEST_CASE=!CUSTOM_TEST_CASE!&ASSIGNED_TO=%--userid%&TASK=%--task%&RELEASE=%--release%"
)

cd ..

echo "Script done!..."
