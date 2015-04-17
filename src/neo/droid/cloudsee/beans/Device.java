package neo.droid.cloudsee.beans;

import java.util.ArrayList;

/**
 * 简单的设备集合类
 * 
 * @author neo
 * 
 */
public class Device {

	private ArrayList<Channel> channelList;

	private String ip;
	private int port;

	private String gid;
	private int no;
	private String fullNo;

	private String user;
	private String pwd;

	private boolean isHomeProduct;
	private boolean isHelperEnabled;

	/**
	 * 创建指定通道个数的设备
	 * 
	 * @param ip
	 * @param port
	 * @param gid
	 * @param no
	 * @param user
	 * @param pwd
	 * @param isHomeProduct
	 * @param channelCount
	 */
	public Device(String ip, int port, String gid, int no, String user,
			String pwd, boolean isHomeProduct, int channelCount) {
		this.ip = ip;
		this.port = port;
		this.gid = gid;
		this.no = no;
		this.fullNo = gid + no;
		this.user = user;
		this.pwd = pwd;
		this.isHomeProduct = isHomeProduct;

		isHelperEnabled = false;
		channelList = new ArrayList<Channel>();

		for (int i = 0; i < channelCount; i++) {
			Channel channel = new Channel(this, 1);
			// Channel channel = new Channel(this, (i % 16) + 1);
			channelList.add(channel);
		}
	}

	public ArrayList<Channel> getChannelList() {
		return channelList;
	}

	public String getIp() {
		return ip;
	}

	public int getPort() {
		return port;
	}

	public String getGid() {
		return gid;
	}

	public int getNo() {
		return no;
	}

	public String getFullNo() {
		return fullNo;
	}

	public String getUser() {
		return user;
	}

	public String getPwd() {
		return pwd;
	}

	public boolean isHomeProduct() {
		return isHomeProduct;
	}

	public void setHelperEnabled(boolean isHelperEnabled) {
		this.isHelperEnabled = isHelperEnabled;
	}

	public void setHomeProduct(boolean isHomeProduct) {
		this.isHomeProduct = isHomeProduct;
	}

	public void setNo(int no) {
		this.no = no;
	}

	public boolean isHelperEnabled() {
		return isHelperEnabled;
	}

	@Override
	public String toString() {
		StringBuilder sBuilder = new StringBuilder(1024);
		sBuilder.append(fullNo).append("(").append(ip).append(":").append(port)
				.append("): ").append("user = ").append(user)
				.append(", pwd = ").append(pwd).append(", enabled = ")
				.append(isHelperEnabled);
		if (null != channelList) {
			int size = channelList.size();
			for (int i = 0; i < size; i++) {
				sBuilder.append("\n").append(channelList.get(i).toString());
			}
		}
		return sBuilder.toString();
	}
}
