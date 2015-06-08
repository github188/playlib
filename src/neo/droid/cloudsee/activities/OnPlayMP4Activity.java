package neo.droid.cloudsee.activities;

import android.content.pm.ActivityInfo;
import android.os.Bundle;
import android.os.Environment;
import android.util.DisplayMetrics;
import android.util.Log;
import android.view.SurfaceHolder;
import android.view.SurfaceView;
import android.view.View;
import android.view.Window;
import android.view.WindowManager;
import android.widget.Button;
import android.widget.RelativeLayout;
import android.widget.SeekBar;
import android.widget.TextView;
import android.widget.Toast;

import com.jovision.Jni;

import org.json.JSONException;
import org.json.JSONObject;

import java.io.File;

import neo.droid.cloudsee.Consts;
import neo.droid.cloudsee.R;

public class OnPlayMP4Activity extends BaseActivity implements SurfaceHolder.Callback, View.OnClickListener, SeekBar.OnSeekBarChangeListener{

    public static final String SD_CARD_PATH = Environment
            .getExternalStorageDirectory().getPath() + File.separator;    
    
//    static {
//        System.loadLibrary("alu");
//        System.loadLibrary("play");
//    }
    
    private SurfaceView mSurfaceView;
    private SurfaceHolder surfaceHolder;
    private Button pause;
    private boolean bpause = false;
    private TextView tv_step, tv_total;
    private SeekBar play_bar;
    private int mSurfaceViewWidth, mSurfaceViewHeight;
    private static final String TAG = "OnPlayMP4Activity";
    private boolean IsVideoReadyToBePlayed = false;
    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        requestWindowFeature(Window.FEATURE_NO_TITLE);
        getWindow().setFlags(WindowManager.LayoutParams.FLAG_FULLSCREEN,
                WindowManager.LayoutParams.FLAG_FULLSCREEN);        
        setContentView(R.layout.onplay_mp4_layout);
        //Jni.init(getApplication(), 9200, Consts.LOG_PATH);
        // [Neo] TODO
        //Jni.enableLog(true);   
        
        DisplayMetrics dm = new DisplayMetrics();
        getWindowManager().getDefaultDisplay().getMetrics(dm);
        mSurfaceViewWidth = dm.widthPixels;
        mSurfaceViewHeight = dm.heightPixels;
        
        pause=(Button)findViewById(R.id.btn_pause);

        play_bar = (SeekBar) findViewById(R.id.seekbar_def);
        play_bar.setOnSeekBarChangeListener(this);
        play_bar.setEnabled(false);//一开始先不支持拖动
        
        tv_step = (TextView)findViewById(R.id.tv_step);
        tv_total = (TextView)findViewById(R.id.tv_total);
        
        pause.setOnClickListener(this);
        pause.setText("暂停");               
        
        Jni.Mp4Init();
        Jni.SetMP4Uri(SD_CARD_PATH+"CSAlarmVOD/848x480_fps25_h264_alaw.mp4");
        int ret = Jni.Mp4Prepare();   
        if(ret != 0)
        {
            Log.e("MP4", "Jni.Mp4Prepare failed:"+ret);
            IsVideoReadyToBePlayed = false;
            Toast.makeText(this, "该视频不合法:"+ret, Toast.LENGTH_LONG);
            this.finish();            
        } 
        else{
            Log.e("MP4", "Jni.Mp4Prepare success:"+ret);
            IsVideoReadyToBePlayed = true;            
        }        


    }
    
    @Override
    protected void onResume(){
        setRequestedOrientation(ActivityInfo.SCREEN_ORIENTATION_LANDSCAPE);
        super.onResume();
    }
    
    private String playTimeFormat(int in_seconds){
        int play_minute = in_seconds/60;
        int play_second = in_seconds - play_minute*60;
        
        String str_minute = String.format("%02d", play_minute);
        String str_second = String.format("%02d", play_second); 
        
        String str_format_res = str_minute+":"+str_second;
        
        return str_format_res;
    }
    
    private void onSurfaceSizeInit(int width, int height){
        if (width == 0 || height == 0) {
            Log.e(TAG, "invalid video width(" + width + ") or height(" + height
                    + ")");
            return;
        }
        if(!IsVideoReadyToBePlayed){
            Toast.makeText(this, "视频不合法", Toast.LENGTH_LONG);
            this.finish();
            return;
        }
        mSurfaceView=(SurfaceView)findViewById(R.id.surface_onplay);
        
        int w = mSurfaceViewHeight * width / height;
        int margin = (mSurfaceViewWidth - w) / 2;
        Log.e(TAG, "onSurfaceSizeInit margin:" + margin);
        RelativeLayout.LayoutParams lp = new RelativeLayout.LayoutParams(
                RelativeLayout.LayoutParams.MATCH_PARENT,
                RelativeLayout.LayoutParams.MATCH_PARENT);
        lp.setMargins(margin, 0, margin, 0);
        
        mSurfaceView.setLayoutParams(lp);
        surfaceHolder=mSurfaceView.getHolder();//SurfaceHolder是SurfaceView的控制接口
        surfaceHolder.addCallback(this); //
        
        Log.e(TAG, "mSurfaceView is invoke~~~");
    }
    @Override
    public void onHandler(int what, int arg1, int arg2, Object obj) {
        // TODO Auto-generated method stub
        if(obj == null){
            obj = "别闹，木有，别打印了";
        }   

        Log.e("MP4 Activity", "what="+what+", arg1="+arg1+", arg2="+arg2+", obj="+obj.toString());
        

        switch(what){
            case 0xB3://mp4一些基本信息
                String strMP4Info = obj.toString();
                JSONObject mp4Obj;
                int total_second = 0;
                try {
                    mp4Obj = new JSONObject(strMP4Info);
                    total_second = mp4Obj.optInt("length", 0);
                    int video_width = mp4Obj.optInt("width", 0);
                    int video_height = mp4Obj.optInt("height", 0);
                    onSurfaceSizeInit(video_width, video_height);
                } catch (JSONException e) {
                    // TODO Auto-generated catch block
                    e.printStackTrace();
                }                 
                tv_total.setText(playTimeFormat(total_second));
                play_bar.setMax(total_second);
                break;
            case 0xB4://播放时间进度
                play_bar.refreshDrawableState();
                tv_step.setText(playTimeFormat(arg1));
                tv_total.setText(playTimeFormat(arg2));
                play_bar.setMax(arg2);
                play_bar.setProgress(arg1);
                break;
            default:
                break;
        }
    }

    @Override
    public void onNotify(int what, int arg1, int arg2, Object obj) {
        // TODO Auto-generated method stub
        handler.sendMessage(handler.obtainMessage(what, arg1, arg2, obj));    
    }

    @Override
    protected void initSettings() {
        // TODO Auto-generated method stub

    }

    @Override
    protected void initUi() {
        // TODO Auto-generated method stub

    }

    @Override
    protected void saveSettings() {
        // TODO Auto-generated method stub

    }

    @Override
    protected void freeMe() {
        // TODO Auto-generated method stub
        Log.e(TAG, "freeMe ondestroy is invoke~~~");
        Jni.Mp4Release();
    }

    @Override
    public void onClick(View v) {
        // TODO Auto-generated method stub
        switch(v.getId()){
            case R.id.btn_pause:
                if(!bpause){
                    bpause = true;
                    Jni.Mp4Pause();
                    pause.setText("继续");
                }
                else{
                    bpause = false;
                    Jni.Mp4Resume();
                    pause.setText("暂停");                    
                }
             break;
        }        
    }

    @Override
    public void surfaceChanged(SurfaceHolder holder, int format, int width, int height) {
        // TODO Auto-generated method stub
        Log.e(TAG, "surfaceChanged is invoke~~~");
        Jni.Mp4Start(holder.getSurface());
    }

    @Override
    public void surfaceCreated(SurfaceHolder holder) {
        // TODO Auto-generated method stub
        Log.e(TAG, "surfaceCreated is invoke~~~");
        //Jni.Mp4Start(surfaceHolder.getSurface());
    }

    @Override
    public void surfaceDestroyed(SurfaceHolder holder) {
        // TODO Auto-generated method stub
        Log.e(TAG, "surfaceDestroyed is invoke~~~");
    }

    @Override
    public void onProgressChanged(SeekBar seekBar, int progress, boolean fromUser) {
        // TODO Auto-generated method stub
        
    }

    @Override
    public void onStartTrackingTouch(SeekBar seekBar) {
        // TODO Auto-generated method stub
        
    }

    @Override
    public void onStopTrackingTouch(SeekBar seekBar) {
        // TODO Auto-generated method stub
        
    }

}
