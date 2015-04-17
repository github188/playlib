package neo.droid.cloudsee.commons;

import java.io.File;
import java.io.FileInputStream;
import java.io.FileOutputStream;
import java.nio.ByteBuffer;
import java.nio.ByteOrder;
import java.util.concurrent.LinkedBlockingQueue;

import neo.droid.cloudsee.Consts;
import neo.droid.cloudsee.IHandlerLikeNotify;
import android.media.AudioFormat;
import android.media.AudioManager;
import android.media.AudioRecord;
import android.media.AudioTrack;
import android.media.MediaRecorder;

/**
 * 音频帮助类
 * 
 * @author neo
 */
public class MyAudio {

	private static final String TAG = "MyAudio";

	private static final int SOURCE = MediaRecorder.AudioSource.MIC;
	private static final int CHANNEL = AudioFormat.CHANNEL_CONFIGURATION_MONO;
	private static final int STREAM_TYPE = AudioManager.STREAM_MUSIC;
	private static final int TRACK_MODE = AudioTrack.MODE_STREAM;

	private static final int TRACK_STEP = 320;
	private static final int DEFAULT_SAMPLERATE = 8000;
	private static final int STANDARD_SAMPLERATE = 48000;

	private static final String TARGET_FILE = Consts.LOG_PATH + File.separator
			+ "tree.wav";

	public static final int ARG1_PLAY = 0x01;
	public static final int ARG1_RECORD = 0x02;

	public static final int ARG2_START = 0x01;
	public static final int ARG2_FINISH = 0x02;
	public static final int ARG2_VOICE_FIN = 0x03;

	private LinkedBlockingQueue<byte[]> queue;

	private int what;
	private IHandlerLikeNotify notify;

	private Play play;
	private Record record;
	private int sampleRate;
	private float volume;

	private boolean isPlaying;
	private boolean isRecording;

	private MyAudio() {
		queue = new LinkedBlockingQueue<byte[]>();

		volume = 1.0f;
		isPlaying = false;
		isRecording = false;
		sampleRate = DEFAULT_SAMPLERATE;
	}

	private static class Container {
		private static MyAudio HOLDER = new MyAudio();
	}

	public static MyAudio getIntance(int what, IHandlerLikeNotify notify) {
		Container.HOLDER.what = what;
		Container.HOLDER.notify = notify;
		return Container.HOLDER;
	}

	public void put(byte[] data) {
		if (null != data) {
			queue.offer(data);
		}
	}

	public void startPlay(int bit, boolean isDefaultRate, boolean isFromQueue) {
		stopPlay();
		queue.clear();
		sampleRate = isDefaultRate ? DEFAULT_SAMPLERATE : STANDARD_SAMPLERATE;
		play = new Play(bit, isFromQueue);
		play.start();
	}

	public void stopPlay() {
		if (null != play) {
			play.interrupt();
			queue.offer(new byte[0]);
			play = null;
		}
	}

	public void startRec(int type, int bit, int block, boolean isSend) {
		stopRec();
		// sampleRate = isDefaultRate ? DEFAULT_SAMPLERATE :
		// STANDARD_SAMPLERATE;
		sampleRate = DEFAULT_SAMPLERATE;
		record = new Record(type, bit, block, isSend);
		record.start();
	}

	public void stopRec() {
		if (null != record) {
			record.interrupt();
			record = null;
		}
	}

	public synchronized boolean isPlaying() {
		return isPlaying;
	}

	private synchronized void setPlaying(boolean isPlaying) {
		this.isPlaying = isPlaying;
	}

	public synchronized boolean isRecording() {
		return isRecording;
	}

	private synchronized void setRecording(boolean isRecording) {
		this.isRecording = isRecording;
	}

	public synchronized float getVolume() {
		return volume;
	}

	public synchronized void setVolume(float volume) {
		this.volume = volume;
	}

	private class Play extends Thread {

		private int format;
		private boolean isFromQueue;

		public Play(int bit, boolean isFromQueue) {
			format = (16 == bit) ? AudioFormat.ENCODING_PCM_16BIT
					: AudioFormat.ENCODING_PCM_8BIT;
			this.isFromQueue = isFromQueue;
		}

		@Override
		public void run() {
			int minSize = AudioTrack.getMinBufferSize(sampleRate, CHANNEL,
					format);

			MyLog.w(TAG, "Play E: fromQueue = " + isFromQueue);
			setPlaying(true);

			if (null != notify) {
				notify.onNotify(what, ARG1_PLAY, ARG2_START, null);
			}

			if (minSize > 128) {
				AudioTrack track = new AudioTrack(STREAM_TYPE, sampleRate,
						CHANNEL, format, minSize, TRACK_MODE);

				int length = 0;
				int offset = 0;
				int left = 0;

				byte[] data = null;
				FileInputStream inputStream = null;

				if (false == isFromQueue) {
					try {
						inputStream = new FileInputStream(new File(TARGET_FILE));
						left = TRACK_STEP;
						data = new byte[TRACK_STEP];
					} catch (Exception e) {
						e.printStackTrace();
					}
				}

				if (AudioTrack.STATE_INITIALIZED == track.getState()) {
					track.play();

					while (false == isInterrupted()) {
						try {
							if (false == isFromQueue) {
								if (-1 != (length = inputStream.read(data,
										offset, left))) {
									if (offset + length < TRACK_STEP) {
										offset += length;
										left = TRACK_STEP - offset;
										continue;
									}
								} else {
									break;
								}
							} else {
								data = queue.take();
							}
						} catch (Exception e) {
							e.printStackTrace();
							break;
						}

						if (null != data) {
							if (3 == data.length && 'F' == data[0]
									&& 'i' == data[1] && 'n' == data[2]) {
								if (null != notify) {
									notify.onNotify(what, ARG1_PLAY,
											ARG2_VOICE_FIN, null);
								}
								continue;
							}

							// offset = 0;
							// left = data.length;
							//
							// while (left > 0) {
							// length = (left < TRACK_STEP) ? left
							// : TRACK_STEP;
							// track.write(data, offset, length);
							//
							// left -= TRACK_STEP;
							// offset += TRACK_STEP;
							// }

							adjustVolume(data, getVolume());
							track.write(data, 0, left);
						}

						length = 0;
						offset = 0;
						left = 0;
					}

					track.stop();
					track.release();

				}

				track = null;

				if (null != inputStream) {
					try {
						inputStream.close();
						inputStream = null;
					} catch (Exception e) {
						e.printStackTrace();
					}
				}

			}

			if (null != notify) {
				notify.onNotify(what, ARG1_PLAY, ARG2_FINISH, null);
			}

			setPlaying(false);
			MyLog.w(TAG, "Play X");
		}
	}

	private class Record extends Thread {

		private int type;
		private int bit;
		private int block;
		private boolean isSend;

		public Record(int type, int bit, int block, boolean isSend) {
			this.type = type;
			this.bit = bit;
			this.block = block;
			this.isSend = isSend;
		}

		@Override
		public void run() {
			int minSize = AudioRecord.getMinBufferSize(sampleRate, CHANNEL,
					AudioFormat.ENCODING_PCM_16BIT);

			MyLog.w(TAG, "Record E: type = " + type + ", bit = " + bit
					+ ", block = " + block + ", send = " + isSend);
			setRecording(true);

			if (null != notify) {
				notify.onNotify(what, ARG1_RECORD, ARG2_START, null);
			}

			if (minSize > 128) {
				AudioRecord rec = new AudioRecord(SOURCE, sampleRate, CHANNEL,
						AudioFormat.ENCODING_PCM_16BIT, minSize);

				if (AudioRecord.STATE_INITIALIZED == rec.getState()) {
					int count = 0;
					int length = 0;

					int offset = 0;
					int left = block;

					byte[] data = new byte[block];
					byte[] out = new byte[block / 2];

					ByteBuffer buffer = ByteBuffer.wrap(out).order(
							ByteOrder.LITTLE_ENDIAN);

					if (isSend && Consts.JAE_ENCODER_G729 == type) {
						for (int i = 0; i < 16; i++) {
							out[i] = 0;
						}
					}

					if (type >= 0) {
						// Jni.initAudioEncoder(type, sampleRate, 1, 16, block);
					}

					FileOutputStream outputStream = null;

					if (false == isSend) {
						try {
							File file = new File(TARGET_FILE);
							outputStream = new FileOutputStream(file);
						} catch (Exception e) {
							e.printStackTrace();
						}
					}

					rec.startRecording();

					while (false == isInterrupted()) {
						length = rec.read(data, offset, left);

						if (0 == left) {
							offset = 0;
							left = block;

							byte[] encoded = null;

							if (type >= 0) {
								// encoded = Jni.encodeAudio(data);
							} else {
								if (8 == bit) {
									// [Neo] TODO 16 to 8
									for (int i = 0; i < out.length; i++) {
										out[i] = (byte) (data[i * 2 + 1] - 0x80);
									}
									encoded = out;
								} else {
									encoded = data;
								}
							}

							if (isSend) {
								if (Consts.JAE_ENCODER_G729 == type) {
									buffer.position(0);
									buffer.putInt(count++);
									buffer.position(16);
									buffer.put(encoded, 0, 60);

									// Jni.sendBytes(0,
									// JVNetConst.JVN_RSP_CHATDATA, out,
									// 76);
								} else {
									// Jni.sendBytes(0,
									// JVNetConst.JVN_RSP_CHATDATA,
									// encoded, encoded.length);
								}
							} else {
								if (null != outputStream) {
									try {
										outputStream.write(encoded, 0,
												encoded.length);
									} catch (Exception e) {
										e.printStackTrace();
									}
								}
							}

						} else {
							left -= length;
							offset += length;
						}

					}

					rec.stop();
					rec.release();

					if (null != outputStream) {
						try {
							outputStream.flush();
							outputStream.close();
							outputStream = null;
						} catch (Exception e) {
							e.printStackTrace();
						}
					}

					if (type >= 0) {
						// Jni.deinitAudioEncoder();
					}

				}

				rec = null;

			}

			if (null != notify) {
				notify.onNotify(what, ARG1_RECORD, ARG2_FINISH, null);
			}

			setRecording(false);
			MyLog.w(TAG, "Record X");
		}
	}

	// [Neo] TODO six louder
	private static void adjustVolume(byte[] data, float volume) {
		if (volume > 1.01f || volume < 0.9f) {
			int size = data.length;
			for (int i = 0; i < size; i += 2) {
				short origin = (short) ((data[i + 1] << 8) + (data[i] & 0xFF));

				if (origin > 0) {
					origin = (short) (origin * volume + 1);
					if (origin < 0) {
						origin = (short) 0xFF7F;
					}
				} else if (origin < 0) {
					origin = (short) ((origin - 0) * volume - 1);
					if (origin > 0) {
						origin = (short) 0x0080;
					}
				}

				data[i] = (byte) (origin & 0xFF);
				data[i + 1] = (byte) ((origin >> 8) & 0xFF);
			}
		}
	}

}
