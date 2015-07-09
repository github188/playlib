package neo.droid.cloudsee.activities;

import com.jovision.Jni;

import neo.droid.cloudsee.R;
import android.app.Activity;
import android.os.Bundle;
import android.os.Environment;
import android.util.Log;
import android.view.View;
import android.view.View.OnClickListener;
import android.widget.Button;

public class TestAct extends Activity {
	private Button btnBroadcast;
	private Button btnStopBroadcast;
	@Override
	protected void onCreate(Bundle savedInstanceState) {
		// TODO Auto-generated method stub
		super.onCreate(savedInstanceState);
		
		setContentView(R.layout.testlayout);
		
		initViews();
		setOnlister();
	}

	
	public void initViews(){
		btnBroadcast = (Button) findViewById(R.id.btnBroadcast);
		btnStopBroadcast = (Button) findViewById(R.id.btnStopbroadcast);
		
	}
	
	public void setOnlister(){
		btnBroadcast.setOnClickListener(new OnClickListener() {
			
			@Override
			public void onClick(View arg0) {
				// TODO Auto-generated method stub
//			    Jni.searchLanServer(9400, 6666);
//				int res = Jni.searchLanDevice("", 0, 0, 0, "", 1 * 40 * 1000, 2);
//				String url = Environment.getExternalStorageDirectory().getPath()+"/mmm.3u8";
//				Jni.NotifytoJni(url);
			}
		});
		
		
		btnStopBroadcast.setOnClickListener(new OnClickListener() {
			
			@Override
			public void onClick(View arg0) {
				// TODO Auto-generated method stub
//			int returnResult=	Jni.StopMobLansearch();
//			Log.e("jack", "result: "+returnResult);
//				Jni.stopSearchLanServer();
				
			}
		});
	}
}
