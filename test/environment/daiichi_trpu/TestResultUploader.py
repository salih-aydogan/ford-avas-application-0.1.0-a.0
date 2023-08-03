from array import array
from junitparser import JUnitXml
import os
import sys
import json
import subprocess
import shlex
from TestResultParser import *
import numpy as np

# arg 0 <this script name>(automatically given by python)
# arg 1 <test specification tracker id>
# arg 2 <test run tracker id>
# arg 3 <xunit result file>
# arg 4 <test log result file>
# arg 5 <6 digit of last commit hash>
# arg 6 <user id for assgined to field>
# arg 7 <task id>
# arg 8 <release id>

# test_list id-test_function-type list
# case_result passed/failed-function_name list
def parse_and_upload(test_list, case_result):
    # Parse the cases and match them with results
    test_run_create_param = {
                                "testCaseIds": [], 
                                "testRunModel": {
                                    "assignedTo": [
                                        {
                                            "id": int(sys.argv[6]),
                                            "type": "UserReference"
                                        }
                                    ], 
                                    "customFields": [
                                        {
                                            "fieldId": 10001,
                                            "name": "Build",
                                            "type": "TextFieldValue",
                                            "value": str(sys.argv[5])
                                        },
                                        {
                                            "fieldId": 1000,
                                            "name": "TASK",
                                            "type": "ChoiceFieldValue",
                                            "values": [
                                                {
                                                    "id" : int(sys.argv[7]),
                                                    "type": "TrackerItemReference"
                                                }
                                            ]
                                        }
                                    ], 
                                    "versions": [
                                        {
                                            "id" : int(sys.argv[8]),
                                            "type": "TrackerItemReference"
                                        }
                                    ]
                                }
                            }
    test_run_update_param = []
    
    for test_case in case_result["itemRefs"]:
        for test_run in test_list:
            if test_run["name"] == test_case["name"]:
                test_run_create_param["testCaseIds"].append({
                                                                "id":test_case["id"],
                                                                "name":test_case["name"],
                                                                "type":"TrackerItemReference"
                                                            })
                test_run_update_param.append({"testCaseReference": {"id":test_case["id"],
                                                                    "name":test_case["name"],
                                                                    "type":"TrackerItemReference"},
                                                                    "result": test_run["result"],
                                                                    "runTime": 0})

    if 0 == len(test_run_create_param["testCaseIds"]):
        print("EMPTY LIST")
        return
    
    # Create test runs in Codebeamer
    create_json_param = json.dumps(test_run_create_param)

    print("Created Test Run. Post :")
    print(str(create_json_param))

    cmd = ("curl --location --request POST \"https://codebeamer.daiichi.com/api/v3/trackers/" + 
            str(sys.argv[2]) + 
            "/testruns\"" +     
            " --header \"Authorization: Basic eWlnaXQtY2FiYXM6YU10QkVTaDE=\"" + 
            " --header \"Content-Type: application/json\"" + 
            " --data-raw '" + 
            str(create_json_param) + "'")

    parsed_arg = shlex.split(cmd)
    create_response = subprocess.Popen(parsed_arg, stdout=subprocess.PIPE)
    create_response.wait()
    resp_json = json.loads(create_response.stdout.read())

    print("Created Test Run. Post Response :")
    print(str(resp_json))

    parent_test_run_id = resp_json["id"]
    os.system("echo " + str(parent_test_run_id) + " > PARENT_TEST_RUN_ID")

    # if not qualification
    if(os.getenv("TEST_TYPE").find("qualification") == -1):
        #Update test run step fields
        if -1 < str(sys.argv[4]).find(".xml"):
            print("Parsing Robot FW test log.\n")
            parsed_test_log_list = parse_robot_log_xml(str(sys.argv[4]))
        elif -1 < str(sys.argv[4]).find(".txt"):
            parsed_test_log_list = parse_ceedling_log_txt(str(sys.argv[4]))
            print("Parsing Ceedling test log.\n")
        else:
            print("Unknown test log file\n")
            exit(-1)

        for cnt_functions_test_specs in range(0, len(test_run_update_param)):
            custom_fields = {"custom_fields":[{"fieldId": 2000000,
                                                "name": "Test Step Results",
                                                "type": "TableFieldValue",
                                                "values":[]}]}

            # ID ile test case'e get isteği : Expect ile Action alanlarını almak için
            cmd_update = ("curl --location --request GET 'https://codebeamer.daiichi.com/api/v3/items/" + 
                                    str(test_run_update_param[cnt_functions_test_specs]["testCaseReference"]["id"]) + "'" +  
                                    " --header 'Authorization: Basic eWlnaXQtY2FiYXM6YU10QkVTaDE='" + 
                                    " --header 'Content-Type: application/json'")

            parsed_update_arg = shlex.split(cmd_update)
            create_response_update = subprocess.Popen(parsed_update_arg, stdout=subprocess.PIPE)
            create_response_update.wait()
            response_test_case = json.loads(create_response_update.stdout.read())

            # Fonksiyon eşleştirme ve paket hazırlama

            for cnt_functions_test_code in range(0, len(parsed_test_log_list)):
                if parsed_test_log_list[cnt_functions_test_code]["name"] == test_run_update_param[cnt_functions_test_specs]["testCaseReference"]["name"]:
                    if "step_data" in parsed_test_log_list[cnt_functions_test_code]:
                        # Fonksiyon test sonucu bulundu:  custom_fields alanları kadar dön 
                        custom_fields["custom_fields"][0]["values"] = parsed_test_log_list[cnt_functions_test_code]["step_data"]
                    # else:
                    #     custom_fields["custom_fields"][0]["values"] = []

                    for cnt_3 in range(len(response_test_case["customFields"])):
                        # Test Steps alanının olduğu json objeti mi?
                        if int(response_test_case["customFields"][cnt_3]["fieldId"]) == 1000000:
                            steps = response_test_case["customFields"][cnt_3]["values"]
                            # Specteki test stepleriyle koddakiler eşit mi?
                            steps_in_code = len(custom_fields["custom_fields"][0]["values"])
                            steps_in_specs = len(steps)
                            if steps_in_specs != steps_in_code:
                                # Fail durumunda da basılacak lakin fail durumu haricinde
                                # test specteki test stepslerin sayısının koddaki stepslerin sayısı kadar
                                # olmadığı durumda yazılır.
                                print(str(test_run_update_param[cnt_functions_test_specs]["testCaseReference"]["name"]))
                                # Steps kadar dön

                                for idx in range(steps_in_code, steps_in_specs):
                                    test_run_update_param[cnt_functions_test_specs]["result"] = "FAILED"
                                    custom_fields["custom_fields"][0]["values"].append([{"fieldId": 2000005,
                                            "name": "Result",
                                            "value": "FAILED",
                                            "type": "TextFieldValue"},
                                            {"fieldId": 2000004,
                                            "name": "Actual result",
                                            "value": "",
                                            "type": "WikiTextFieldValue"}])

                                for idx in range(steps_in_specs, steps_in_code):
                                    test_run_update_param[cnt_functions_test_specs]["result"] = "FAILED"

                                    for idx2 in range(0, len(custom_fields["custom_fields"][0]["values"][idx])):
                                        if int(custom_fields["custom_fields"][0]["values"][idx][idx2]["fieldId"]) == 2000005:
                                            custom_fields["custom_fields"][0]["values"][idx][idx2]["value"] = "FAILED"
                                            
                            for cnt_4 in range(0, steps_in_specs):
                                values = steps[cnt_4]
                                for cnt_5 in range(0, len(values)):
                                    # Fieldler kadar dön
                                    if int(values[cnt_5]["fieldId"]) == 1000002:
                                        # Expected result
                                        custom_fields["custom_fields"][0]["values"][cnt_4].append({"fieldId": 2000002,
                                                    "name":values[cnt_5]["name"],
                                                    "value" : values[cnt_5]["value"],
                                                    "type": values[cnt_5]["type"]})
                                    elif int(values[cnt_5]["fieldId"]) == 1000001:
                                        # Action result
                                        custom_fields["custom_fields"][0]["values"][cnt_4].append({"fieldId": 2000001,
                                                    "name":values[cnt_5]["name"],
                                                    "value" : values[cnt_5]["value"],
                                                    "type": values[cnt_5]["type"]})

            test_run_update_param[cnt_functions_test_specs]["customFields"] = custom_fields["custom_fields"]
            custom_fields.clear()
        

    update_param = {"updateRequestModels" : test_run_update_param, 
                    "parentResultPropagation": True}

    try:
        json_fd = open("json_param.json", mode='w')
    except Exception as e:
        print("Cannot open JSON file: " + str(e))
        exit(-1)

    print("JSON DATA")
    print(str(json.dumps(update_param)))
    print("JSON DATA")

    json_fd.write(json.dumps(update_param))
    json_fd.flush()
    json_fd.close()

    #Update test run results(This step should be done lastly because after doing it, 
    #the test run state changed to "Finished" directly)
    cmd_update = ("curl --location --request PUT 'https://codebeamer.daiichi.com/api/v3/testruns/" + 
                str(parent_test_run_id) + "'" +  
                " --header 'Authorization: Basic eWlnaXQtY2FiYXM6YU10QkVTaDE='" + 
                " --header 'Content-Type: application/json'" + 
                " --data '@json_param.json'")

    parsed_update_arg = shlex.split(cmd_update)
    create_response_update = subprocess.Popen(parsed_update_arg, stdout=subprocess.PIPE)
    create_response_update.wait()

    print("PUT RESULT")
    print(create_response_update.stdout.read())

    os.system("rm -rf json_param.json")

#
# SCRIPT
#
xml = JUnitXml.fromfile(str(sys.argv[3]))

test_run_list = []

#Parse xunit xml test results
for suite in xml:
    failure_count = 0
    
    for cntx in suite:
        failure_count = failure_count + 1

    if 0 == failure_count:
        test_run_list.append({"name": suite.name, "result":"PASSED"})        
    else:
        test_run_list.append({"name": suite.name, "result":"FAILED"})
    

page_count = 1
test_case_result = json.loads('{"itemRefs": []}')

#Get test cases in the given tracker
while True:
    # Get Test Specs
    # {
    #      "id":86158,
    #      "name":"test_tps7b770_init__success",
    #      "type":"TrackerItemReference"
    # }
    cmd = ("curl --location --request GET \"https://codebeamer.daiichi.com/api/v3/trackers/" + 
            str(sys.argv[1]) + 
            "/items?page=" + str(page_count) + "&pageSize=200\"" + 
            " --header \"Authorization: Basic eWlnaXQtY2FiYXM6YU10QkVTaDE=\"" + 
            " --header \"Content-Type: application/json\"")

    page_count = page_count + 1

    parsed_arg = shlex.split(cmd)
    create_response = subprocess.Popen(parsed_arg, stdout=subprocess.PIPE)
    create_response.wait()
    
    page_case_result = json.loads(create_response.stdout.read())

    if len(page_case_result["itemRefs"]) == 0:
        break

    test_case_result["itemRefs"].extend(page_case_result["itemRefs"])

print("PRINT TEST CASES")
print(str(test_case_result))

if len(test_case_result["itemRefs"]) != 0:
    parse_and_upload(test_run_list, test_case_result)
else:
    print("FINISH")

