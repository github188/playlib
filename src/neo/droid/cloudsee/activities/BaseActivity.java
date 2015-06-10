package neo.droid.cloudsee.activities;

import neo.droid.cloudsee.IHandlerLikeNotify;
import neo.droid.cloudsee.IHandlerNotify;
import neo.droid.cloudsee.MainApplication;
import neo.droid.cloudsee.R;

import android.app.Activity;
import android.app.ProgressDialog;
import android.os.Bundle;
import android.os.Handler;
import android.os.Message;
/**
 * 抽象的活动基类，所有活动都应该继承这个类，并实现其抽象方法和接口
 * 
 * @author neo
 * 
 */
public abstract class BaseActivity extends Activity implements IHandlerNotify,
		IHandlerLikeNotify {

	/** 初始化设置，不要在这里写费时的操作 */
	protected abstract void initSettings();

	/** 初始化界面，不要在这里写费时的操作 */
	protected abstract void initUi();

	/** 保存设置，不要在这里写费时的操作 */
	protected abstract void saveSettings();

	/** 释放资源、解锁、删除不用的对象，不要在这里写费时的操作 */
	protected abstract void freeMe();

	private IHandlerNotify notify = this;
	protected MyHandler handler = new MyHandler(this);
	protected ProgressDialog proDialog;
	protected static class MyHandler extends Handler {

		private BaseActivity activity;

		public MyHandler(BaseActivity activity) {
			this.activity = activity;
		}

		@Override
		public void handleMessage(Message msg) {
			activity.notify.onHandler(msg.what, msg.arg1, msg.arg2, msg.obj);
			super.handleMessage(msg);
		}

	}

	@Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		((MainApplication) getApplication()).push(this);
		((MainApplication) getApplication()).setCurrentNotifyer(this);
		initSettings();
		initUi();
	}

	@Override
	protected void onDestroy() {
		((MainApplication) getApplication()).pop();
		freeMe();
		super.onDestroy();
	}

	@Override
	protected void onPause() {
		saveSettings();
		super.onPause();
	}

	@Override
	protected void onResume() {
		super.onResume();
	}
    /**
     * 弹dialog
     * 
     * @param id
     */
    protected void createDialog(int id, boolean cancel) {
        try {
            if (null != BaseActivity.this && !BaseActivity.this.isFinishing()) {
                if (null == proDialog) {
                    proDialog = new ProgressDialog(BaseActivity.this);
                }
                proDialog.setMessage(BaseActivity.this.getString(id));
                if (null != proDialog) {
                    if (null != BaseActivity.this
                            && !BaseActivity.this.isFinishing()) {
                        proDialog.show();
                        proDialog.setCancelable(cancel);
                    }
                }
            }
        } catch (Exception e) {
            e.printStackTrace();
        }

    }

    /**
     * 弹dialog
     * 
     * @param msg
     */
    public void createDialog(String msg, boolean cancel) {

        if (null == msg || "".equalsIgnoreCase(msg)) {
            msg = getResources().getString(R.string.waiting);
        }
        try {
            if (null != BaseActivity.this && !BaseActivity.this.isFinishing()) {
                if (null == proDialog) {
                    proDialog = new ProgressDialog(BaseActivity.this);
                }
                proDialog.setMessage(msg);
                if (null != proDialog) {
                    if (null != BaseActivity.this
                            && !BaseActivity.this.isFinishing()) {
                        proDialog.show();
                        proDialog.setCancelable(cancel);
                    }
                }
            }
        } catch (Exception e) {
            e.printStackTrace();
        }

    }

    /**
     * 关闭dialog
     * 
     * @param dialog
     */
    public void dismissDialog() {
        if (null != proDialog && !this.isFinishing() && proDialog.isShowing()) {
            proDialog.dismiss();
            proDialog = null;
        }
    }
}
