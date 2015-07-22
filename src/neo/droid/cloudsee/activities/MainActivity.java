package neo.droid.cloudsee.activities;

import java.io.File;
import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.IOException;
import java.util.ArrayList;
import java.util.Locale;
import java.util.Timer;
import java.util.TimerTask;

import neo.droid.cloudsee.Consts;
import neo.droid.cloudsee.JVNetConst;
import neo.droid.cloudsee.R;
import neo.droid.cloudsee.beans.Channel;
import neo.droid.cloudsee.beans.Device;
import neo.droid.cloudsee.commons.MyAudio;
import neo.droid.cloudsee.commons.MyGestureDispatcher;
import neo.droid.cloudsee.commons.MyLog;
import neo.droid.cloudsee.commons.MyViewPager;
import neo.droid.cloudsee.commons.PlayWindowManager;

import org.json.JSONArray;
import org.json.JSONException;
import org.json.JSONObject;

import android.content.res.Configuration;
import android.graphics.Color;
import android.graphics.Point;
import android.os.Environment;
import android.support.v4.view.PagerAdapter;
import android.support.v4.view.ViewPager;
import android.text.TextUtils;
import android.view.Surface;
import android.view.View;
import android.view.ViewGroup;
import android.widget.TextView;
import android.widget.Toast;

import com.jovision.Jni;

public class MainActivity extends BaseActivity implements
		PlayWindowManager.OnUiListener {

	private static final int WHAT_CHECK_SURFACE = 0x20;
	private static final int WHAT_CHANGE_INFO = 0x21;
	private static final int WHAT_RESTORE_UI = 0x22;

	private static final int WHAT_AUDIO = 0x30;
	private static final int SAMPLE_RATE_DEVICE = 8000;
	private static final int SAMPLE_RATE_CONFIG = 48000;

	private static final int WHAT_AUDIO_FOO = 0x31;

	private static final int DELAY_CHECK_SURFACE = 500;
	private static final int DELAY_DOUBLE_CHECKER = 500;
	private static final int CONNECTION_MIN_PEROID = 200;
	private static final int DISCONNECTION_MIN_PEROID = 50;
	private static final int RESUME_VIDEO_MIN_PEROID = 50;

	private static final int ARG2_INFO_INVISIBLE = 0x00;
	private static final int ARG2_INFO_CONNECTING = 0x01;
	private static final int ARG2_INFO_CONNECTED = 0x02;
	private static final int ARG2_INFO_BUFFERING = 0x03;
	private static final int ARG2_INFO_RESUMING = 0x04;
	private static final int ARG2_INFO_DISCONNECTED = 0x05;
	private static final int ARG2_INFO_OTHER = 0x06;

	private static final int ARG2_INFO_SURFACE_CREATED = 0x10;
	private static final int ARG2_INFO_SURFACE_DESTORYED = 0x11;
	private static final int ARG2_INFO_SURFACE_TODO = 0x12;

	private static final int BORDER_SELECT_COLOR = Color.RED;
	private static final int BORDER_UNSELECT_COLOR = Color.BLUE;

	public static final String THUMB_FILE_NAME = Consts.LOG_PATH + "/t.jpg";

	static {
		System.loadLibrary("alu");
	    System.loadLibrary("nplayer");
		System.loadLibrary("play");
	}

	private boolean isQuit;
	private boolean tryOmx;
	private boolean isTcp;
	private boolean isBlockUi;

	private int screenCount;
	private int lastItemIndex;
	private int lastClickIndex;

	private Timer doubleClickTimer;
	private boolean isDoubleClickCheck;

	private ArrayList<Device> deviceList;
	private ArrayList<Channel> channelList;

	private PlayWindowManager manager;
	private ArrayList<Channel> currentPageChannelList;

	private ArrayList<Channel> connectChannelList;
	private ArrayList<Channel> disconnectChannelList;

	private MyViewPager pager;
	private MyPagerAdapter adapter;

	private TextView logView;

	private TextView additional;

	private float fps;
	private float frameDecodeSum;
	private float frameNetworkSum;
	private long startTimeStamp;

	private float gain;

	private int targetIndex;
	private String audioLog;

	private int audio_count;

	private boolean enableYST;

	private int pointer = 0;
	private static String urls[] = { "rtmp://119.188.172.3/live/a367_1",
			"rtmp://119.188.172.3/live/a368_1",
			"rtmp://119.188.172.3/live/a361_1",
			"rtmp://119.188.172.3:1935/live/a579223323_1",
			"rtmp://119.188.172.3/live/a366_1",
			"rtmp://119.188.172.3:1935/live/ipcstory_1", };

	@Override
	protected void initSettings() {
		gain = 1.0f;

		screenCount = 1;
		targetIndex = 0;
		audio_count = 3;

		isQuit = false;
		isBlockUi = false;

		isTcp = false;
		tryOmx = false;
		enableYST = true;

		deviceList = new ArrayList<Device>();

		channelList = new ArrayList<Channel>();
		connectChannelList = new ArrayList<Channel>();
		disconnectChannelList = new ArrayList<Channel>();

		// deviceList.add(new Device(Consts.IPC_DEFAULT_IP, 9101, "S", -1,
		// Consts.IPC_DEFAULT_USER, Consts.IPC_DEFAULT_PWD, true, 1));

		// deviceList.add(new Device("172.17.1.204", 9101, "A", -1, "admin",
		// "qaz", false, 1));
		// deviceList.add(new Device("172.17.4.254", 9101, "A", -1, "admin",
		// "q",
		// false, 1));
		// deviceList.add(new Device("172.17.1.41", 9101, "B", 96246397,
		// "admin",
		// "q", true, 1));
		// deviceList.add(new Device("172.16.27.236", 9101, "A", 230283235,
		// "admin", "1", false, 1));
		// deviceList.add(new Device("172.16.27.236", 9101, "A", 362, "abc",
		// "123", false, 1));
		// deviceList.add(new Device("172.16.27.236", 9101, "A", 30121922,
		// "abc",
		// "123", false, 1));
		deviceList.add(new Device("172.16.27.236", 9101, "A", 230283235, "abc",
				"123", false, 1));

		for (Device device : deviceList) {
			channelList.addAll(device.getChannelList());
		}

		try {
			String ver = Jni.getVersion();
			JSONObject object = new JSONObject(ver);
			MyLog.i(Consts.TAG_APP, "jni: " + object.getString("jni"));
			MyLog.i(Consts.TAG_APP, "net: " + object.getString("net"));
		} catch (Exception e) {
			// [Neo] Empty
		}

		Jni.init(getApplication(), 9200, Consts.LOG_PATH);
		// [Neo] TODO
		Jni.enableLog(true);
		Jni.enableLinkHelper(true, 3, 10);
		Jni.setLinkHelper(genLinkHelperJson());
		Jni.setStat(true);

		// deviceList.get(0).setNo(-1);

		manager = PlayWindowManager.getIntance(this);
		int size = channelList.size();
		for (int i = 0; i < size; i++) {
			channelList.get(i).setUrl(urls[i % urls.length]);
			manager.addChannel(channelList.get(i));
		}

		lastItemIndex = 0;
		lastClickIndex = 0;
		isDoubleClickCheck = false;

	}

	@Override
	protected void initUi() {
		setContentView(R.layout.activity_main);
		logView = (TextView) findViewById(R.id.log);
		logView.setVisibility(View.VISIBLE);
		additional = (TextView) findViewById(R.id.additional);
		additional.setVisibility(View.VISIBLE);

		pager = (MyViewPager) findViewById(R.id.pager);
		adapter = new MyPagerAdapter();
		manager.setBorderColor(BORDER_UNSELECT_COLOR);
		changeWindow();

		pager.setOnPageChangeListener(new ViewPager.OnPageChangeListener() {

			@Override
			public void onPageSelected(int arg0) {
				MyLog.i(Consts.TAG_UI, "<- pageSelected: " + arg0 + ", to "
						+ ((arg0 > lastItemIndex) ? "right" : "left"));

				currentPageChannelList = manager.getValidChannelList(arg0);
				int size = currentPageChannelList.size();

				// [Neo] select the first one border as default
				int target = currentPageChannelList.get(0).getIndex();
				for (int i = 0; i < size; i++) {
					if (lastClickIndex == currentPageChannelList.get(i)
							.getIndex()) {
						target = lastClickIndex;
						break;
					}
				}
				changeBorder(target);

				if (false == isBlockUi) {
					if (1 == screenCount) {
						try {
							pauseChannel(channelList.get(arg0 - 1));
						} catch (Exception e) {
							// [Neo] empty
						}

						try {
							pauseChannel(channelList.get(arg0 + 1));
						} catch (Exception e) {
							// [Neo] empty
						}
					} else {
						disconnectChannelList.addAll(manager
								.getValidChannelList(lastItemIndex));
					}

					isBlockUi = true;
					pager.setDisableSliding(isBlockUi);

					handler.removeMessages(WHAT_CHECK_SURFACE);
					handler.sendMessageDelayed(handler.obtainMessage(
							WHAT_CHECK_SURFACE, arg0, lastClickIndex),
							DELAY_CHECK_SURFACE);
				}

				lastItemIndex = arg0;
			}

			@Override
			public void onPageScrolled(int arg0, float arg1, int arg2) {
				// [Neo] Empty
			}

			@Override
			public void onPageScrollStateChanged(int arg0) {
				// [Neo] Empty
			}
		});

	}

	@Override
	public void onClick(Channel channel, boolean isFromImageView, int viewId) {
		MyLog.i(Consts.TAG_UI, "onClick: " + channel.getIndex());

		if (false == isBlockUi && isDoubleClickCheck
				&& lastClickIndex == channel.getIndex()) {

			if (1 != screenCount) {
				int size = currentPageChannelList.size();
				for (int i = 0; i < size; i++) {
					if (lastClickIndex - 1 > i && lastClickIndex + 1 < i) {
						disconnectChannelList
								.add(currentPageChannelList.get(i));
					} else if (lastClickIndex == i) {
						// [Neo] Empty
					} else {
						// [Neo] stand alone for single destroy window, too
						pauseChannel(currentPageChannelList.get(i));
					}
				}

				screenCount = 1;
			} else {
				screenCount = 4;
			}

			changeWindow();

		} else {

			changeBorder(channel.getIndex());

			if (false == isBlockUi) {
				isDoubleClickCheck = true;
				if (null != doubleClickTimer) {
					doubleClickTimer.cancel();
				}

				doubleClickTimer = new Timer();
				doubleClickTimer.schedule(new DoubleClickChecker(),
						DELAY_DOUBLE_CHECKER);
			}

		}
	}

	@Override
	public void onLongClick(Channel channel) {
	}

	private void gestureOnView(View v, Channel channel, int gesture,
			int distance, Point vector, Point middle) {
		int viewWidth = v.getWidth();
		int viewHeight = v.getHeight();

		int left = channel.getLastPortLeft();
		int bottom = channel.getLastPortBottom();
		int width = channel.getLastPortWidth();
		int height = channel.getLastPortHeight();

		boolean needRedraw = false;

		switch (gesture) {
		case MyGestureDispatcher.GESTURE_TO_LEFT:
		case MyGestureDispatcher.GESTURE_TO_UP:
		case MyGestureDispatcher.GESTURE_TO_RIGHT:
		case MyGestureDispatcher.GESTURE_TO_DOWN:
			left += vector.x;
			bottom += vector.y;
			needRedraw = true;
			break;

		case MyGestureDispatcher.GESTURE_TO_BIGGER:
		case MyGestureDispatcher.GESTURE_TO_SMALLER:
			if (width > viewWidth || distance > 0) {
				float xFactor = (float) vector.x / viewWidth;
				float yFactor = (float) vector.y / viewHeight;
				float factor = yFactor;

				if (distance > 0) {
					if (xFactor > yFactor) {
						factor = xFactor;
					}
				} else {
					if (xFactor < yFactor) {
						factor = xFactor;
					}
				}

				int xMiddle = middle.x - left;
				int yMiddle = viewHeight - middle.y - bottom;

				factor += 1;
				left = middle.x - (int) (xMiddle * factor);
				bottom = (viewHeight - middle.y) - (int) (yMiddle * factor);
				width = (int) (width * factor);
				height = (int) (height * factor);

				if (width <= viewWidth || height < viewHeight) {
					left = 0;
					bottom = 0;
					width = viewWidth;
					height = viewHeight;
				} else if (width > 4000 || height > 4000) {
					width = channel.getLastPortWidth();
					height = channel.getLastPortHeight();

					if (width > height) {
						factor = 4000.0f / width;
						width = 4000;
						height = (int) (height * factor);
					} else {
						factor = 4000.0f / height;
						width = (int) (width * factor);
						height = 4000;
					}

					left = middle.x - (int) (xMiddle * factor);
					bottom = (viewHeight - middle.y) - (int) (yMiddle * factor);
				}

				needRedraw = true;
			}
			break;

		default:
			break;
		}

		if (needRedraw) {
			if (left + width < viewWidth) {
				left = viewWidth - width;
			} else if (left > 0) {
				left = 0;
			}

			if (bottom + height < viewHeight) {
				bottom = viewHeight - height;
			} else if (bottom > 0) {
				bottom = 0;
			}

			channel.setLastPortLeft(left);
			channel.setLastPortBottom(bottom);
			channel.setLastPortWidth(width);
			channel.setLastPortHeight(height);
			Jni.setViewPort(channel.getIndex(), left, bottom, width, height);
		}
	}

	private void fillView(Channel channel, View view) {
		int picWidth = channel.getWidth();
		int picHeight = channel.getHeight();
		int viewWidth = view.getWidth();
		int viewHeight = view.getHeight();

		int left = 0;
		int bottom = 0;
		int width = 0;
		int height = 0;

		// [Neo] picture is wider than view
		if (((float) picWidth / viewWidth) > ((float) picHeight / viewHeight)) {
			height = viewHeight;
			width = (int) ((float) viewHeight / picHeight * picWidth);
			left = (int) ((viewWidth - width) / 2.0f);
		} else {
			width = viewWidth;
			height = (int) ((float) viewWidth / picWidth * picHeight);
			bottom = (int) ((viewHeight - height) / 2.0f);
		}

		channel.setLastPortLeft(left);
		channel.setLastPortBottom(bottom);
		channel.setLastPortWidth(width);
		channel.setLastPortHeight(height);

		MyLog.d(Consts.TAG_XX, "fixed: (" + left + ", " + bottom + "), "
				+ width + "x" + height + "(" + picWidth + "x" + picHeight + ")"
				+ " on: " + viewWidth + "x" + viewHeight);

		Jni.setViewPort(channel.getIndex(), left, bottom, width, height);
	}

	private void adjustView(Channel channel, View view) {
		int picWidth = channel.getWidth();
		int picHeight = channel.getHeight();
		int viewWidth = view.getWidth();
		int viewHeight = view.getHeight();

		int left = 0;
		int bottom = 0;
		int width = 0;
		int height = 0;

		// [Neo] view is wider than picture
		if (((float) viewWidth / picWidth) > ((float) viewHeight / picHeight)) {
			height = viewHeight;
			width = (int) ((float) viewHeight / picHeight * picWidth);
			left = (int) ((viewWidth - width) / 2.0f);
		} else {
			width = viewWidth;
			height = (int) ((float) viewWidth / picWidth * picHeight);
			bottom = (int) ((viewHeight - height) / 2.0f);
		}

		channel.setLastPortLeft(left);
		channel.setLastPortBottom(bottom);
		channel.setLastPortWidth(width);
		channel.setLastPortHeight(height);
		Jni.setViewPort(channel.getIndex(), left, bottom, width, height);
	}

	private void tensileView(Channel channel, View view) {
		channel.setLastPortLeft(0);
		channel.setLastPortBottom(0);
		channel.setLastPortWidth(view.getWidth());
		channel.setLastPortHeight(view.getHeight());
		Jni.setViewPort(channel.getIndex(), 0, 0, view.getWidth(),
				view.getHeight());
	}

	private void tileView(Channel channel, View view) {
		channel.setLastPortLeft(0);
		channel.setLastPortBottom(view.getHeight() - channel.getHeight());
		channel.setLastPortWidth(channel.getWidth());
		channel.setLastPortHeight(channel.getHeight());
		Jni.setViewPort(channel.getIndex(), 0, channel.getLastPortBottom(),
				channel.getWidth(), channel.getHeight());
	}

	private void middleView(Channel channel, View view) {
		int picWidth = channel.getWidth();
		int picHeight = channel.getHeight();
		int viewWidth = view.getWidth();
		int viewHeight = view.getHeight();

		int left = (int) ((viewWidth - picWidth) / 2.0f);
		int bottom = (int) ((viewHeight - picHeight) / 2.0f);
		int width = picWidth;
		int height = picHeight;

		channel.setLastPortLeft(left);
		channel.setLastPortBottom(bottom);
		channel.setLastPortWidth(width);
		channel.setLastPortHeight(height);
		Jni.setViewPort(channel.getIndex(), left, bottom, width, height);
	}

	@Override
	public void onGesture(int index, int gesture, int distance, Point vector,
			Point middle) {
		gestureOnView(manager.getView(index), channelList.get(index), gesture,
				distance, vector, middle);
	}

	@Override
	public void onLifecycle(int index, int status, Surface surface, int width,
			int height) {

		boolean isFromCurrent = false;
		final Channel channel = channelList.get(index);

		if (1 == screenCount) {
			int size = currentPageChannelList.size();
			for (int i = 0; i < size; i++) {
				if (index == currentPageChannelList.get(i).getIndex()) {
					isFromCurrent = true;
					break;
				}
			}
		}

		switch (status) {
		case PlayWindowManager.STATUS_CREATED:
			if (1 == screenCount && false == isFromCurrent
					&& false == channel.isConnected()) {
				connectChannelList.add(channel);
			}

			channel.setSurface(surface);
			break;

		case PlayWindowManager.STATUS_CHANGED:
			tensileView(channel, channel.getSurfaceView());
			Jni.resume(index, surface);
			break;

		case PlayWindowManager.STATUS_DESTROYED:
			pauseChannel(channel);
			channel.setSurface(null);
			break;

		default:
			break;
		}

	}

	@Override
	protected void freeMe() {
		manager.destroy();
		adapter.notifyDataSetChanged();
	}

	@Override
	protected void saveSettings() {
		Jni.enableLog(false);
		Jni.deleteLog();
	}

	@Override
	public void onConfigurationChanged(Configuration newConfig) {
		switch (newConfig.orientation) {
		case Configuration.ORIENTATION_LANDSCAPE:
			// screenCount = 1;
			// changeWindow();
			break;

		case Configuration.ORIENTATION_PORTRAIT:
			break;

		default:
			break;
		}

		super.onConfigurationChanged(newConfig);
	}

	@Override
	public void onBackPressed() {
		isQuit = true;
		onDisconnect(null);
		Jni.setStat(false);

		super.onBackPressed();
	}

	@Override
	protected void onResume() {
		super.onResume();
		Jni.enablePlayAudio(targetIndex, true);
		handler.removeMessages(WHAT_CHECK_SURFACE);
		handler.sendMessageDelayed(handler.obtainMessage(WHAT_CHECK_SURFACE,
				lastItemIndex, lastClickIndex), DELAY_CHECK_SURFACE);
	}

	public void onF1(View view) {
		Jni.enablePlayAudio(targetIndex, true);
		Jni.resumeAudio(targetIndex);

	    
		// Jni.foo(null);

		// new Thread() {
		//
		// @Override
		// public void run() {
		// Jni.genVoice("test;dsfawefwefwe", 2);
		// }
		//
		// }.start();

		// Jni.startAudioRecord(16, 640);

		// Jni.setFrameCounts(targetIndex, 5, 20);
		// Jni.resume(targetIndex, channelList.get(targetIndex).getSurface());

		// pager.setDisableSliding(true);

		// Channel channel = channelList.get(targetIndex);
		// channel.setWidth(1280);
		// channel.setHeight(720);
		// fillView(channel, manager.getView(targetIndex));

		// Jni.foo(null);

		// new Thread() {
		//
		// @Override
		// public void run() {
		// Jni.genVoice("Netcore_YFONE;1234567890", 2);
		// }
		//
		// }.start();

		// audio_count = 3;
		// MyAudio.getIntance(WHAT_AUDIO, MainActivity.this, SAMPLE_RATE_CONFIG)
		// .startPlay(16, true);

		// Jni.enablePlayAudio(targetIndex, true);
		// MyAudio.getIntance(WHAT_AUDIO, this, SAMPLE_RATE_DEVICE).startPlay(
		// channelList.get(targetIndex).getAudioBitCount(), true);

		// Jni.startRecord(targetIndex,
		// Consts.LOG_PATH + File.separator + "x.mp4", true, true);

		// Jni.setOmx(targetIndex, true);

		// Jni.sendBytes(targetIndex, (byte) JVNetConst.JVN_REQ_TEXT, new
		// byte[0],
		// 8);

		// Channel channel = channelList.get(targetIndex);
		// if (Consts.JAE_ENCODER_G729 == channel.getAudioEncType()) {
		// MyAudio.getIntance(WHAT_AUDIO, this, SAMPLE_RATE_DEVICE).startPlay(
		// 16, true);
		// } else {
		// MyAudio.getIntance(WHAT_AUDIO, this, SAMPLE_RATE_DEVICE).startPlay(
		// channel.getAudioBitCount(), true);
		// }

		// screenCount = 1;
		// changeWindow();

		// Jni.setOmx(targetIndex, false);
		// Jni.setOmx(targetIndex, true);

		// String target = "";
		// byte[] targetArray = target.getBytes();
		//
		// Jni.enablePlayback(targetIndex, true);
		// Jni.sendBytes(targetIndex, JVNetConst.JVN_REQ_PLAY, targetArray,
		// targetArray.length);

		// MyAudio.getIntance(WHAT_AUDIO, this).startRec(-1, 16, 320, false);
	}

	public void onF2(View view) {
		Jni.enablePlayAudio(targetIndex, false);
		Jni.pauseAudio(targetIndex);

	    
		// Jni.setRecordVolume(5.0f);

		// Jni.startAudioRecord(16, 640);
		// Jni.setFrameCounts(targetIndex, 25, 25);

		Jni.enablePlayback(targetIndex, true);
		String target =
		// "E:\\JdvrFile\\20150115\\01023100.mp4";
		// "./rec/13/20141231/M01075155.sv5";
		// "./rec/02/20150202/N01155005.mp4";
		"./rec/00/20150420/N01000700.mp4";
		Jni.sendBytes(targetIndex, JVNetConst.JVN_REQ_PLAY, target.getBytes(),
				target.getBytes().length);

		// Jni.resume(targetIndex, channelList.get(targetIndex).getSurface());
		// Jni.setColor(targetIndex, 0, 0, 0, 0);

		// Jni.pause(targetIndex);

		// MyAudio.getIntance(WHAT_AUDIO, this).stopPlay();
		//
		// Channel channel = channelList.get(targetIndex);
		// MyAudio.getIntance(WHAT_AUDIO, this).setVolume(0.5f);
		// MyAudio.getIntance(WHAT_AUDIO, this).startRec(
		// channel.getAudioEncType(), channel.getAudioBitCount(),
		// channel.getAudioBlock(), true);

		// Channel channel = channelList.get(pointer);
		// MyLog.d(Consts.TAG_XX, "check: " + channel.getUrl() + ", pointer = "
		// + pointer);
		// boolean result = Jni
		// .connectRTMP(
		// pointer,
		// // "rtmp://192.168.10.22/live/test01",
		// // "rtmp://119.188.172.3/live/A368_1",
		// channel.getUrl(), channel.getSurface(), tryOmx,
		// THUMB_FILE_NAME);
		// if (result) {
		// channel.setPaused(false);
		// }

		// View v = manager.getView(targetIndex);
		// gestureOnView(v, channelList.get(targetIndex),
		// MyGestureDispatcher.GESTURE_TO_BIGGER, 1, new Point(0, 50),
		// new Point(v.getWidth(), v.getHeight()));

		// MyAudio.getIntance(WHAT_AUDIO, this, SAMPLE_RATE_DEVICE).stopPlay();

		// Jni.sendTextData(targetIndex, (byte) JVNetConst.JVN_RSP_TEXTDATA, 8,
		// JVNetConst.JVN_WIFI_INFO);

		// Jni.enablePlayAudio(targetIndex, false);
		// MyAudio.getIntance(WHAT_AUDIO, this, SAMPLE_RATE_DEVICE).stopPlay();

		// Jni.sendBytes(targetIndex, (byte) JVNetConst.JVN_REQ_TEXT, new
		// byte[0],
		// 8);

		// Jni.stopRecord();

		// Jni.setOmx(targetIndex, false);

		// MyAudio.getIntance().stopRec();
		// Jni.startRecord(0, "/sdcard/x.mp4", true, false);

		// screenCount = 4;
		// changeWindow();

		// Jni.sendBytes(targetIndex, JVNetConst.JVN_REQ_TEXT, new byte[0], 8);
		// Jni.sendTextData(targetIndex, JVNetConst.JVN_RSP_TEXTDATA, 8,
		// JVNetConst.JVN_STREAM_INFO);
		// Jni.sendString(targetIndex, JVNetConst.JVN_RSP_TEXTDATA, false, 0,
		// Consts.TYPE_SET_PARAM, String.format(
		// Consts.FORMATTER_SET_BPS_FPS, 1, 352, 288, 512, 25, 1));

		// MyAudio.getIntance(WHAT_AUDIO, this).stopRec();

		// int switcher = 0;
		// Jni.sendString(targetIndex, JVNetConst.JVN_RSP_TEXTDATA, false, 0,
		// JVNetConst.RC_SETPARAM,
		// String.format(Consts.FORMATTER_PN_SWITCH, switcher));

		// Jni.genVoice("123456789, 56498518");
	}

	public void onF3(View view) {
		// Jni.enablePlayAudio(targetIndex, false);
		// Jni.resumeAudio(targetIndex);
		// Jni.sendBytes(targetIndex, (byte) JVNetConst.JVN_REQ_CHAT, new
		// byte[0],
		// 8);

       Jni.initDenoisePlayer();
       Jni.recordAndsendAudioData(1);       
	    
//		Jni.startRecord(targetIndex, "/sdcard/out.mp4", true, true);

		// new Thread() {
		//
		// @Override
		// public void run() {
		// Jni.genVoice("Netcore_YFONE;1234567890", 3);
		// }
		//
		// }.start();

		// Jni.stopAudioRecord();
		// Jni.sendBytes(targetIndex, JVNetConst.JVN_CMD_PLAYSTOP, new byte[0],
		// 8);

		// Jni.fastForward(targetIndex);

		// Channel channel = channelList.get(targetIndex);
		// MyAudio.getIntance(WHAT_AUDIO, this, SAMPLE_RATE_DEVICE).startRec(
		// channel.getAudioEncType(), channel.getAudioBitCount(),
		// channel.getAudioBlock(), true);

		// Jni.pause(targetIndex);
		// Jni.sendBytes(targetIndex, JVNetConst.JVN_CMD_PLAYSTOP, new byte[0],
		// 8);
		// Jni.enablePlayback(targetIndex, false);

		// manager.setViewVisibility(
		// (View) (manager.getView(targetIndex).getParent()),
		// PlayWindowManager.ID_CONTROL_CENTER, View.VISIBLE);

		// Jni.screenshot(targetIndex, Consts.LOG_PATH + File.separator +
		// "j.jpg",
		// 80);

		// MyAudio.getIntance(WHAT_AUDIO, this).setVolume(2.0f);

		// Jni.shutdownRTMP(pointer);
		// pointer = (pointer + 1) % urls.length;

		// View v = manager.getView(targetIndex);
		// gestureOnView(v, channelList.get(targetIndex),
		// MyGestureDispatcher.GESTURE_TO_BIGGER, -1, new Point(0, -50),
		// new Point(v.getWidth(), v.getHeight()));

		// Channel channel = channelList.get(targetIndex);
		//
		// if (Consts.JAE_ENCODER_G729 == channel.getAudioEncType()) {
		// MyAudio.getIntance(WHAT_AUDIO, this, SAMPLE_RATE_DEVICE).startRec(
		// channel.getAudioEncType(), 16, channel.getAudioBlock(),
		// true);
		// } else {
		// MyAudio.getIntance(WHAT_AUDIO, this, SAMPLE_RATE_DEVICE).startRec(
		// channel.getAudioEncType(), channel.getAudioBitCount(),
		// channel.getAudioBlock(), true);
		// }

		// Jni.stopRecord();

		// screenCount = 9;
		// changeWindow();

		// int switcher = 1;
		// Jni.sendString(targetIndex, JVNetConst.JVN_RSP_TEXTDATA, false, 0,
		// JVNetConst.RC_SETPARAM,
		// String.format(Consts.FORMATTER_PN_SWITCH, switcher));
	}

	public void onF4(View view) {
		// Jni.pauseAudio(targetIndex);
		// Jni.sendBytes(targetIndex, (byte) JVNetConst.JVN_CMD_CHATSTOP,
		// new byte[0], 8);

//        Jni.recordOff();
	    Jni.playOn();
	    
//	    Jni.startAudioPlayer(6, 640);
//	    Jni.playAudioData(null);

//	    String path = "/sdcard/org.pcm";
//	    File f  = new File(path);
//	    File f1 = new File("/sdcard/org2.pcm");
//	    
//	    FileInputStream fis = null;
//	    FileOutputStream fos = null;
//	    byte[] b = new byte[640] ;
//	    	    
//        try {
//            fis = new FileInputStream(f);
//            fos = new FileOutputStream(f1);
//            while (fis.read(b)!=-1){
//                fos.write(b);
//                Jni.playAudioData(b);
//            }
//            
//            fis.close();
//            fos.close();
//        } catch (FileNotFoundException e) {
//            // TODO Auto-generated catch block
//            e.printStackTrace();
//        } catch (IOException e) {
//            // TODO Auto-generated catch block
//            e.printStackTrace();
//        }
        

        
//		Jni.stopRecord();

		// new Thread() {
		//
		// @Override
		// public void run() {
		// Jni.foo(null);
		// }
		//
		// }.start();

		// startTimeStamp = System.currentTimeMillis();
		// frameDecodeSum = 0.0f;
		// frameNetworkSum = 0.0f;

		// Jni.resume(targetIndex, channelList.get(targetIndex).getSurface());
		// Jni.setColor(targetIndex, 0, 0, 0, 0);

		// MyAudio.getIntance(WHAT_AUDIO, this, SAMPLE_RATE_DEVICE).stopRec();

		// gestureOnView(manager.getView(targetIndex),
		// channelList.get(targetIndex),
		// MyGestureDispatcher.GESTURE_TO_SMALLER, -1, new Point(50, 50),
		// new Point(500, 200));

		// MyAudio.getIntance(WHAT_AUDIO, this).stopPlay();
		// MyAudio.getIntance(WHAT_AUDIO, this).stopRec();

		// Jni.enablePlayback(targetIndex, true);
		// Jni.enablePlayAudio(targetIndex, true);
		//
		// String target = "./rec/00/20141211/A01113649.mp4";
		// Jni.sendBytes(targetIndex, JVNetConst.JVN_REQ_PLAY,
		// target.getBytes(),
		// target.length());

		// Jni.sendString(targetIndex, JVNetConst.JVN_RSP_TEXTDATA, false, 0,
		// Consts.TYPE_SET_PARAM, String.format(
		// Consts.FORMATTER_SET_BPS_FPS, 2, 352, 288, 512, 25, 1));

		// screenCount = 16;
		// changeWindow();

		// Jni.sendString(targetIndex, JVNetConst.JVN_RSP_TEXTDATA, false, 0,
		// JVNetConst.RC_SETPARAM,
		// String.format(Consts.FORMATTER_AP_SWITCH, 1));

		// Jni.enablePlayAudio(targetIndex, false);
	}

	public void onFoo(View view) {
		 new Thread() {
		
		 @Override
		 public void run() {
		 Jni.init(getApplication(), 9200, Consts.LOG_PATH);
		 Jni.enableLinkHelper(true, 3, 10);
		 Jni.setLinkHelper(genLinkHelperJson());
		 Jni.setStat(true);
		 try {
		 sleep(1000);
		 } catch (InterruptedException e) {
		 }
		 onConnect(null);
		 }
		
		 }.start();

//		gain *= 1.2f;
//		Boolean result = Jni.setAudioVolume(targetIndex, gain);
//		if (result) {
//			Toast.makeText(MainActivity.this,
//					String.format("gain = %.1f", gain), Toast.LENGTH_SHORT)
//					.show();
//		}

		// Jni.startAudioRecord(16, 640);

	}

	public void onBar(View view) {
		// new Thread() {
		//
		// @Override
		// public void run() {
		// // if (null != playAudio) {
		// // playAudio.interrupt();
		// // playAudio = null;
		// // }
		//
		// // Jni.enableLinkHelper(false, 3, 0);
		// onDisconnect(null);
		// Jni.setStat(false);
		//
		// // try {
		// // sleep(2000);
		// // } catch (InterruptedException e) {
		// // }
		// // Jni.deinit();
		// }
		//
		// }.start();

		gain *= 0.8f;
		Boolean result = Jni.setAudioVolume(targetIndex, gain);
		if (result) {
			Toast.makeText(MainActivity.this,
					String.format("gain = %.1f", gain), Toast.LENGTH_SHORT)
					.show();
		}

		// Jni.stopAudioRecord();

	}
	
	private boolean connect2(Channel channel, boolean isPlayDirectly) {
		boolean result = false;
		System.out.println("connnect 2");
		if (null != channel) {

			//Jni.NotifytoJni("/storage/sdcard1/hls.h264");
			
			int connect = 0;
			Device device = channel.getParent();

			String path = Environment.getExternalStorageDirectory().getPath();
			String url = "/jovetech/B129362515/2015/7/20";
			JSONObject obj = new JSONObject();
			try {
				obj.put("cshost", "oss-cn-hangzhou.aliyuncs.com");
				obj.put("csid", "4fZazqCFmQTbbmcw");
				obj.put("cskey", "sBT6DgBMdCHnnaYdvK0o6O3zaN06sW");
				obj.put("csspace", "jovetech");
				System.out.println("get index = "+channel.getIndex());
				System.out.println("Jni.CloudStorePlay");
				Jni.CloudStorePlay(channel.getIndex(), path,url, "M01154152",channel.getSurface(), false, "xxxxx", obj.toString());
			} catch (JSONException e) {
				e.printStackTrace();
			}
			
			// [Neo] TODO important
			channel.setLastPortWidth(channel.getSurfaceView().getWidth());
			channel.setLastPortHeight(channel.getSurfaceView().getHeight());

			if (Consts.BAD_CONN_OVERFLOW == connect) {
				handler.sendMessage(handler.obtainMessage(WHAT_CHANGE_INFO,
						channel.getIndex(), ARG2_INFO_OTHER, "overflow"));
			} else if (Consts.BAD_HAS_CONNECTED == connect) {
				handler.sendMessage(handler.obtainMessage(WHAT_CHANGE_INFO,
						channel.getIndex(), ARG2_INFO_OTHER, "has connected"));
			} else {
				channel.setConnecting(true);
				handler.sendMessage(handler.obtainMessage(WHAT_CHANGE_INFO,
						channel.getIndex(), ARG2_INFO_CONNECTING));
				result = true;
			}

		}

		return result;
	}

	public void onConnect(View view) {
		new Thread() {

			@Override
			public void run() {
				// Jni.foo(null);

				int size = channelList.size();
				for (int i = 0; i < size; i++) {
					connect2(channelList.get(i), true);
				}
			}

		}.start();
	}

	public void onDisconnect(View view) {
		new Thread() {

			@Override
			public void run() {
				int size = channelList.size();
				Channel channel = null;
				for (int i = 0; i < size; i++) {
					channel = channelList.get(i);
					if (channel.isConnected() || channel.isConnecting()) {
						if (enableYST) {
							Jni.disconnect(channel.getIndex());
						} else {
							Jni.shutdownRTMP(channel.getIndex());
							
							//Jni.NotifytoJniClose();
						}
					}
				}
			}

		}.start();
	}

	@Override
	public void onNotify(int what, int arg1, int arg2, Object obj) {

		Channel channel = null;

		switch (what) {
		case Consts.CALL_CONNECT_CHANGE: {
			channel = channelList.get(arg1);
			channel.setConnecting(false);

			switch (arg2) {
			case -3:
				channel.setPaused(true);
				channel.setConnected(false);
				handler.sendMessage(handler.obtainMessage(WHAT_CHANGE_INFO,
						arg1, ARG2_INFO_DISCONNECTED, obj));
				break;

			case 1:
			case Consts.RTMP_CONN_SCCUESS: {
				channel.setConnected(true);

				if (currentPageChannelList.contains(channel)) {
					if (channel.isPaused()) {
						resumeChannel(channel);
					}
				}

				handler.sendMessage(handler.obtainMessage(WHAT_CHANGE_INFO,
						arg1, ARG2_INFO_CONNECTED));
				break;
			}

			case 2:
			case 4:
			case 6:
			case 7:
			case Consts.RTMP_CONN_FAILED:
			case Consts.RTMP_DISCONNECTED:
			case Consts.RTMP_EDISCONNECT:
				break;

			default:
				handler.sendMessage(handler.obtainMessage(WHAT_CHANGE_INFO,
						arg1, ARG2_INFO_OTHER, "CC: " + arg2 + ", " + obj));
				break;
			}
			break;
		}

		case Consts.CALL_NORMAL_DATA: {
			channel = channelList.get(arg1);
			MyLog.d(Consts.TAG_XX, "meta: " + obj);

			try {
				JSONObject object = new JSONObject(String.valueOf(obj));

				fps = (float) object.getDouble("fps");

				channel.setAudioEncMeta(object.getInt("audio_enc_type"),
						object.getInt("audio_bit"));

				channel.setWidth(object.getInt("width"));
				channel.setHeight(object.getInt("height"));

				channel.setAudioEncMeta(object.getInt("audio_enc_type"),
						object.getInt("audio_bit"));

				if (enableYST) {
					channel.setDVR(Consts.DEVICE_TYPE_DVR == object
							.getInt("device_type"));
					MyLog.e(Consts.TAG_PLAY,
							"window = "
									+ channel.getIndex()
									+ String.format(", code = 0x%X",
											object.getInt("start_code"))
									+ ", is05 = " + object.getBoolean("is05")
									+ ", dvr = " + channel.isDVR() + ", bit = "
									+ channel.getAudioBitCount());
				}

			} catch (JSONException e) {
				e.printStackTrace();
			}

			handler.sendMessage(handler.obtainMessage(WHAT_CHANGE_INFO, arg1,
					ARG2_INFO_BUFFERING));
			break;
		}

		case Consts.CALL_NEW_PICTURE: {
			startTimeStamp = System.currentTimeMillis();
			handler.sendMessage(handler.obtainMessage(WHAT_CHANGE_INFO, arg1,
					ARG2_INFO_INVISIBLE));
			break;
		}

		case Consts.CALL_GEN_VOICE:
			MyLog.d(Consts.TAG_PLAY, "gen voice");
			// if (1 == arg2) {
			// byte[] bytes = (byte[]) obj;
			// MyAudio.getIntance(WHAT_AUDIO, MainActivity.this).put(bytes);
			// } else if (0 == arg2) {
			// byte[] bytes = new byte[] { 'F', 'i', 'n' };
			// MyAudio.getIntance(WHAT_AUDIO, MainActivity.this).put(bytes);
			// }
			break;

		case Consts.CALL_PLAY_AUDIO: {
			byte[] data = (byte[]) obj;
			MyAudio.getIntance(WHAT_AUDIO, MainActivity.this).put(data);
			break;
		}

		case Consts.CALL_TEXT_DATA: {
			try {
				MyLog.i(Consts.TAG_PLAY, ">>> textdata: " + arg1 + ", " + arg2
						+ ", " + new JSONObject(obj.toString()));
			} catch (Exception e) {
				e.printStackTrace();
			}
			break;
		}

		case Consts.CALL_LAN_SEARCH:
			MyLog.i(Consts.TAG_PLAY, ">>> lan: " + arg1 + ", " + arg2 + ", "
					+ obj);
			break;

		case Consts.CALL_PLAY_DOOMED:
			switch (arg2) {
			case Consts.VIDEO_SIZE_CHANGED:
				try {
					JSONObject object = new JSONObject(String.valueOf(obj));
					MyLog.d(Consts.TAG_XX, object.getInt("width") + "x"
							+ object.getInt("height"));
				} catch (JSONException e) {
					e.printStackTrace();
				}
				MyLog.i(Consts.TAG_PLAY, ">>> doomed: " + arg1 + ", " + arg2
						+ ", " + obj);
				break;

			default:
				break;
			}
			break;

		case Consts.CALL_DOWNLOAD:
		case Consts.CALL_GOT_SCREENSHOT:
		case Consts.CALL_CHECK_RESULT:
		case Consts.CALL_PLAY_DATA:
		case Consts.CALL_QUERY_DEVICE:
		case Consts.CALL_HDEC_TYPE:
			// case Consts.CALL_CHAT_DATA:
			break;

		default:
			handler.sendMessage(handler.obtainMessage(what, arg1, arg2, obj));
			break;
		}
	}

	@Override
	public void onHandler(int what, int arg1, int arg2, Object obj) {
		if(what == 161)
			System.out.println("连接成功");
		if(what == 162)
			System.out.println("O帧过来");
		if (isQuit) {
			return;
		}

		switch (what) {
		case WHAT_AUDIO_FOO: {
			if (audio_count > 0) {
				audio_count--;
				Jni.genVoice("testing;123456789", 0);
			}
			break;
		}

		case WHAT_AUDIO: {
			switch (arg1) {
			case MyAudio.ARG1_PLAY: {

				switch (arg2) {
				case MyAudio.ARG2_START:
					// handler.sendEmptyMessageDelayed(WHAT_AUDIO_FOO, 200);
					break;

				case MyAudio.ARG2_FINISH:
					break;

				case MyAudio.ARG2_VOICE_FIN:
					// handler.sendEmptyMessageDelayed(WHAT_AUDIO_FOO, 500);
					break;

				default:
					break;
				}

				break;
			}

			default:
				break;
			}

			break;
		}

		case WHAT_CHECK_SURFACE: {
			boolean hasNull = false;
			boolean hasChannel = false;

			int size = currentPageChannelList.size();
			for (int i = 0; i < size; i++) {
				hasChannel = true;
				if (null == currentPageChannelList.get(i).getSurface()) {
					hasNull = true;
					break;
				}
			}

			if (hasChannel && false == hasNull) {
				new Connecter().start();
			} else {
				handler.sendMessageDelayed(
						handler.obtainMessage(WHAT_CHECK_SURFACE, arg1, arg2),
						DELAY_CHECK_SURFACE);
			}
			break;
		}

		case WHAT_CHANGE_INFO: {
			TextView textView = (TextView) ((View) manager.getView(arg1)
					.getParent()).findViewById(PlayWindowManager.ID_INFO_TEXT);

			if (null != textView) {
				switch (arg2) {
				case ARG2_INFO_CONNECTING:
					textView.setText("Connecting");
					textView.setVisibility(View.VISIBLE);
					break;

				case ARG2_INFO_CONNECTED:
					textView.setText("Connected");
					textView.setVisibility(View.VISIBLE);
					break;

				case ARG2_INFO_BUFFERING:
					textView.setText("Buffering");
					textView.setVisibility(View.VISIBLE);
					break;

				case ARG2_INFO_RESUMING:
					textView.setText("Resuming");
					textView.setVisibility(View.VISIBLE);
					break;

				case ARG2_INFO_DISCONNECTED:
					String reason = "thread done";
					if (null != obj) {
						try {
							JSONObject object = new JSONObject(obj.toString());
							reason = object.getString("msg");
							if (TextUtils.isEmpty(reason)) {
								reason = "Disconn(null)";
							}
						} catch (JSONException e) {
							reason = obj.toString();
						}
					}
					textView.setText(reason);
					textView.setVisibility(View.VISIBLE);
					break;

				case ARG2_INFO_INVISIBLE:
					textView.setVisibility(View.INVISIBLE);
					break;

				case ARG2_INFO_OTHER:
					textView.setText(String.valueOf(obj));
					textView.setVisibility(View.VISIBLE);
					break;

				case ARG2_INFO_SURFACE_CREATED:
					textView.setText(String.valueOf(arg1));
					textView.setVisibility(View.VISIBLE);
					break;

				case ARG2_INFO_SURFACE_DESTORYED:
					textView.setText("!");
					textView.setVisibility(View.VISIBLE);
					break;

				case ARG2_INFO_SURFACE_TODO:
					textView.setText(arg1 + ":" + arg1);
					textView.setVisibility(View.VISIBLE);
					break;

				default:
					textView.setText("Another");
					textView.setVisibility(View.VISIBLE);
					break;
				}
			}

			break;
		}

		case WHAT_RESTORE_UI:
			isBlockUi = false;
			if (null != pager) {
				pager.setDisableSliding(isBlockUi);
			}
			break;

		case Consts.CALL_CHAT_DATA: {
			Toast.makeText(this, String.format("chat: 0x%02X", arg2),
					Toast.LENGTH_SHORT).show();
			break;
		}

		case Consts.CALL_STAT_REPORT: {
			try {
				JSONArray array = new JSONArray(obj.toString());
				JSONObject object = null;

				int size = array.length();
				StringBuilder sBuilder = new StringBuilder(1024 * size);
				for (int i = 0; i < size; i++) {
					object = array.getJSONObject(i);

					if (targetIndex == i) {
						// frameDecodeSum += object.getDouble("decoder_fps");
						// frameNetworkSum += object.getDouble("network_fps");
						// SimpleDateFormat format = new
						// SimpleDateFormat("mm:ss");
						// long played = System.currentTimeMillis()
						// - startTimeStamp;
						// float inSeconds = played / 1000.0f;
						// additional
						// .setText(String
						// .format("played: %s\nshould: %.1f\nnetwork: %.1f, %.1f, %.1f\ndecoded: %.1f, %.1f, %.1f\nleft: %d",
						// format.format(played), fps
						// * inSeconds,
						// frameNetworkSum,
						// frameNetworkSum / inSeconds,
						// frameNetworkSum
						// - (fps * inSeconds),
						// frameDecodeSum, frameDecodeSum
						// / inSeconds,
						// frameDecodeSum
						// - (fps * inSeconds),
						// object.getInt("left")));
					}

					String msg = String
							.format(Locale.CHINA,
									"stat%d(%s.%s.%s), v: kbps=%.2fK, fps=%.0f+%.0f/%.0f/%d, %.2fms+%.2fms, left=%d,\n\t\t%dx%d; a(%d|%d): kbps=%.2fK, fps=%.0f+%d/%.0f, %.2fms+%.2fms, left = %d.",
									object.getInt("window"), (object
											.getBoolean("is_turn") ? "TURN"
											: "P2P"),
									(object.getBoolean("is_omx") ? "HD" : "-"),
									(object.getBoolean("is_playback") ? "P"
											: "N"), object.getDouble("kbps"),
									object.getDouble("decoder_fps"), object
											.getDouble("video_jump_fps"),
									object.getDouble("network_fps"), object
											.getInt("video_space"), object
											.getDouble("decoder_delay"), object
											.getDouble("render_delay"), object
											.getInt("video_left"), object
											.getInt("width"), object
											.getInt("height"), object
											.getInt("audio_type"), object
											.getInt("audio_enc_type"), object
											.getDouble("audio_kbps"), object
											.getDouble("audio_decoder_fps"),
									object.getInt("audio_jump_fps"), object
											.getDouble("audio_network_fps"),
									object.getDouble("audio_decoder_delay"),
									object.getDouble("audio_play_delay"),
									object.getInt("audio_left"));
					sBuilder.append(msg).append("\n");
				}

				logView.setText(audioLog + "\n" + sBuilder.toString());

			} catch (Exception e) {
				e.printStackTrace();
			}

			break;
		}

		case Consts.CALL_PLAY_BUFFER: {
			if (arg2 > 0) {
				MyLog.i(Consts.TAG_PLAY, "buffering: " + arg2);
			} else if (Consts.BUFFER_START == arg2) {
				MyLog.w(Consts.TAG_PLAY, "buffer started");
			} else if (Consts.BUFFER_FINISH == arg2) {
				MyLog.w(Consts.TAG_PLAY, "buffer finished");
			}
			break;
		}

		default:
			MyLog.e(Consts.TAG_XX, "NO switch:" + what);
			break;
		}
	}

	// private class DummyInputer extends Thread {
	//
	// @Override
	// public void run() {
	// MyLog.w(Consts.TAG_PLAY, "DummyInputer E");
	// File file = new File(Consts.LOG_PATH + "/dummy.wav");
	// if (file.exists()) {
	// file.delete();
	// }
	//
	// FileOutputStream outputStream = null;
	// try {
	// outputStream = new FileOutputStream(file);
	// } catch (FileNotFoundException e) {
	// e.printStackTrace();
	// }
	//
	// // byte[] enc = null;
	// // Jni.initAudioEncoder(1, SAMPLE_RATE_HUMAN, 1, 16, 640, 0, 0);
	// // enc = Jni.encodeAudio(data);
	// // Jni.deinitAudioEncoder();
	//
	// // if (null != enc) {
	// // outputStream.write(enc);
	// // }
	//
	// if (null != outputStream) {
	// try {
	// outputStream.close();
	// } catch (IOException e) {
	// e.printStackTrace();
	// }
	// }
	// MyLog.w(Consts.TAG_PLAY, "DummyInputer X");
	// }
	//
	// }

	// private class DummyOutputer extends Thread {
	//
	// @Override
	// public void run() {
	// MyLog.w(Consts.TAG_PLAY, "DummyOutputer E");
	// // [Neo] test for audio play from file
	// FileInputStream inputStream = null;
	// try {
	// inputStream = new FileInputStream(Consts.LOG_PATH
	// + File.separator + "dummy.wav");
	// byte[] buffer = new byte[AUDIO_FRAME_SIZE];
	//
	// int count = 0;
	// int offset = 0;
	// int length = AUDIO_FRAME_SIZE;
	//
	// byte[] enc = null;
	//
	// MyLog.e(Consts.TAG_PLAY,
	// "init audio encoder = "
	// + Jni.initAudioEncoder(1, SAMPLE_RATE_HUMAN, 1, 16, 640));
	//
	// while (-1 != (count = inputStream.read(buffer, offset, length))) {
	// if (offset + count < AUDIO_FRAME_SIZE) {
	// offset += count;
	// length = AUDIO_FRAME_SIZE - offset;
	// continue;
	// }
	//
	// // [Neo] o1. just play out
	// audioQueue.offer(buffer.clone());
	//
	// // [Neo] o2. try network
	// enc = Jni.encodeAudio(buffer);
	// Jni.sendBytes(0, JVNetConst.JVN_RSP_CHATDATA, enc,
	// enc.length);
	// // System.out.println(String.format(
	// // "check: [%d] %02X %02X %02X", enc.length, enc[0],
	// // enc[10], enc[enc.length - 1]));
	//
	// Thread.sleep(40);
	// }
	//
	// } catch (Exception e) {
	// e.printStackTrace();
	// } finally {
	// if (null != inputStream) {
	// try {
	// inputStream.close();
	// } catch (IOException e) {
	// }
	// }
	// }
	//
	// MyLog.e(Consts.TAG_PLAY,
	// "deinit audio encoder = " + Jni.deinitAudioEncoder());
	//
	// MyLog.w(Consts.TAG_PLAY, "DummyOutputer X");
	// }
	//
	// }

	// public boolean bytes2jpg(byte[] data, String fileName, int width,
	// int height, int quality) {
	// boolean result = false;
	//
	// ByteBuffer buffer = ByteBuffer.wrap(data);
	// Bitmap bitmap = Bitmap.createBitmap(width, height,
	// Bitmap.Config.RGB_565);
	// buffer.position(0);
	// bitmap.copyPixelsFromBuffer(buffer);
	//
	// buffer.clear();
	// data = null;
	//
	// FileOutputStream outputStream = null;
	// try {
	// outputStream = new FileOutputStream(fileName);
	// result = bitmap.compress(Bitmap.CompressFormat.JPEG, quality,
	// outputStream);
	// } catch (Exception e) {
	// e.printStackTrace();
	// } finally {
	// if (null != outputStream) {
	// try {
	// outputStream.close();
	// } catch (IOException e) {
	// }
	// }
	// }
	//
	// bitmap.recycle();
	// bitmap = null;
	// return result;
	// }

	private String genLinkHelperJson() {
		JSONArray array = new JSONArray();
		JSONObject object = null;

		for (Device device : deviceList) {
			if (device.getNo() > 0) {
				try {
					object = new JSONObject();
					object.put("gid", device.getGid());
					object.put("no", device.getNo());
					object.put("channel", 1);
					object.put("name", device.getUser());
					object.put("pwd", device.getPwd());
					array.put(object);
				} catch (JSONException e) {
					e.printStackTrace();
				}
			}
		}

		return array.toString();
	}
	

	private boolean connect(Channel channel, boolean isPlayDirectly) {
		boolean result = false;

		// if (false == enableYST) {
		// return false;
		// }

		if (null != channel && false == channel.isConnected()
				&& false == channel.isConnecting()) {

			int connect = 0;
			Device device = channel.getParent();

			if (isPlayDirectly) {
				if (enableYST) {
					connect = Jni.connect(channel.getIndex(),
							channel.getChannel(), device.getIp(),
							device.getPort(), device.getUser(),
							device.getPwd(), device.getNo(), device.getGid(),
							true, 1, true, (device.isHomeProduct() ? 6 : 5),
							channel.getSurface(), false, isTcp, false, tryOmx,
							THUMB_FILE_NAME);
				} else {
					connect = Jni.connectRTMP(channel.getIndex(),
							channel.getUrl(), channel.getSurface(), tryOmx,
							THUMB_FILE_NAME) ? channel.getIndex() : -1;
				}
				if (channel.getIndex() == connect) {
					channel.setPaused(null == channel.getSurface());
				}
			} else {
				if (enableYST) {
					connect = Jni.connect(channel.getIndex(),
							channel.getChannel(), device.getIp(),
							device.getPort(), device.getUser(),
							device.getPwd(), device.getNo(), device.getGid(),
							true, 1, true, (device.isHomeProduct() ? 6 : 5),
							null, false, isTcp, false, tryOmx, THUMB_FILE_NAME);
				} else {
					connect = Jni.connectRTMP(channel.getIndex(),
							channel.getUrl(), channel.getSurface(), tryOmx,
							THUMB_FILE_NAME) ? channel.getIndex() : -1;
				}
				if (channel.getIndex() == connect) {
					channel.setPaused(true);
				}
			}

			// [Neo] TODO important
			channel.setLastPortWidth(channel.getSurfaceView().getWidth());
			channel.setLastPortHeight(channel.getSurfaceView().getHeight());

			if (Consts.BAD_CONN_OVERFLOW == connect) {
				handler.sendMessage(handler.obtainMessage(WHAT_CHANGE_INFO,
						channel.getIndex(), ARG2_INFO_OTHER, "overflow"));
			} else if (Consts.BAD_HAS_CONNECTED == connect) {
				handler.sendMessage(handler.obtainMessage(WHAT_CHANGE_INFO,
						channel.getIndex(), ARG2_INFO_OTHER, "has connected"));
			} else {
				channel.setConnecting(true);
				handler.sendMessage(handler.obtainMessage(WHAT_CHANGE_INFO,
						channel.getIndex(), ARG2_INFO_CONNECTING));
				result = true;
			}

		}

		return result;
	}

	private boolean resumeChannel(Channel channel) {
		boolean result = false;

		if (null != channel && channel.isConnected() && channel.isPaused()
				&& null != channel.getSurface()) {
			result = Jni.sendBytes(channel.getIndex(),
					JVNetConst.JVN_CMD_VIDEO, new byte[0], 8);

			if (result) {
				if (Jni.resume(channel.getIndex(), channel.getSurface())) {
					handler.sendMessage(handler.obtainMessage(WHAT_CHANGE_INFO,
							channel.getIndex(), ARG2_INFO_RESUMING));
				}

				channel.setPaused(false);
			}
		}

		return result;
	}

	private boolean pauseChannel(Channel channel) {
		boolean result = false;

		if (null != channel && channel.isConnected()
				&& false == channel.isPaused()) {
			result = Jni.sendBytes(channel.getIndex(),
					JVNetConst.JVN_CMD_VIDEOPAUSE, new byte[0], 8);

			if (result) {
				Jni.pause(channel.getIndex());
				channel.setPaused(true);
			}
		}

		return result;
	}

	private void changeWindow() {
		adapter.update(manager.genPageList(screenCount));
		lastItemIndex = lastClickIndex / screenCount;
		currentPageChannelList = manager.getValidChannelList(lastItemIndex);

		pager.setAdapter(adapter);
		pager.setCurrentItem(lastItemIndex, false);

		isBlockUi = true;
		pager.setDisableSliding(isBlockUi);
		changeBorder(lastClickIndex);

		handler.removeMessages(WHAT_CHECK_SURFACE);
		handler.sendMessageDelayed(handler.obtainMessage(WHAT_CHECK_SURFACE,
				lastItemIndex, lastClickIndex), DELAY_CHECK_SURFACE);
	}

	private void changeBorder(int currentIndex) {
		if (lastClickIndex != currentIndex) {
			if (lastClickIndex >= 0) {
				((View) manager.getView(lastClickIndex).getParent())
						.setBackgroundColor(BORDER_UNSELECT_COLOR);
			}
			lastClickIndex = currentIndex;
		}

		((View) manager.getView(currentIndex).getParent())
				.setBackgroundColor(BORDER_SELECT_COLOR);
	}

	private class MyPagerAdapter extends PagerAdapter {

		private ArrayList<View> list;

		public void update(ArrayList<View> list) {
			if (null != list) {
				this.list = list;
			}
		}

		@Override
		public int getCount() {
			int result = 0;
			if (null != list) {
				result = list.size();
			}
			return result;
		}

		@Override
		public boolean isViewFromObject(View arg0, Object arg1) {
			return (arg0 == arg1);
		}

		@Override
		public void destroyItem(ViewGroup container, int position, Object object) {
			if (null != list && list.size() > position) {
				container.removeView(list.get(position));

				if (1 == screenCount) {
					disconnectChannelList.add(manager.getChannel(position));
				}
			}
		}

		@Override
		public Object instantiateItem(ViewGroup container, int position) {
			View result = null;
			if (null != list && list.size() > position) {
				result = list.get(position);
				container.addView(result);
			}
			return result;
		}

	}

	private class DoubleClickChecker extends TimerTask {

		@Override
		public void run() {
			cancel();
			isDoubleClickCheck = false;
		}

	}

	private class Connecter extends Thread {

		@Override
		public void run() {
			try {
				int size = disconnectChannelList.size();

				MyLog.w(Consts.TAG_PLAY, "disconnect count: " + size);
				for (int i = 0; i < size; i++) {
					Channel channel = disconnectChannelList.get(i);

					int index = channel.getIndex();
					boolean needConnect = false;
					for (Channel currentChannel : currentPageChannelList) {
						if (index == currentChannel.getIndex()) {
							MyLog.w(Consts.TAG_PLAY,
									"disconnect not for current: " + channel);
							needConnect = true;
							break;
						}
					}
					if (needConnect) {
						continue;
					}

					if (channel.isConnected() || channel.isConnecting()) {

						boolean result = false;
						if (enableYST) {
							result = Jni.disconnect(channel.getIndex());
						} else {
							result = Jni.shutdownRTMP(channel.getIndex());
						}

						if (false == result) {
							MyLog.e(Consts.TAG_PLAY, "disconnect failed: "
									+ channel);
						} else {
							boolean needSleep = true;
							while (needSleep) {
								needSleep = false;
								if (channel.isConnected()
										|| channel.isConnecting()) {
									needSleep = true;
									MyLog.w(Consts.TAG_PLAY,
											"wait for change: " + channel);
									sleep(DISCONNECTION_MIN_PEROID);
									break;
								}
							}
						}
					} else {
						MyLog.w(Consts.TAG_PLAY, "disconnect has done: "
								+ index);
					}

				}
				disconnectChannelList.clear();

				size = currentPageChannelList.size()
						+ connectChannelList.size();

				MyLog.w(Consts.TAG_PLAY, "connect count: " + size);
				for (int i = 0; i < size; i++) {
					Channel channel = null;
					boolean isPlayDirectly = false;

					if (i < screenCount) {
						isPlayDirectly = true;
						channel = currentPageChannelList.get(i);
					} else {
						channel = connectChannelList.get(i - screenCount);
					}

					if (false == channel.isConnected()
							&& false == channel.isConnecting()) {

						boolean result = connect(channel, isPlayDirectly);
						if (false == result) {
							MyLog.e(Consts.TAG_PLAY, "connect failed: "
									+ channel);
						} else {
							sleep(CONNECTION_MIN_PEROID);
						}

					} else if (channel.isConnecting()) {
						handler.sendMessage(handler.obtainMessage(
								WHAT_CHANGE_INFO, channel.getIndex(),
								ARG2_INFO_CONNECTING));
					} else if (false == channel.isPaused()) {
						handler.sendMessage(handler.obtainMessage(
								WHAT_CHANGE_INFO, channel.getIndex(),
								ARG2_INFO_CONNECTED));

						channel.setPaused(true);
						boolean result = resumeChannel(channel);
						if (false == result) {
							MyLog.e(Consts.TAG_PLAY, "force resume failed: "
									+ channel);
						} else {
							sleep(RESUME_VIDEO_MIN_PEROID);
							MyLog.e(Consts.TAG_PLAY, "force resume: " + channel);
						}

					} else {
						boolean result = resumeChannel(channel);
						if (false == result) {
							MyLog.e(Consts.TAG_PLAY, "resume failed: "
									+ channel);
						} else {
							sleep(RESUME_VIDEO_MIN_PEROID);
						}
					}
				}
				connectChannelList.clear();

			} catch (Exception e) {
				// [Neo] Empty
				e.printStackTrace();
			}

			handler.sendEmptyMessage(WHAT_RESTORE_UI);
		}
	}

	
	
	public void gotonext(View view){
//		Intent intent = new Intent(MainActivity.this,hlsActivity.class);
//		startActivity(intent);
		//Jni.NotifytoJni("xx");
		Jni.CloudStoreClose();
	}
}
