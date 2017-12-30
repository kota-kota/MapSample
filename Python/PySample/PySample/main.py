'''
main.py
'''

import numpy as np
import matplotlib.pyplot as plt

import mywave


#テスト1
def test1():
	#正弦波220Hz
	SIN0 = mywave.make_sin(1, 220)
	#正弦波440Hz
	SIN1 = mywave.make_sin(1, 440)
	#合成波
	FREQLIST = [220, 440, 660]
	COMB = mywave.make_combine(1, FREQLIST)

	#プロット
	FIG = plt.figure()

	AX0 = FIG.add_subplot(311)
	AX0.plot(SIN0[0:100])

	AX1 = FIG.add_subplot(312)
	AX1.plot(SIN1[0:100])

	AX2 = FIG.add_subplot(313)
	AX2.plot(COMB[0:100])

	plt.show()

#テスト2
def test2():
	FILENAME = "haruka_kanata.wav"

	print("---------------------------------")
	print("- Read ", FILENAME)
	print("---------------------------------")
	wavobj = mywave.read_wav_file(FILENAME)
	print(vars(wavobj))

	print("---------------------------------")
	print("- Start FFT")
	print("---------------------------------")
	spec = mywave.make_spec(wavobj)

	#Plot
	xtime = np.arange(0, wavobj.totaltime, 1 / wavobj.framerate)
	fig = plt.figure()
	ax1 = fig.add_subplot(2,1,1)
	ax1.plot(xtime, wavobj.buf)
	ax1.set_title(FILENAME)
	ax1.set_xlabel("time")
	ax1.set_ylabel("amplitude")
	
	ax2 = fig.add_subplot(2,1,2)
	ax2.imshow(spec.T, extent=[0, wavobj.totaltime, 0, wavobj.framerate / 2], cmap="jet", aspect="auto")
	
	plt.show()

#メイン処理
def main():
	#test1()
	test2()



#メイン
if __name__ == '__main__':
	main()
