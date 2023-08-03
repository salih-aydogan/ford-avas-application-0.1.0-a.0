import matplotlib.pyplot as plt

matched_bytes = 0
match_offset = 0
matched_start_offset = 0
unmatched_bytes = []
unmatched_byte_cnt = 0
unmatch_limit = 2000

raw_file_path = "Fiat_500_AVAS_Master_2dBAmplified.raw"
raw_file_path = "test_file.raw"

audio_samples = []

with open(raw_file_path, 'rb') as raw_file:
    while True:
        b = raw_file.read(1)
        if not b:
            # eof
            break
        audio_samples.append(int(b.hex(), 16))

print("File size:{} bytes.".format(len(audio_samples)))

start_offset_range = 1

for start_offset in range(start_offset_range):
    print("Checking offset: {}/{}".format(start_offset, start_offset_range), end='\r', flush=True)

    for start_idx, start_sample in enumerate(audio_samples):
        #print("Check idx:{}, value:{}".format(start_idx, start_sample))
        
        if audio_samples[start_offset] == start_sample:
            print("Start offset:{}, start_idx{}".format(start_offset, start_idx))
            match_cnt = 1
            unmatch_cnt = 0
            unmatch_bytes = []
        
            for check_idx, check_sample in enumerate(audio_samples[start_idx + 1:2*start_idx]):
                if audio_samples[check_idx + 1 + start_offset] == check_sample:
                    match_cnt = match_cnt + 1
                else:
                    unmatch_bytes.append(check_sample - (audio_samples[check_idx + 1 + start_offset]))
                    unmatch_cnt = unmatch_cnt + 1
                    if unmatch_cnt > unmatch_limit:
                        break

            if matched_bytes < match_cnt:
                matched_bytes = match_cnt
                match_offset = start_idx
                matched_start_offset = start_offset
                unmatched_bytes = unmatch_bytes
                unmatched_byte_cnt = unmatch_cnt

print("Matched bytes:{}, start_offset:{}, matched offset:{}, Unmatched byte count:{}".format(matched_bytes, matched_start_offset, match_offset, unmatched_byte_cnt))

with open(output_file_name, 'wb') as output_file:
    output_file.write(bytearray(audio_samples[matched_start_offset:match_offset]))

plt.plot(unmatched_bytes)
plt.ylabel('Unmatched Bytes')
plt.show()