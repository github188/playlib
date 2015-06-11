package neo.droid.cloudsee.activities;

import android.content.Intent;
import android.content.pm.ActivityInfo;
import android.os.AsyncTask;
import android.os.Bundle;
import android.os.Environment;
import android.os.Handler;
import android.os.Message;
import android.text.Html;
import android.util.DisplayMetrics;
import android.util.Log;
import android.view.GestureDetector;
import android.view.KeyEvent;
import android.view.MotionEvent;
import android.view.SurfaceHolder;
import android.view.SurfaceView;
import android.view.View;
import android.view.Window;
import android.view.WindowManager;
import android.view.GestureDetector.SimpleOnGestureListener;
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
    
    private SurfaceView mSurfaceView = null;
    private SurfaceHolder surfaceHolder;
    private Button pause;
    private boolean bpause = false;
    private TextView tv_step, tv_total;
    private SeekBar play_bar;
    private int mSurfaceViewWidth, mSurfaceViewHeight;
    private static final String TAG = "OnPlayMP4Activity";
    private boolean IsVideoReadyToBePlayed = false;
    private int mStopSeconds = 0;
    private long exitTime = 0;
    private boolean bShowGoonPlayToast = false;
    private GestureDetector mGestureDetector = null;
    private boolean bar_visibility = true;
    private RelativeLayout play_control_layout = null;
    private View LoadingLayout; //加载界面
    public static final int FLAG_HOMEKEY_DISPATCHED = 0x80000000; //需要自己定义标志
    Handler athandler = new Handler();
    private long last_show_seconds = 0;
    private OnPlayMP4Activity mActivity;
    private static int surface_status = 0;//默认没创建
    private boolean bStartPlay = false;
    String mp4_uri = "";
    boolean isLocal = true;
    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        requestWindowFeature(Window.FEATURE_NO_TITLE);
        getWindow().setFlags(WindowManager.LayoutParams.FLAG_FULLSCREEN,
                WindowManager.LayoutParams.FLAG_FULLSCREEN);       
        this.getWindow().setFlags(FLAG_HOMEKEY_DISPATCHED, FLAG_HOMEKEY_DISPATCHED);//关键代码
        setContentView(R.layout.onplay_mp4_layout);
        
        Intent intent = getIntent();
        if (null != intent) {
            mp4_uri = intent.getStringExtra("URL");
            isLocal = intent.getBooleanExtra("IS_LOCAL", true);
        }
        if(mp4_uri == null || "".equals(mp4_uri)){
            finish();
            return;
        }
        //Jni.init(getApplication(), 9200, Consts.LOG_PATH);
        // [Neo] TODO
        //Jni.enableLog(true);   
        mActivity = this;
        
        Jni.Mp4Init();      
        //SD_CARD_PATH+"CSAlarmVOD/848x480_fps25_h264_alaw.mp4"
        Jni.SetMP4Uri(mp4_uri);       
        
        pause=(Button)findViewById(R.id.btn_pause);
        play_control_layout = (RelativeLayout)findViewById(R.id.play_control_bar);
        play_bar = (SeekBar) findViewById(R.id.seekbar_def);
        play_bar.setOnSeekBarChangeListener(this);
        play_bar.setEnabled(false);//一开始先不支持拖动
        
        tv_step = (TextView)findViewById(R.id.tv_step);
        tv_total = (TextView)findViewById(R.id.tv_total);
        tv_step.setText("00:00");
        tv_total.setText("00:00");
        LoadingLayout = (View)findViewById(R.id.loading);
        pause.setOnClickListener(this);
        pause.setText("暂停");               
        mGestureDetector = new GestureDetector(this, new MyGestureListener());
        
    }
//    @Override
//    public boolean onKeyDown( int keyCode,KeyEvent event) {
//           // TODO Auto-generated method stub
//           if (keyCode == event.KEYCODE_HOME) {
//               if(LoadingLayout.getVisibility() == View.VISIBLE){
//                   Log.e(TAG, "---------------此时屏蔽home键---------------");
//                   return false;
//               }
//          }
//          return super.onKeyDown(keyCode, event);
//    }    
    @Override
    protected void onResume(){
        //从xml里设置横屏，这里动态设置的话， 有时候surfaceview会有问题
        //setRequestedOrientation(ActivityInfo.SCREEN_ORIENTATION_LANDSCAPE);
        getWindow().getDecorView().setKeepScreenOn(true);
        play_control_layout.setVisibility(View.VISIBLE);
        last_show_seconds = System.currentTimeMillis();
        bar_visibility = true;
        bStartPlay = false;
        DismissBar();
        DisplayMetrics dm = new DisplayMetrics();
        getWindowManager().getDefaultDisplay().getMetrics(dm);
        mSurfaceViewWidth = dm.widthPixels;
        mSurfaceViewHeight = dm.heightPixels;
        //准备播放
        PreparePlayTask task = new PreparePlayTask();
        String[] params = new String[3];
        task.execute(params);        
         
        super.onResume();
    }
    @Override
    protected void onPause(){
        if(mStopSeconds > 0){
            bShowGoonPlayToast = true;
        }
        else{
            bShowGoonPlayToast = false;
        }
        Jni.Mp4Stop(mStopSeconds);
        super.onPause();
    }
    @Override
    public void onBackPressed() {
        if ((System.currentTimeMillis() - exitTime) > 2000) {
            Toast.makeText(getApplicationContext(), "再次按下返回键退出",
                    Toast.LENGTH_SHORT).show();
            exitTime = System.currentTimeMillis();
        } else {
              mStopSeconds = 0;//停止时间注意置0
              finish();  
        }
    }
    private String playTimeFormat(int in_seconds){
        int play_minute = in_seconds/60;
        int play_second = in_seconds - play_minute*60;
        
        String str_minute = String.format("%02d", play_minute);
        String str_second = String.format("%02d", play_second); 
        
        String str_format_res = str_minute+":"+str_second;
        
        return str_format_res;
    }
    class PreparePlayTask extends AsyncTask<String, Integer, Integer> {

        @Override
        protected Integer doInBackground(String... arg0) {
            // TODO Auto-generated method stub
            int ret = -1;
            int try_cnt = 2;
            do{
                ret = Jni.Mp4Prepare();   
                if(ret == 0){
                    Log.e("MP4", "Jni.Mp4Prepare success:"+ret);
                    IsVideoReadyToBePlayed = true; 
                    int check_surface_cnt = 10;
                    while(surface_status==0){
                        try {
                            Thread.sleep(200);
                        } catch (InterruptedException e) {
                            // TODO Auto-generated catch block
                            e.printStackTrace();
                        }    
                        check_surface_cnt--;
                        if(check_surface_cnt < 0){
                            Log.e(TAG, "检测surfaceview状态超时...");
                            break;
                        }
                    }
                    if(surface_status == 1){
                        int ret2 = Jni.Mp4Start(surfaceHolder.getSurface());
                        if(ret2 != 0){   
                            Log.e(TAG, "Mp4Start failed, ret:"+ret2);
                            ret = -100;    
                            break;
                        }
                        else{
                            Log.e(TAG, "Mp4Start OK, ret:"+ret2);
                            ret = ret2;
                            break;
                        }
                    }
                    else{
                        ret = -99;
                        break;
                    }
                }
                else if(ret == 1){
                    //正在播放，需要先停止
                    IsVideoReadyToBePlayed = false;
                    Log.e(TAG, "视频正在播放...");
                    return ret;
                }
                else if(ret == 2){
                    Log.e(TAG, "视频播放线程正在停止，稍后再运行...");
                    IsVideoReadyToBePlayed = false;
                    //延时2秒，重新准备
                    try {
                        Thread.sleep(2000);
                        try_cnt--;
                        if(try_cnt < 0){
                            break;
                        }
                    } catch (InterruptedException e) {
                        // TODO Auto-generated catch block
                        e.printStackTrace();
                    }
                }
            }while(ret==2);
         
            return ret;
        }    
        @Override
        protected void onPreExecute() {
            LoadingLayout.setVisibility(View.VISIBLE);
        }
        
        @Override
        protected void onCancelled() {
            super.onCancelled();
        }

        @Override
        protected void onPostExecute(Integer result) {  
            if(result < 0)
            {
                Log.e("MP4", "play failed:"+result);
                IsVideoReadyToBePlayed = false;
                Toast.makeText(mActivity, "play failed:"+result, Toast.LENGTH_SHORT).show();
                mActivity.finish();            
            } 
            else{
                if(result == 0){
                    Log.e("MP4", "pre and play success:"+result);
                }
                else if(result == 1){
                    //正在播放，需要先停止
                    IsVideoReadyToBePlayed = false;
                    Log.e(TAG, "视频正在播放...");
                    Toast.makeText(mActivity, "视频正在播放中...", Toast.LENGTH_LONG).show();
                }
                else if(result == 2){
                    Log.e(TAG, "视频播放线程正在停止，稍后再运行...");
                }
            }             
        }
    }
    private void onSurfaceSizeInit(int width, int height){
        if (width == 0 || height == 0) {
            Log.e(TAG, "invalid video width(" + width + ") or height(" + height
                    + ")");
            return;
        }

        Log.e(TAG, "video width(" + width + ") or height(" + height
                + ")"+"screen width:"+mSurfaceViewWidth+", screen height:"+mSurfaceViewHeight);        
        //if(mSurfaceView == null){
            
            mSurfaceView=(SurfaceView)findViewById(R.id.surface_onplay);
            
            int w = mSurfaceViewHeight * width / height;
            int margin = (mSurfaceViewWidth - w) / 2;
            Log.e(TAG, "onSurfaceSizeInit margin:" + margin);
            RelativeLayout.LayoutParams lp = new RelativeLayout.LayoutParams(
                    RelativeLayout.LayoutParams.MATCH_PARENT,
                    RelativeLayout.LayoutParams.MATCH_PARENT);
            if(margin > 50){
                lp.setMargins(margin, 0, margin, 0);
            }
            else{
                margin = 2;//这里margin不能为0，如果为0,surfaceview创建不成功，不知道为啥
                lp.setMargins(margin, 0, margin, 0);
            }
            
            mSurfaceView.setLayoutParams(lp);
            surfaceHolder=mSurfaceView.getHolder();//SurfaceHolder是SurfaceView的控制接口
            surfaceHolder.addCallback(this); //
            
            Log.e(TAG, "mSurfaceView is invoke~~~");
       
        //}
    }
    private class MyGestureListener extends SimpleOnGestureListener {
        /** 双击 */
        @Override
        public boolean onDoubleTap(MotionEvent e) {
            return true;
        }
        /** 单击 */
        @Override
        public boolean onSingleTapConfirmed(final MotionEvent e) {
            // TODO Auto-generated method stub
            if(bar_visibility){
                play_control_layout.setVisibility(View.GONE);
                bar_visibility = false;
            }
            else{
                bar_visibility = true;
                play_control_layout.setVisibility(View.VISIBLE);
                last_show_seconds = System.currentTimeMillis();
            }
            //return super.onSingleTapConfirmed(e);
            return true;
        }
    }    
    @Override
    public void onHandler(int what, int arg1, int arg2, Object obj) {
        // TODO Auto-generated method stub
        if(obj == null){
            obj = "";
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
                //dismissDialog();
                LoadingLayout.setVisibility(View.GONE);
                mStopSeconds = arg1;
                if(bShowGoonPlayToast){
                    String strRestartTime = playTimeFormat(mStopSeconds);                    
                    Toast.makeText(this, "从"+strRestartTime+"处继续播放", Toast.LENGTH_SHORT).show();;                    
                    bShowGoonPlayToast = false;
                }                    
                tv_step.setText(playTimeFormat(arg1));
                tv_total.setText(playTimeFormat(arg2));
                play_bar.setProgress(arg1);
                break;
            case 0xB5://播放结束
                mStopSeconds = 0;//停止时间注意置0
                finish();
                break;
            case 0xB6://播放失败
                Toast.makeText(this, obj.toString(), Toast.LENGTH_SHORT).show();
                mStopSeconds = 0;
                finish();
                break;     
            default:
                break;
        }
    }
    
    private void DismissBar() {
        runOnUIThread(new Runnable() {
            public void run() {
                long current_time = System.currentTimeMillis();
                long showed_seconds = current_time - last_show_seconds;
                if(showed_seconds >= 3000){
                    play_control_layout.setVisibility(View.GONE);  
                    runOnUIThread(this, 3000);
                }
                else{
                    runOnUIThread(this, (int)(3000-showed_seconds));
                }
                
            }
        }, 3000);
    }
    private void runOnUIThread(Runnable runnable, int i) {
        // TODO Auto-generated method stub

        athandler.postDelayed(runnable, i);
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
        surface_status = 1;
    }

    @Override
    public void surfaceCreated(SurfaceHolder holder) {
        // TODO Auto-generated method stub
        Log.e(TAG, "surfaceCreated is invoke~~~");
        surface_status = 1;
        //Jni.Mp4Start(surfaceHolder.getSurface());
    }

    @Override
    public void surfaceDestroyed(SurfaceHolder holder) {
        // TODO Auto-generated method stub
        Log.e(TAG, "surfaceDestroyed is invoke~~~");
        surface_status = 0;
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
    @Override
    public boolean onTouchEvent(MotionEvent event) {
        if (mGestureDetector.onTouchEvent(event))
            return true;
        return super.onTouchEvent(event);
    }
}
