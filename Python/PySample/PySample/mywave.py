'''
mywave.py
'''

import numpy as np
import wave


#MyWaveクラス
class MyWave():
	def __init__(self):
		self.channelnum = 0
		self.totaltime = 0
		self.samplewidth = 0
		self.framerate = 0
		self.framenum = 0
		self.buf = []


#正弦波作成
def make_sin(amp, freq, sec=3, samplerate=8000):
	wavbuf = []
	for i in np.arange(sec * samplerate):
		s_t = amp * np.sin(2.0 * np.pi * freq * i / samplerate)
		wavbuf.append(s_t)
	return wavbuf

#合成波作成
def make_combine(amp, freqlist, sec=3, samplerate=8000):
	amp /= len(freqlist)
	wavbuf = []
	for i in np.arange(sec * samplerate):
		s_t = 0
		for freq in freqlist:
			s_t += amp * np.sin(2.0 * np.pi * freq * i / samplerate)
		wavbuf.append(s_t)
	return wavbuf

#WAVファイル読み込み
def read_wav_file(filename):
	#WAVファイルオープン
	wavfile = wave.open(filename, "rb")

	#WAVヘッダ取得
	channelnum = wavfile.getnchannels()
	samplewidth = wavfile.getsampwidth()
	framerate = wavfile.getframerate()
	framenum = wavfile.getnframes()

	#WAVデータバッファ取得
	framebuf = wavfile.readframes(framenum)

	#WAVファイルクローズ
	wavfile.close()

	#WAVデータを変換
	if samplewidth == 2:
		wavbuf = np.frombuffer(framebuf, dtype='int16')
	elif samplewidth == 4:
		wavbuf = np.frombuffer(framebuf, dtype='int32')
	#else:
		#Not support
	
	#モノラル変換
	if channelnum == 2:
		wavbuf = (wavbuf[::channelnum] / 2) + (wavbuf[1::channelnum] / 2)

	wavobj = MyWave()
	wavobj.channelnum = channelnum
	wavobj.totaltime = framenum / framerate
	wavobj.samplewidth = samplewidth
	wavobj.framerate = framerate
	wavobj.framenum = framenum
	wavobj.buf = wavbuf

	return wavobj

#スペクトログラム作成
def make_spec(wavobj):
	#FFT settings
	NFFT = 1024
	OVERLAP = int(NFFT / 2)
	frametime = 1 / wavobj.framerate

	#Time list of each FFT frames
	sta = (NFFT / 2) * frametime
	stop = wavobj.totaltime
	step = (NFFT - OVERLAP) * frametime
	timelist = np.arange(sta, stop, step)
	
	#Spectrogram
	spec = np.zeros([len(timelist), int(NFFT / 2) + 1])
	
	#Window function
	window = np.hamming(NFFT)

	#FFT
	pos = 0
	for iFrame in range(len(timelist)):
		frame = wavobj.buf[pos:pos + NFFT]
		if len(frame) == NFFT:
			frame_win = window * frame
			fftdata = np.fft.fft(frame_win)
			fftdata = np.log(np.abs(fftdata) ** 2)
			for i in range(len(spec[iFrame])):
				spec[iFrame][-i-1] = fftdata[i]
			
			pos += (NFFT - OVERLAP)

	return spec