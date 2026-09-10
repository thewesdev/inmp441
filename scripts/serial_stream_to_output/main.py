import serial
import numpy as np
import sounddevice as sd
import queue

PORT = '/dev/ttyUSB0'
BAUDRATE = 921600
BLOCK_SIZE = 512
SAMPLE_RATE = 16000

q = queue.Queue()
ser = serial.Serial(port=PORT, baudrate=BAUDRATE, timeout=1)

def callback(outdata, frames, time, status):
	if status:
		print(status)

	try:
		data = q.get_nowait()
	except queue.Empty:
		data = np.zeros(frames, np.int16)

	outdata[:, 0] = data

with sd.OutputStream(samplerate=SAMPLE_RATE, channels=1, blocksize=BLOCK_SIZE, dtype='int16', callback=callback):
	try:
		while True:
			buffer = ser.read(BLOCK_SIZE * 2)
			samples = np.frombuffer(buffer, dtype=np.int16)
			q.put_nowait(samples)
	except KeyboardInterrupt:
		ser.close()
