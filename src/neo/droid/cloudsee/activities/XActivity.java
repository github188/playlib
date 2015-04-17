package neo.droid.cloudsee.activities;

import neo.droid.cloudsee.Consts;
import neo.droid.cloudsee.commons.MICRecorder;
import neo.droid.cloudsee.commons.MyLog;
import android.view.View;
import android.view.ViewGroup;
import android.widget.Button;
import android.widget.LinearLayout;

public class XActivity extends BaseActivity implements View.OnClickListener {

	// Jni.screenshot(0, Consts.LOG_PATH + File.separator + "jpg.jpg", 80);

	// new DummyOutputer().start();

	// Jni.setViewPort(0, 0, 0, 1, 1);

	// 获取主控AP信息请求
	// Jni.sendTextData(0, (byte) JVNetConst.JVN_RSP_TEXTDATA, 8,
	// JVNetConst.JVN_WIFI_INFO);

	// Jni.setOmx(0, true);
	// recorder.stop();
	// Jni.sendBytes(0, JVNetConst.JVN_CMD_CHATSTOP, new byte[0],
	// 8);

	// Jni.screenshot(0, "/sdcard/xx.png", 80);

	// isPlayAudioIndeed = !isPlayAudioIndeed;
	// MyLog.e(Consts.TAG_PLAY, "play audio = " + isPlayAudioIndeed);

	// Jni.enablePlayAudio(0, false);
	// Jni.sendBytes(0, (byte) JVNetConst.JVN_REQ_CHAT, new byte[0], 8);

	// Jni.setViewPort(0, 100, 100, 100, 200);

	// Jni.queryDevice("S", 53530352, 2000);

	// 获取主控码流信息请求
	// Jni.sendTextData(0, JVNetConst.JVN_RSP_TEXTDATA, 8,
	// JVNetConst.JVN_STREAM_INFO);

	// Jni.setOmx(0, false);
	// recorder.start(Consts.JAE_ENCODER_ALAW);

	// changeWindow(9);

	// Jni.setColor(0, 1, 0, 0.3f, 0);
	// Jni.fastForward(0);

	// Jni.sendBytes(0, JVNetConst.JVN_CMD_CHATSTOP, new byte[0], 8);

	// adapter.update(manager.genPageList(4));
	// pager.setAdapter(adapter);

	// Jni.stopRecord();

	// isPlayAudioIndeed = true;

	// Jni.setViewPort(0, 100, 100, 200, 100);

	// Jni.setOmx(0, true);

	// Jni.searchLanServer(9400, 6666);
	// Jni.searchLanDevice("", 0, 0, 0, "", 2000, 100);

	// pager.setDisableSliding(false);

	// Channel channel = manager.getChannel(0);
	// channel.setSurfaceView(surfaceView);
	// Jni.resume(windowIndex, surfaceView.getHolder().getSurface());
	//
	// // String target = "F:\\JdvrFile\\20141010\\00013100.mp4";
	// // String target = "./rec/00/20141010/N01191558.mp4";
	// String target = "./rec/00/20141020/A01100427.mp4";
	//
	// Jni.enablePlayback(windowIndex, true);
	// Jni.sendBytes(windowIndex, JVNetConst.JVN_REQ_PLAY,
	// target.getBytes(),
	// target.length());

	// isPlayAudioIndeed = false;

	// String target = "./rec/00/20141017/A01183434.mp4";
	// // String target = "./rec/00/20141017/A01205359.mp4";
	// byte[] bytes = target.getBytes();
	// Jni.sendBytes(0, (byte) Consts.DOWNLOAD_REQUEST, bytes,
	// bytes.length);

	// changeWindow(4);

	// Jni.setColor(0, 1, 0.3f, 0, 0);
	// Jni.enableLog(true);
	// Jni.setOmx(0, true);

	// Jni.sendBytes(0, JVNetConst.JVN_CMD_VIDEO, new byte[0], 8);
	// Jni.sendBytes(0, JVNetConst.JVN_REQ_CHAT, new byte[0], 8);

	// adapter.update(manager.genPageList(1));
	// pager.setAdapter(adapter);

	// Jni.startRecord(0, Consts.LOG_PATH + File.separator + "tmp.mp4",
	// true,
	// true);

	// isPlayAudioIndeed = false;
	// Jni.setViewPort(0, 0, 0, 100, 100);

	// Jni.setOmx(0, false);

	// Jni.init(getApplication(), 9200, Consts.LOG_PATH);

	// pager.setDisableSliding(true);

	// Jni.sendBytes(0, (byte) JVNetConst.JVN_REQ_TEXT, new byte[0], 8);

	// Jni.sendBytes(0, JVNetConst.JVN_CMD_VIDEOPAUSE, new byte[0], 8);

	// Jni.enablePlayAudio(0, false);
	// isPlayAudioIndeed = true;

	// Jni.sendCmd(0, JVNetConst.JVN_CMD_ONLYI, new byte[0], 0);
	// Jni.sendBytes(0, (byte) Consts.DOWNLOAD_STOP, new byte[0], 8);

	// Channel channel = manager.getChannel(0);
	// surfaceView = channel.getSurfaceView();
	// channel.setSurfaceView(null);
	// windowIndex = 10;
	// channel.setIndex(windowIndex);

	// int size = channelList.size();
	// for (int i = 0; i < size; i++) {
	// MyLog.d(Consts.TAG_XX, channelList.get(i).toString());
	// }

	// Device device = deviceList.get(0);
	// Jni.queryDevice(device.getGid(), device.getNo(), 2000);

	// Jni.setColor(0, 0.8f, 0, 0, 0);
	// Jni.enableLog(false);

	// Jni.setOmx(0, false);

	// [Neo] record about
	// Jni.startRecord(0, Consts.LOG_PATH + File.separator + "tmp.mp4", true,
	// true);
	// Jni.stopRecord();

	// [Neo] status
	// String status = Jni.getAllDeviceStatus();
	// try {
	// JSONArray array = new JSONArray(status);
	// JSONObject object = null;
	//
	// int length = array.length();
	// for (int i = 0; i < length; i++) {
	// object = array.getJSONObject(i);
	// for (Device device : deviceList) {
	// if (device.getFullNo().equals(
	// object.getString("cno"))) {
	// device.setHelperEnabled(object
	// .getBoolean("enable"));
	// break;
	// }
	// }
	// }
	//
	// } catch (JSONException e) {
	// e.printStackTrace();
	// }

	// [Neo] search
	// Jni.searchLanServer(9400, 6666);
	// Jni.searchLanDevice("", 0, 0, 0, "", 2000, 30);
	// Jni.stopSearchLanServer();

	// [Neo] test for playback
	// manager.getView(0).setVisibility(View.VISIBLE);
	// new Thread() {
	//
	// @Override
	// public void run() {
	// try {
	// Thread.sleep(1000);
	// } catch (InterruptedException e) {
	// e.printStackTrace();
	// }
	//
	// Jni.resume(0, manager.getView(0).getHolder().getSurface());
	// byte[] lala = new byte[] { (byte) 0x46, (byte) 0x3A,
	// (byte) 0x5C, (byte) 0x4A, (byte) 0x64, (byte) 0x76,
	// (byte) 0x72, (byte) 0x46, (byte) 0x69, (byte) 0x6C,
	// (byte) 0x65, (byte) 0x5C, (byte) 0x32, (byte) 0x30,
	// (byte) 0x31, (byte) 0x34, (byte) 0x30, (byte) 0x37,
	// (byte) 0x31, (byte) 0x38, (byte) 0x5C, (byte) 0x30,
	// (byte) 0x31, (byte) 0x30, (byte) 0x31, (byte) 0x30,
	// (byte) 0x36, (byte) 0x30, (byte) 0x30, (byte) 0x2E,
	// (byte) 0x6D, (byte) 0x70, (byte) 0x34 };
	// Jni.sendData(0, (byte) 0x30, lala, lala.length);
	// Jni.enablePlayback(0, true);
	// }
	//
	// }.start();

	// Jni.enablePlayAudio(0, true);

	// Jni.foo(null);

	// int size = deviceList.size();
	// for (int i = 0; i < size; i++) {
	// Jni.screenshot(i);
	// }

	// Jni.enablePlayAudio(0, false);

	// Jni.sendBytes(0, (byte) JVNetConst.JVN_REQ_TEXT, new byte[0], 8);

	// Jni.sendBytes(0, (byte) JVNetConst.JVN_REQ_CHAT, new byte[0], 8);
	// Jni.enablePlayAudio(0, true);

	// Jni.sendTextData(0, (byte) JVNetConst.JVN_RSP_TEXTDATA, 8,
	// JVNetConst.JVN_STREAM_INFO);
	// Jni.enablePlayAudio(0, false);

	// Jni.changeStream(0, (byte) JVNetConst.JVN_RSP_TEXTDATA,
	// "MainStreamQos=1");

	// Jni.changeStream(0, (byte) JVNetConst.JVN_RSP_TEXTDATA, String.format(
	// // "[CH%d];width=%d;height=%d;nMBPH=%d;framerate=%d;rcMode=0;", 1, 1280,
	// // 720, 800, 15
	// "[CH%d];nMBPH=%d;framerate=%d;rcMode=0;", 1, 800, 15));

	// adapter.update(manager.genPageList(4));
	// pager.setAdapter(adapter);

	private Button startButton;
	private Button fooButton;
	private Button stopButton;
	private Button playButton;

	private MICRecorder recorder;

	@Override
	protected void initSettings() {
		// ((AudioManager) getSystemService(Context.AUDIO_SERVICE))
		// .setMode(AudioManager.MODE_IN_CALL);
		recorder = MICRecorder.getInstance();
	}

	@Override
	protected void initUi() {
		LinearLayout ll = new LinearLayout(this);
		ll.setOrientation(LinearLayout.VERTICAL);

		startButton = new Button(this);
		startButton.setText("Start");
		startButton.setOnClickListener(this);
		ll.addView(startButton, new LinearLayout.LayoutParams(
				ViewGroup.LayoutParams.WRAP_CONTENT,
				ViewGroup.LayoutParams.WRAP_CONTENT, 0));

		fooButton = new Button(this);
		fooButton.setText("Start and Play");
		fooButton.setOnClickListener(this);
		ll.addView(fooButton, new LinearLayout.LayoutParams(
				ViewGroup.LayoutParams.WRAP_CONTENT,
				ViewGroup.LayoutParams.WRAP_CONTENT, 0));

		stopButton = new Button(this);
		stopButton.setText("Stop");
		stopButton.setOnClickListener(this);
		ll.addView(stopButton, new LinearLayout.LayoutParams(
				ViewGroup.LayoutParams.WRAP_CONTENT,
				ViewGroup.LayoutParams.WRAP_CONTENT, 0));

		playButton = new Button(this);
		playButton.setText("Play");
		playButton.setOnClickListener(this);
		ll.addView(playButton, new LinearLayout.LayoutParams(
				ViewGroup.LayoutParams.WRAP_CONTENT,
				ViewGroup.LayoutParams.WRAP_CONTENT, 0));

		setContentView(ll);
	}

	@Override
	public void onClick(View v) {
		if (v == startButton) {
			// recorder.start(Consts.JAE_ENCODER_ALAW);
		} else if (v == fooButton) {
			recorder.foo();
		} else if (v == stopButton) {
			recorder.stop();
		} else if (v == playButton) {
			recorder.play();
		}
	}

	@Override
	protected void freeMe() {
		// ((AudioManager) getSystemService(Context.AUDIO_SERVICE))
		// .setMode(AudioManager.MODE_NORMAL);
	}

	@Override
	protected void saveSettings() {
	}

	@Override
	public void onNotify(int what, int arg1, int arg2, Object obj) {
		MyLog.d(Consts.TAG_UI, getClass().getName() + ".onNotify: " + what
				+ ", " + arg1 + ", " + arg2 + ", " + obj);
	}

	@Override
	public void onHandler(int what, int arg1, int arg2, Object obj) {
		MyLog.d(Consts.TAG_UI, getClass().getName() + ".onHandler: " + what
				+ ", " + arg1 + ", " + arg2 + ", " + obj);
	}

}
