import struct
import wave


with wave.open("test.wav", "rb") as obj:
  num_samples = obj.getnframes()
  data = []

  for _ in range(num_samples):
    sample = struct.unpack('<1h', obj.readframes(1))
    data.append(int(127.5 + sample[0]/2**8))

print(data)

print(len(data))
