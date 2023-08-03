from array import array
from junitparser import JUnitXml
import os
import sys
import json
import subprocess
import shlex
import xml.etree.ElementTree as ET
import io

#Get test case name
#Parse test step data as Json

# Return array of {"name":val,"step_data":val} dictionary

def parse_robot_logs(robot_node, ret):
    tmp_step_log = []
    test_cnt = len(ret) 
    for test_case in robot_node.findall('./test'):
        ret.append({"name" : test_case.attrib["name"]})
        is_failed = False
        tmp_step_log.clear()
        step_cnt = 0
        for test_log in test_case.findall('./kw/msg'):
            if -1 < test_log.text.find("TEST STEP"):
                #Record test step data
                if 0 != step_cnt:
                    if "step_data" in ret[test_cnt]:
                        ret[test_cnt]["step_data"].append({"fieldId": 2000005,
                                                        "name": "Result",
                                                        "value": "PASSED" if False == is_failed else "FAILED",
                                                        "type": "TextFieldValue"})
                        ret[test_cnt]["step_data"].append({"fieldId": 2000004,
                                                        "name": "Actual result",
                                                        "value": "{{{" + str(tmp_step_log).replace("'", "") + "}}}" if 0 != len(tmp_step_log) else "---",
                                                        "type": "WikiTextFieldValue"})
                    else:
                        new_key = {"step_data" : [{"fieldId": 2000005,
                                                    "name": "Result",
                                                    "value": "PASSED" if False == is_failed else "FAILED",
                                                    "type": "TextFieldValue"},
                                                    {"fieldId": 2000004,
                                                    "name": "Actual result",
                                                    "value": "{{{" + str(tmp_step_log).replace("'", "") + "}}}" if 0 != len(tmp_step_log) else "---",
                                                    "type": "WikiTextFieldValue"}]}
                        ret[test_cnt].update(new_key)
                        
                step_cnt = step_cnt + 1
                
                #Reset tmp_step_log
                tmp_step_log.clear()
                is_failed = False
            else:
                #Keep recording
                if "FAIL" ==  test_log.attrib["level"]:
                    is_failed = True
                
                if -1 == test_log.text.find("b''"):
                    tmp_step_log.append(test_log.text)

        # For remaning logs if exist
        #if 0 != len(tmp_step_log) and 0 != step_cnt:
        if 0 != len(tmp_step_log):
            if "step_data" in ret[test_cnt]:
                ret[test_cnt]["step_data"].append({"fieldId": 2000005,
                                                "name": "Result",
                                                "value": "PASSED" if False == is_failed else "FAILED",
                                                "type": "TextFieldValue"})
                ret[test_cnt]["step_data"].append({"fieldId": 2000004,
                                                "name": "Actual result",
                                                "value": "{{{" + str(tmp_step_log).replace("'", "") + "}}}" if 0 != len(tmp_step_log) else "---",
                                                "type": "WikiTextFieldValue"})
            else:
                new_key = {"step_data" : [{"fieldId": 2000005,
                                            "name": "Result",
                                            "value": "PASSED" if False == is_failed else "FAILED",
                                            "type": "TextFieldValue"},
                                            {"fieldId": 2000004,
                                            "name": "Actual result",
                                            "value": "{{{" + str(tmp_step_log).replace("'", "") + "}}}" if 0 != len(tmp_step_log) else "---",
                                            "type": "WikiTextFieldValue"}]}
                ret[test_cnt].update(new_key)

        test_cnt = test_cnt + 1
    

def recursive_suite_find(node, ret):
    for suit in node.findall('./suite'):
        if -1 < suit.attrib["source"].find(".robot"):
            parse_robot_logs(suit, ret)
        else:
            recursive_suite_find(suit, ret)

def parse_robot_log_xml(file_path:str):
    print("PARSE ROBOT LOG XML")

    ret = []

    tree = ET.parse(file_path)
    root = tree.getroot()

    recursive_suite_find(root, ret)

    return ret

def record_step_log(ret, record, is_failed, test_cnt):
    #Save step val
    if "step_data" in ret[test_cnt]:
        ret[test_cnt]["step_data"].append([{"fieldId": 2000005,
                                            "name": "Result",
                                            "value": "PASSED" if False == is_failed else "FAILED",
                                            "type": "TextFieldValue"},
                                            {"fieldId": 2000004,
                                            "name": "Actual result",
                                            "value": str(record).replace("'", "").replace("[", "").replace("]", "").replace(", ", ""),
                                            "type": "WikiTextFieldValue"}])
    else:
        new_key = {"step_data" : [[{"fieldId": 2000005,
                                    "name": "Result",
                                    "value": "PASSED" if False == is_failed else "FAILED",
                                    "type": "TextFieldValue"},
                                    {"fieldId": 2000004,
                                    "name": "Actual result",
                                    "value": str(record).replace("'", "").replace("[", "").replace("]", "").replace(", ", ""),
                                    "type": "WikiTextFieldValue"}]]}
                                    
        ret[test_cnt].update(new_key)

# Return array of {"name":val,"step_data":val} dictionary
def parse_ceedling_log_txt(file_path:str):
    print("CEEDLING LOG TXT")

    ret = []

    try:
        fd = open(file_path, mode='r', encoding='utf-8')
    except Exception as e:
        print("Cannot open log file: " + str(e))

    test_cnt = -1
    tmp_step_log = []
    new_step = 0
    last_step = 0
    is_failed = False
    new_test_case= False

    for file_line in fd:
        parsed_line = file_line.split(":")

        if -1 < parsed_line[0].find("TEST CASE"):
            #Create new test case
            ret.append({"name" : parsed_line[1]})

            if 0 < new_step and 0 != len(tmp_step_log):
                #Record remaining test log
                record_step_log(ret, tmp_step_log, is_failed, test_cnt)

            #Prepare for new test
            new_test_case = False
            test_cnt = test_cnt + 1
            is_failed = False
            new_step = 0
            last_step = 0
            tmp_step_log.clear()
        else:
            #Test step log
            if -1 < parsed_line[0].find("TEST STEP"):
                new_step = new_step + 1
                new_test_case = True
                
                if last_step != new_step and 1 < new_step:
                    last_step = new_step
                    #Save step val
                    record_step_log(ret, tmp_step_log, is_failed, test_cnt)
                    
                    #Reset tmp_step_log
                    tmp_step_log.clear()
                    is_failed = False
            #Test assertion log 
            elif new_test_case == True:
                #Prepare JSON
                #Keep recording
                if 4 < len(parsed_line):
                    if "FAIL" ==  parsed_line[3]:
                        is_failed = True
                    file_line = file_line.replace("\n", "").replace("_", "~_")
                    tmp_step_log.append("{{ " + file_line + " }}\\")
                else:
                    tmp_step_log.append("")

    #Save remaining test logs
    if 0 < new_step and 0 != len(tmp_step_log):
        record_step_log(ret, tmp_step_log, is_failed, test_cnt)
    
    return ret
        
# ret = parse_ceedling_log_txt("output.txt")
# # for i in range(0, len(ret)):
# #     for cnt in ret[i]["step_data"]:
# #         print(cnt)
# #         print("\n")
# print(ret)