__author__ = "Enes AYDIN"
__copyright__ = "Copyright 2021"
__version__ = "0.0.1"
__email__ = "enes.aydin@daiichi.com"

import sys
import time
import base64
import requests
import xml.etree.ElementTree as ET

################################################################################

# Configuration

QAC_URL = 'http://11.1.0.217:8080'
LOGIN_USERNAME = 'admin'
LOGIN_PASSWORD = 'admin'

################################################################################

# Ceaser algorithm

def caesar(a: str) -> str:
    base = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ"
    trgt = "nopqrstuvwxyzabcdefghijklmNOPQRSTUVWXYZABCDEFGHIJKLM"

    csr: dict = {k: v for k, v in zip(base, trgt)}

    result = [csr.get(i, i) for i in a]
    return "".join(result)

################################################################################

# Use 'with' to ensure the session context is closed after use.

with requests.Session() as s:
    LOGIN_URL = QAC_URL
    LOGIN_URL += '?request=14&db=&reqid='

    HEADERS = { 'User-Agent': 'Mozilla/5.0 (Windows NT 10.0; Win64; x64; rv:91.0) Gecko/20100101 Firefox/91.0' }

    LOGIN_REQ = '<qav><user_list><user login=\"' + LOGIN_USERNAME
    LOGIN_REQ += '\" password=\"' + caesar(base64.b64encode(LOGIN_PASSWORD.encode('ascii')).decode('ascii'))
    LOGIN_REQ += '\" /></user_list></qav>'

    req_id = round(time.time() * 10000)

    print('Login request...')

    if s.post(LOGIN_URL + str(req_id), data=LOGIN_REQ, headers=HEADERS).status_code == 200:

        SS_INFO_URL = QAC_URL
        SS_INFO_URL += '/viewer.html?request=23&db=' + sys.argv[1]
        SS_INFO_URL += '&reqid=' + str(req_id + 1)

        print('Getting snapshot ids...')
        ss_xml = s.get(SS_INFO_URL).text

        parsed_ss_xml = ET.fromstring(ss_xml)
        ss_id = parsed_ss_xml.findall('ss_comp/ss')[-1].get('id')

        RESULT_URL = QAC_URL
        RESULT_URL += '/viewer.html?request=33&start=1&next=500000&orderby=file&desc=0&db=' + sys.argv[1]
        RESULT_URL += '&ss='+ ss_id + '&bl=&sv=0&sps=&id=0&sh=0&reqid=' + str(req_id + 2)

        print('Download request result of analysis... 15~20 seconds')
        encrypted_text = s.get(RESULT_URL).text

        print('Decrypt response...5~10 seconds')
        base64_text = caesar(encrypted_text)

        print('Decode base64...')
        result_xml = base64.b64decode(base64_text)

        DIAG_MESSAGES_URL = QAC_URL
        DIAG_MESSAGES_URL += '/viewer.html?request=7&hidden=1&db=' + sys.argv[1]
        DIAG_MESSAGES_URL += '&reqid=' + str(req_id + 3)
        
        print('Download diag messages list...')
        diag_xml = s.get(DIAG_MESSAGES_URL).text

        # text_file = open("Output.txt", "wb")
        # text_file.write(result_xml)
        # text_file.close()

        if (result_xml != b'') and (diag_xml != ''):
            parsed_result = ET.fromstring(result_xml)
            parsed_diag_xml = ET.fromstring(diag_xml)

            for diag_el in parsed_result.findall('warning_listing/diag'):
                if None != diag_el.get('supp') and diag_el.get('supp') == '1':
                    continue
                if None == diag_el.get('filename')[11:] or '' == diag_el.get('filename')[11:]:
                    continue

                b = tuple(map(lambda i: i.get("text"), diag_el.findall(f".//*[@text]")))
                message = parsed_diag_xml.find(f".//*[@id='{diag_el.get('msg')}']").get('text')
                cnt_param = message.count('%')

                if b and (cnt_param != 0):
                    message = message.replace('%s', "{0}").replace('%1s', "{0}").replace('%2s', "{1}").replace('%3s', "{2}").replace('%4s', "{3}")
                    message = message.format(*b[:cnt_param])
                if len(message) != 0:
                    link = QAC_URL + "/viewer.html?request=15&db=" + sys.argv[1] + "&msg=" + diag_el.get('msg')
                    print("Diag: " + diag_el.get('filename')[11:] + ":" + diag_el.get('line') + ":" + diag_el.get('column') + " " + diag_el.get('msg') + " " + message + " Detail: " + link)
                else:
                    print("Diag message bulunamadı!")
        else:
            print("Proje bulunamadı veya proje sonuçları yüklenmedi!")
    else:
        print("Oturum açılamadı!")
