package neo.droid.cloudsee.beans;

import neo.droid.cloudsee.Consts;
import android.view.Surface;
import android.view.SurfaceView;

/**
 * 简单的通道集合类
 * 
 * @author neo
 * 
 */
public class Channel {

	private Device parent;
	/** 窗口索引 */
	private int index;
	/** 设备通道，从 0 开始 */
	private int channel;

	private boolean isConnecting;
	private boolean isConnected;
	private boolean isPaused;

	private boolean isDVR;

	private int audioEncType;
	private int audioBitCount;
	private int audioBlock;

	private int width;
	private int height;

	private int lastPortLeft;
	private int lastPortBottom;
	private int lastPortWidth;
	private int lastPortHeight;

	private String url;

	private Surface surface;
	private SurfaceView surfaceView;

	public Channel(Device device, int channel) {
		this.parent = device;
		this.channel = channel;

		this.index = -1;
		width = -1;
		height = -1;

		this.isConnecting = false;
		this.isConnected = false;
		this.isPaused = true;

		this.surface = null;
		this.surfaceView = null;
	}

	public int getIndex() {
		return index;
	}

	public void setIndex(int index) {
		this.index = index;
	}

	public String getUrl() {
		return url;
	}

	public void setUrl(String url) {
		this.url = url;
	}

	public int getWidth() {
		return width;
	}

	public void setWidth(int width) {
		this.width = width;
	}

	public int getHeight() {
		return height;
	}

	public void setHeight(int height) {
		this.height = height;
	}

	public int getLastPortLeft() {
		return lastPortLeft;
	}

	public void setLastPortLeft(int lastPortLeft) {
		this.lastPortLeft = lastPortLeft;
	}

	public int getLastPortBottom() {
		return lastPortBottom;
	}

	public void setLastPortBottom(int lastPortBottom) {
		this.lastPortBottom = lastPortBottom;
	}

	public int getLastPortWidth() {
		return lastPortWidth;
	}

	public void setLastPortWidth(int lastPortWidth) {
		this.lastPortWidth = lastPortWidth;
	}

	public int getLastPortHeight() {
		return lastPortHeight;
	}

	public void setLastPortHeight(int lastPortHeight) {
		this.lastPortHeight = lastPortHeight;
	}

	public boolean isConnecting() {
		return isConnecting;
	}

	public void setConnecting(boolean isConnecting) {
		this.isConnecting = isConnecting;
	}

	public boolean isConnected() {
		return isConnected;
	}

	public void setConnected(boolean isConnected) {
		this.isConnected = isConnected;
	}

	public boolean isPaused() {
		return isPaused;
	}

	public void setPaused(boolean isPaused) {
		this.isPaused = isPaused;
	}

	public Surface getSurface() {
		return surface;
	}

	public void setSurface(Surface surface) {
		this.surface = surface;
	}

	public SurfaceView getSurfaceView() {
		return surfaceView;
	}

	public void setSurfaceView(SurfaceView surfaceView) {
		this.surfaceView = surfaceView;
	}

	public Device getParent() {
		return parent;
	}

	public int getChannel() {
		return channel;
	}

	public int getAudioEncType() {
		return audioEncType;
	}

	public int getAudioBitCount() {
		return audioBitCount;
	}

	public int getAudioBlock() {
		return audioBlock;
	}

	public void setAudioEncMeta(int audioEncType, int audioBitCount) {
		this.audioEncType = audioEncType;
		this.audioBitCount = audioBitCount;

		switch (audioEncType) {
		case Consts.JAE_ENCODER_ALAW:
		case Consts.JAE_ENCODER_ULAW:
			audioBlock = Consts.ENC_G711_SIZE;
			break;

		case Consts.JAE_ENCODER_SAMR:
			audioBlock = Consts.ENC_AMR_SIZE;
			break;

		case Consts.JAE_ENCODER_G729:
			audioBlock = Consts.ENC_G729_SIZE;
			break;

		default:
			if (8 == audioBitCount) {
				audioBlock = Consts.ENC_8BIT_PCM_SIZE;
			} else {
				audioBlock = Consts.ENC_16BIT_PCM_SIZE;
			}
			break;
		}
	}

	public boolean isDVR() {
		return isDVR;
	}

	public void setDVR(boolean isDVR) {
		this.isDVR = isDVR;
	}

	@Override
	public String toString() {
		StringBuilder sBuilder = new StringBuilder(1024);
		sBuilder.append("Channel-").append((channel < 0) ? "X" : channel)
				.append(", window = ").append(index)
				.append(": isConnecting = ").append(isConnecting)
				.append(": isConnected = ").append(isConnected)
				.append(", isPaused: ").append(isPaused).append(", surface = ")
				.append((null != surface) ? surface.hashCode() : "null");
		return sBuilder.toString();
	}

}
