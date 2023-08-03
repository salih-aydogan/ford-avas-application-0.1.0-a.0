//*****************************************************************************
// @file    /Jenkinsfile
// @author  Daiichi Arge Team - Sercan Agacdograyan, Yigit Cabas
// @version V1.0.0
// @date    21.11.2022
// @brief   Contains clone-compile-test command steps for Jenkins-CI platform
//***************************************************************************
// @attention
//
// COPYRIGHT(c) 2022 Daiichi Arge
//
//*****************************************************************************

/*******************************************************************************
 ************************ Pipeline configuration *******************************
 ******************************************************************************/
pipeline {
        agent any
    environment {
        HOME = '.'
    }
/*******************************************************************************
 ************************ Preperation stage ************************************
 ******************************************************************************/
    stages {
        stage('Preperation') {
            steps {
                echo 'Stage: Preperation'

                echo 'Preparing...'

                // Parsing Branch_Name from the detached HEAD commit
                script {
                    env.BRANCH_NAME = sh(script: "git show -s --pretty=%D HEAD | awk '{gsub(\"origin/\", \"\"); print \$2}'", returnStdout: true).trim()
                }

                // Parsing Commit_ID from the detached HEAD commit
                script {
                    env.COMMIT_LONG_ID = sh(script: "git rev-parse HEAD", returnStdout: true).trim()
                    env.COMMIT_SHORT_ID = sh(script: "git rev-parse --short HEAD", returnStdout: true).trim()
                }

                // Parsing Commit_Message from the detached HEAD commit
                script {
                    env.COMMIT_MESSAGE = sh(script: "git log -1 --pretty=%B", returnStdout: true).trim()
                }

                // Printing all environment variables
                sh "export LANG=C.UTF-8"
                echo sh(returnStdout: true, script: 'env')

                
            }
        }
        /*******************************************************************************
 ************************ Integration Testing stage ****************************
 ******************************************************************************/
        stage('Integration Testing') {
            when {
                expression {
                    return env.TEST_TYPE == 'integration';
                }
            }
            steps {
                echo 'Stage: Integration Testing'
                echo 'Running Integration Tests...'

                script {
                    try {
                        sh "rm -rf test/integration/output/*"
                        sh "cd test/integration && sh ceedling_run.sh ${CUSTOM_TEST_CASE}"
                        
                        currentBuild.result = 'SUCCESS'
                        echo "Ceedling Unit-Test Result: All tests passed" 
                    } catch (Exception err) {
                        parse_robot_exception(err)
                    }
                }
            }
        }
// /*******************************************************************************
//  ************************ Unit Testing stage ***********************************
//  ******************************************************************************/
        stage('Unit Testing') {
            when {
                expression {
                    return env.TEST_TYPE == 'unit';
                }
            }
            steps {
                echo 'Stage: Unit Testing'
                echo 'Running Unit Tests...'

                script {
                    try {
                        sh "rm -rf test/unit/output/*"
                        sh "cd test/unit && sh ceedling_run.sh ${CUSTOM_TEST_CASE}"
                        
                        currentBuild.result = 'SUCCESS'
                        echo "Ceedling Unit-Test Result: All tests passed" 
                    } catch (Exception err) {
                        echo "run_unit_test.bat output: " + err.getMessage()
                    }
                }

                echo 'RESULT = ' + currentBuild.result
            }
        }
/*******************************************************************************
 ************************ Static Testing stage *********************************
 ******************************************************************************/
//TODO: it is going to be activated when the analysis tool is ready.
//         stage('Static Testing') {
//             when {
//                 expression {
//                     return env.TEST_TYPE == 'static';
//                 }
//             }
//             agent {
//                 node {
//                     label "${SLAVE_NODE_NAME}"
//                     customWorkspace "${REPO_PATH_FOR_SLAVE}"
//                 }
//             }
//             steps {
//                 echo 'Stage: Static Testing'
//                 echo 'Running Static Tests...'

//                 script {
//                     try {
//                         // TODO : burada, tüm proje dosyaları için LDRA Test Set nasıl oluşturulup verilebilir o düşünülecek

//                         //slave_cmd("mkdir ${STAT_TEST_OUTPUT_PATH_FOR_SLAVE} && contestbed ${LDRA_TESTSET_FILE} -${LDRA_TEST_OPTIONS} -generate_code_review=ASCII,HTML -tb_workfiledir=${STAT_TEST_OUTPUT_PATH_FOR_SLAVE}")
                        
//                         currentBuild.result = 'SUCCESS'
//                     } catch (Exception err) {
//                         echo "contestbed output: " + err.getMessage()
//                     }
//                 }

//                 echo 'RESULT = ' + currentBuild.result
//             }
//         }
//     }
// /*******************************************************************************
//  ************************ Post stage *******************************************
//  ******************************************************************************/
    }
    post {
        always {
            // always runs regardless of the completion status of the Pipeline run
            echo "'Always' post step"
            delete_old_output_files()
        }
        success {
            //step will run only if the build is successful
            echo "'Success' post step"
            post_results()
        }
        failure {
            // only when the Pipeline is currently in a "failed" state run, 
            // usually expressed in the Web UI with the red indicator.
            echo "'Failure' post step"
        }
        unstable {
            // current Pipeline has "unstable" state, usually by a failed test, 
            // code violations and other causes, in order to run. 
            // Usually represented in a web UI with a yellow indication.
            echo "'Unstable' post step"
            post_results()
        }
    }
}
/*******************************************************************************
 ************************ Helper functions *************************************
 ******************************************************************************/

def post_results() {
    if (env.TEST_TYPE == 'integration') {
        echo "Integration Test Post Results"

        sh 'mkdir -p ./unity_report_output'
        sh 'cp test/integration/output/* unity_report_output'
        sh 'cp test/integration/build/artifacts/test/report.xml unity_report_output'
        sh 'cd test/integration/build/test/out && zip -r ${WORKSPACE}/unity_report_output/lcov_report.zip coverage_html'

        convert_unity_xml_to_xunit_xml()
        post_xunit_results_to_codebeamer()
        post_test_logs_as_attachments_to_codebeamer()   
    }
    if (env.TEST_TYPE == 'unit') {
        echo "Unit Test Post Results"

        sh 'mkdir -p ./unity_report_output'
        sh 'cp test/unit/output/* unity_report_output'
        sh 'cp test/unit/build/artifacts/test/report.xml unity_report_output'
        sh 'cd test/unit/build/test/out && zip -r ${WORKSPACE}/unity_report_output/lcov_report.zip coverage_html'

        convert_unity_xml_to_xunit_xml()
        post_xunit_results_to_codebeamer()
        post_test_logs_as_attachments_to_codebeamer()   
    }   
    if (env.TEST_TYPE == 'static') { 
        // TODO : burada, commit status bilgisi github'a Rest-Api üzerinden iletilebilir
        // TODO : burada, test sonuçlarını github'a veya codebeamer'a gönderme adımı eklenebilir
        //copy_ldra_results_to_master_node()
    }  
}

def convert_unity_xml_to_xunit_xml() {
    xunit reduceLog: false, tools: [Custom(customXSL: '/var/lib/jenkins/unity.xsl', deleteOutputFiles: false, failIfNotNew: true, pattern: "unity_report_output/report.xml", skipNoTestFiles: false, stopProcessingIfError: true)]
}

def post_xunit_results_to_codebeamer() {
   if (env.TEST_TYPE == 'integration') { 
        sh "cd generatedJUnitFiles && find -name '* *' -type d | rename 's/ /_/g' && cd .."
        sh 'cd generatedJUnitFiles && mv "$(ls -trh | tail -1)" f1'
        sh 'cd generatedJUnitFiles/f1 && mv "$(ls -trh | tail -1)" f2'
        sh 'cd generatedJUnitFiles/f1/f2 && mv "$(ls -trh | tail -1)" report_xunit.xml'

        sh "python3 test/environment/daiichi_trpu/TestResultUploader.py \
            ${CBEAMER_INTG_TESTCASE_TRACKER_ID} \
            ${CBEAMER_INTG_TESTRUN_TRACKER_ID} \
            generatedJUnitFiles/f1/f2/report_xunit.xml \
            test/integration/output/unit_test_log.txt \
            ${COMMIT_SHORT_ID} \
            ${ASSIGNED_TO} \
            ${TASK} \
            ${RELEASE}"

        // xUnitUploader \
        //     codebeamerUrl: 'https://codebeamer.daiichi.com', \
        //     credentialsId: 'a2a995d9-f5fe-4d6b-9fb9-41b1dee17edf', \
        //     defaultPackagePrefix: '', \
        //     disabled: false, \
        //     testCaseTrackerId: CBEAMER_INTG_TESTCASE_TRACKER_ID, \
        //     testConfigurationId: CBEAMER_INTG_TESTCONF_ID, \
        //     testResultsDir: "generatedJUnitFiles/f1/f2", \
        //     testRunTrackerId: CBEAMER_INTG_TESTRUN_TRACKER_ID   
   }
   if (env.TEST_TYPE == 'unit') { 
        sh "cd generatedJUnitFiles && find -name '* *' -type d | rename 's/ /_/g' && cd .."
        sh 'cd generatedJUnitFiles && mv "$(ls -trh | tail -1)" f1'
        sh 'cd generatedJUnitFiles/f1 && mv "$(ls -trh | tail -1)" f2'
        sh 'cd generatedJUnitFiles/f1/f2 && mv "$(ls -trh | tail -1)" report_xunit.xml'

        sh "python3 test/environment/daiichi_trpu/TestResultUploader.py \
            ${CBEAMER_UNIT_TESTCASE_TRACKER_ID} \
            ${CBEAMER_UNIT_TESTRUN_TRACKER_ID} \
            generatedJUnitFiles/f1/f2/report_xunit.xml \
            test/unit/output/unit_test_log.txt \
            ${COMMIT_SHORT_ID} \
            ${ASSIGNED_TO} \
            ${TASK} \
            ${RELEASE}"

        // xUnitUploader \
        //     codebeamerUrl: 'https://codebeamer.daiichi.com', \
        //     credentialsId: 'a2a995d9-f5fe-4d6b-9fb9-41b1dee17edf', \
        //     defaultPackagePrefix: '', \
        //     disabled: false, \
        //     testCaseTrackerId: CBEAMER_UNIT_TESTCASE_TRACKER_ID, \
        //     testConfigurationId: CBEAMER_UNIT_TESTCONF_ID, \
        //     testResultsDir: "generatedJUnitFiles/f1/f2", \
        //     testRunTrackerId: CBEAMER_UNIT_TESTRUN_TRACKER_ID
    } 
}


def delete_old_output_files() {
    sh "rm -rf generatedJUnitFiles"
    sh "rm -rf f1"
    sh "rm -rf unity_report_output/*"
}

def post_test_logs_as_attachments_to_codebeamer() {

    // Preparing test run name to search
    // Example Codebeamer xunit uploader test run format: 
    // ntpp-pipeline-release-deneme #403 [1]
    // Note: %23 = #  , %20 = space
    // def CBEAMER_TEST_RUN_NAME = "%27${JOB_NAME}%20%23${BUILD_NUMBER}%20[1]%27"
    //sh "echo CBEAMER_TEST_RUN_NAME = ${CBEAMER_TEST_RUN_NAME}"

    script {
        env.ENV_PARENT_TEST_RUN_ID = sh(script: "cat PARENT_TEST_RUN_ID", returnStdout: true).trim()
    }
    //sh "export ENV_PARENT_TEST_RUN_ID=`cat PARENT_TEST_RUN_ID`"

    // // Getting testrun id from codebeamer
    // def jsonTestrunString = sh (
    //     script: "curl --location --request GET \
    //                 'https://codebeamer.daiichi.com/api/v3/items/query?queryString=summary=${CBEAMER_TEST_RUN_NAME}' \
    //                 --header 'Authorization: Basic ${CB_AUTH_KEY}'",
    //     returnStdout: true
    // )
    // def jsonTestrunObj = readJSON text: jsonTestrunString
    
    def CBEAMER_TEST_RUN_ID = env.ENV_PARENT_TEST_RUN_ID   

    // sh "echo ${jsonTestrunObj}"
    sh "echo CBEAMER_TEST_RUN_ID = ${CBEAMER_TEST_RUN_ID}"

    final foundFiles
    def outputDir

    // Getting files to push to codebeamer
    if (env.TEST_TYPE == 'integration') { 
        foundFiles = sh(script: "ls -1 unity_report_output/", returnStdout: true).split()
        outputDir = "unity_report_output"
    }
    if (env.TEST_TYPE == 'unit') { 
        foundFiles = sh(script: "ls -1 unity_report_output/", returnStdout: true).split()
        outputDir = "unity_report_output"
    }

    // Pushing files to codebeamer
    for (file in foundFiles) {
        def jsonAttachmentString = sh (
            script: "curl -X POST \
            'https://codebeamer.daiichi.com/api/v3/items/${CBEAMER_TEST_RUN_ID}/attachments'  \
            --header 'accept: application/json'    \
            --header 'Authorization: Basic ${CB_AUTH_KEY}'   \
            --header 'Content-Type: multipart/form-data'   \
            --form  'attachments=@${outputDir}/${file};type=text/plain' ",
            returnStdout: true
        )
    }

    // Updating commit info in codebeamer test-run fields
    def jsonTestRunCommitInfoFields = " ' \
            { \
                \"fieldValues\": [  \
                    {       \
                        \"fieldId\": ${CBEAMER_TESTED_BRANCH_FIELD_ID}, \
                        \"type\": \"TextFieldValue\", \
                        \"name\": \"Tested Branch\",  \
                        \"value\": \"${BRANCH_NAME}\"  \
                    },      \
                    {       \
                        \"fieldId\": ${CBEAMER_TESTED_COMMIT_ID_FIELD_ID}, \
                        \"type\": \"TextFieldValue\", \
                        \"name\": \"Tested Commit ID\",  \
                        \"value\": \"${COMMIT_SHORT_ID}\" \
                    },      \
                    {       \
                        \"fieldId\": ${CBEAMER_TESTED_REPO_FIELD_ID},   \
                        \"type\": \"TextFieldValue\", \
                        \"name\": \"Tested Repo\",    \
                        \"value\": \"${REPO_NAME}\"   \
                    },      \
                    {       \
                        \"fieldId\": ${CBEAMER_TESTED_COMMIT_MESSAGE_FIELD_ID},   \
                        \"type\": \"TextFieldValue\", \
                        \"name\": \"Tested Repo\",    \
                        \"value\": \"${COMMIT_MESSAGE}\"   \
                    }       \
                ] \
            } ' "

    def jsonAttachmentStringCommitFields = sh (
        script: "curl -X PUT \
        'https://codebeamer.daiichi.com/api/v3/items/${CBEAMER_TEST_RUN_ID}/fields'  \
        --header 'accept: application/json'    \
        --header 'Authorization: Basic ${CB_AUTH_KEY}'   \
        --header 'Content-Type: application/json'   \
        --data-raw ${jsonTestRunCommitInfoFields} " ,
        returnStdout: true
    )
}