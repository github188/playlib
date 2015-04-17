package neo.droid.cloudsee.commons;

import java.io.File;
import java.io.FileInputStream;
import java.io.FileOutputStream;
import java.nio.ByteBuffer;
import java.nio.ByteOrder;

import neo.droid.cloudsee.Consts;
import neo.droid.cloudsee.JVNetConst;
import android.media.AudioFormat;
import android.media.AudioManager;
import android.media.AudioRecord;
import android.media.AudioTrack;
import android.media.MediaRecorder;

import com.jovision.Jni;

public class MICRecorder {

	private static final String TAG = "MICR";

	private static final int SOURCE = MediaRecorder.AudioSource.MIC;
	private static final int SAMPLERATE = 8000;
	private static final int CHANNEL = AudioFormat.CHANNEL_CONFIGURATION_MONO;
	private static final int ENCODING = AudioFormat.ENCODING_PCM_16BIT;
	private static final int ENCODE_SIZE = 640;

	private static final int STREAM_TYPE = AudioManager.STREAM_MUSIC;
	private static final int TRACK_MODE = AudioTrack.MODE_STREAM;
	private static final int DECODE_SIZE = 320;

	private static final String TARGET_FILE = Consts.LOG_PATH + File.separator
			+ "o.pcm";

	private int encodeSize;
	private int bufferSize;
	private boolean isWorking;

	private AudioTrack track;
	private AudioRecord record;

	private static MICRecorder RECORDER;

	private MICRecorder() {
		isWorking = false;
		bufferSize = AudioRecord
				.getMinBufferSize(SAMPLERATE, CHANNEL, ENCODING);
		encodeSize = ENCODE_SIZE;
		MyLog.i(TAG, "construction, size = " + bufferSize);
	}

	public static MICRecorder getInstance() {
		if (null == RECORDER) {
			RECORDER = new MICRecorder();
		}

		return RECORDER;
	}

	public int getEncodeSize() {
		return encodeSize;
	}

	public void setEncodeSize(int encodeSize) {
		this.encodeSize = encodeSize;
	}

	public static void halfVol(short[] in, int offset, int length) {
		int tmp = 0;

		length += offset;
		for (int i = offset; i < length; i++) {
			tmp = in[i];

			if (tmp > 16350) {
				in[i] = 16350 >> 1;
			} else if (tmp < -16350) {
				in[i] = -16350 >> 1;
			} else {
				in[i] = (short) (tmp >> 1);
			}
		}
	}

	public boolean start(final int type, final int block) {
		boolean result = false;

		if (false == isWorking && bufferSize > 128) {
			record = new AudioRecord(SOURCE, SAMPLERATE, CHANNEL, ENCODING,
					1024);

			if (null != record
					&& AudioRecord.STATE_INITIALIZED == record.getState()) {

				record.startRecording();

				result = true;
				isWorking = true;

				new Thread() {

					@Override
					public void run() {
						MyLog.w(TAG, "recording E");

						try {
							File file = new File(TARGET_FILE);
							boolean ready = true;
							if (file.exists()) {
								ready = file.delete();
								MyLog.w(TAG, "overide: " + ready);
							}

							if (ready) {
								FileOutputStream outputStream = new FileOutputStream(
										file);
								int ret = 0;
								byte[] buffer = new byte[block];
								byte[] out = null;

								MyLog.d(Consts.TAG_PLAY,
										"init audio encoder(type = "
												+ type
												+ ", block = "
												+ block
												+ ") = "
												+ Jni.initAudioEncoder(type,
														SAMPLERATE, 1, 16,
														block));

								int count = 0;
								byte[] g729 = new byte[76];
								for (int i = 0; i < 16; i++) {
									g729[i] = 0;
								}
								ByteBuffer g729Buffer = ByteBuffer.wrap(g729);
								g729Buffer.order(ByteOrder.LITTLE_ENDIAN);

								while (isWorking) {
									ret = record.read(buffer, 0, block);
									if (ret > 0) {
										if (ret == block) {

											out = Jni.encodeAudio(buffer);
											if (null != out) {
												g729Buffer.position(0);
												g729Buffer.putInt(count);
												g729Buffer.position(16);
												g729Buffer.put(out, 0, 60);
												count++;
												Jni.sendBytes(
														0,
														JVNetConst.JVN_RSP_CHATDATA,
														g729, g729.length);
											}

											// outputStream.write(buffer);

										} else {
											MyLog.e(Consts.TAG_APP,
													"record bad size: " + ret);
										}
									} else {
										MyLog.e(Consts.TAG_APP, "record bad: "
												+ ret);
									}
								}

								outputStream.close();
								MyLog.d(Consts.TAG_PLAY,
										"deinit audio encoder = "
												+ Jni.deinitAudioEncoder());

							}
						} catch (Exception e) {
							e.printStackTrace();
						}

						MyLog.w(TAG, "recording X");
					}

				}.start();
			} else {
				MyLog.w(TAG, "record init not ok");
			}
		}

		MyLog.i(TAG, "start with: " + result);
		return result;
	}

	public boolean stop() {
		boolean result = false;

		if (isWorking) {
			isWorking = false;
			if (null != record) {
				if (AudioRecord.STATE_INITIALIZED == record.getState()
						&& AudioRecord.RECORDSTATE_RECORDING == record
								.getRecordingState()) {
					record.stop();
				}
				record.release();
				record = null;
				result = true;
			}
		}

		MyLog.i(TAG, "stop with: " + result);
		return result;
	}

	public boolean play() {
		boolean result = false;
		final File file = new File(TARGET_FILE);

		if (null != file && file.exists()) {

			new Thread() {

				@Override
				public void run() {
					try {
						byte[] buffer = new byte[ENCODE_SIZE];
						FileInputStream inputStream = new FileInputStream(file);
						if (null == track) {
							track = new AudioTrack(STREAM_TYPE, SAMPLERATE,
									CHANNEL, ENCODING, 1024, TRACK_MODE);
						}

						if (null != track) {
							track.play();

							int offset = 0;
							int length = ENCODE_SIZE;
							int count = 0;

							byte[] out = null;
							// Jni.initAudioEncoder(Consts.JAE_ENCODER_ALAW,
							// SAMPLERATE, 1, 16, ENCODE_SIZE);
							// out = Jni.encodeAudio(buffer);
							// Jni.deinitAudioEncoder();

							while (-1 != (count = inputStream.read(buffer,
									offset, length))) {
								if (offset + count < ENCODE_SIZE) {
									offset += count;
									length = ENCODE_SIZE - offset;
									continue;
								}

								out = buffer;
								if (null != out) {
									track.write(out, 0, out.length);
								} else {
									MyLog.e(Consts.TAG_APP, "play with null");
								}
							}

							inputStream.close();
							track.stop();
							track.release();
							track = null;
						}
					} catch (Exception e) {
					}

					MyLog.i(TAG, "play end");
				}

			}.start();

			result = true;
		} else {
			MyLog.i(TAG, "play without file: " + result);
		}

		return result;
	}

	public boolean foo() {
		boolean result = false;

		if (false == isWorking && bufferSize > 128) {
			record = new AudioRecord(SOURCE, SAMPLERATE, CHANNEL, ENCODING,
					bufferSize);

			if (null != record
					&& AudioRecord.STATE_INITIALIZED == record.getState()) {

				result = true;
				isWorking = true;

				new Thread() {

					@Override
					public void run() {
						MyLog.w(TAG, "foo E");

						record.startRecording();

						try {
							File file = new File(TARGET_FILE);
							boolean ready = true;
							if (file.exists()) {
								ready = file.delete();
								MyLog.w(TAG, "overide: " + ready);
							}

							if (ready) {
								FileOutputStream outputStream = new FileOutputStream(
										file);
								byte[] buffer = new byte[encodeSize];
								int ret = 0;

								FileInputStream inputStream = new FileInputStream(
										file);
								if (null == track) {
									track = new AudioTrack(STREAM_TYPE,
											SAMPLERATE, CHANNEL, ENCODING,
											bufferSize * 2, TRACK_MODE);
								}

								track.play();

								while (isWorking) {
									ret = record.read(buffer, 0, encodeSize);
									if (AudioRecord.ERROR_INVALID_OPERATION != ret) {
										outputStream.write(buffer);
										track.write(buffer, 0, encodeSize);
									}
								}
								outputStream.close();

								inputStream.close();
								track.stop();
								track.release();
								track = null;
							}
						} catch (Exception e) {
						}

						MyLog.w(TAG, "foo X");
					}

				}.start();
			} else {
				MyLog.w(TAG, "record init not ok");
			}
		}

		MyLog.i(TAG, "foo with: " + result);
		return result;
	}

}
