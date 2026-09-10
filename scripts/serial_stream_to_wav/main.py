import serial
import numpy as np
import sounddevice as sd
from datetime import datetime
import wave
import sys
import os

argv = sys.argv

port = '/dev/ttyUSB0'
out_dir = 'out'
baudrate = 921600
sample_rate = 16000
block_size = 512
file_count = 1
duration = 1

opts = ['--out-dir', '--sample-rate', '--block-size', '--baudrate', '--port', '--file-count', '--duration', '-h']

for i, arg in enumerate(argv):
	match arg:
		case '--out-dir':
			out_dir = argv[i+1]
			break
		case '--sample-rate':
			sample_rate = int(argv[i+1])
			break
		case '--block-size':
			block_size = int(argv[i+1])
			break
		case '--baudrate':
			baudrate = int(argv[i+1])
			break
		case '--port':
			port = argv[i+1]
			break
		case '--file-count':
			file_count = int(argv[i+1])
			break
		case '--duration':
			duration = int(argv[i+1])
			break
		case '-h':
			for opt in opts:
				print(opt)

			exit(0)

os.makedirs(out_dir, exist_ok=True)

ser = serial.Serial(port=port, baudrate=baudrate, timeout=1)

for i in range(file_count):
	wf = wave.open(f"{out_dir}/{datetime.now().strftime('%Y%m%d_%H%M%S')}.wav", "wb")
	wf.setnchannels(1)
	wf.setsampwidth(2)
	wf.setframerate(sample_rate)

	try:
		total_samples = sample_rate * duration
		remaining_samples = total_samples

		while remaining_samples > 0:
			read_size = min(block_size, remaining_samples)
			buffer = ser.read(read_size * 2)
			wf.writeframes(np.frombuffer(buffer, dtype=np.int16))
			remaining_samples -= read_size

	except KeyboardInterrupt:
		wf.close()
		ser.close()
	finally:
		wf.close()

ser.close()
