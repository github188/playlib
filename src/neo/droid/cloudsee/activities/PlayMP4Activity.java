package neo.droid.cloudsee.activities;

import android.content.Intent;
import android.os.Bundle;
import android.os.Environment;
import android.util.Log;
import android.view.SurfaceHolder;
import android.view.SurfaceView;
import android.view.View;
import android.widget.Button;
import android.view.View.OnClickListener;

import com.jovision.Jni;
import java.io.File;

import neo.droid.cloudsee.Consts;
import neo.droid.cloudsee.R;

public class PlayMP4Activity extends BaseActivity implements SurfaceHolder.Callback, View.OnClickListener {
    public static final String SD_CARD_PATH = Environment
            .getExternalStorageDirectory().getPath() + File.separator;
    static {
        System.loadLibrary("alu");
        System.loadLibrary("play");
    }
    
    public SurfaceView surface;
    SurfaceHolder surfaceHolder;
    Button play,pause,stop;
    boolean bpause = false;
    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.playmp4_layout);
        Jni.init(getApplication(), 9200, Consts.LOG_PATH);
        // [Neo] TODO
        Jni.enableLog(true);        
        
        play=(Button)findViewById(R.id.btn_play);
        play.setEnabled(false);
        pause=(Button)findViewById(R.id.btn_pause);
        stop=(Button)findViewById(R.id.btn_stop);
        surface=(SurfaceView)findViewById(R.id.surface);
 
        surfaceHolder=surface.getHolder();//SurfaceHolder是SurfaceView的控制接口
        surfaceHolder.addCallback(this); //因为这个类实现了SurfaceHolder.Callback接口，所以回调参数直接this
        //surfaceHolder.setFixedSize(320, 220);//显示的分辨率,不设置为视频默认
        //surfaceHolder.setType(SurfaceHolder.SURFACE_TYPE_PUSH_BUFFERS);//Surface类型
 
        play.setOnClickListener(this);
        pause.setOnClickListener(this);
        stop.setOnClickListener(this);
        pause.setText("暂停");
        stop.setVisibility(View.INVISIBLE);
    }
    @Override
    public void onHandler(int what, int arg1, int arg2, Object obj) {
        // TODO Auto-generated method stub
        if(obj == null){
            obj = "别闹，木有，别打印了";
        }
        Log.e("MP4 Activity", "what="+what+", arg1="+arg1+", arg2="+arg2+", obj="+obj.toString());
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
        //Jni.Mp4Release();
    }
    @Override
    public void surfaceChanged(SurfaceHolder holder, int format, int width, int height) {
        // TODO Auto-generated method stub
        
    }
    @Override
    public void surfaceCreated(final SurfaceHolder holder) {
        // TODO Auto-generated method stub
//        Jni.Mp4Init();
//        Jni.SetMP4Uri(SD_CARD_PATH+"CSAlarmVOD/848x480_fps25_h264_alaw.mp4");

        play.setEnabled(true);
    }
    @Override
    public void surfaceDestroyed(SurfaceHolder holder) {
        // TODO Auto-generated method stub
        
    }
    @Override
    public void onClick(View v) {
        // TODO Auto-generated method stub
        switch(v.getId()){
            case R.id.btn_play:
//                int ret = Jni.Mp4Prepare();   
//                if(ret != 0)
//                {
//                    Log.e("MP4", "Jni.Mp4Prepare failed:"+ret);
//                } 
//                else{
//                    Log.e("MP4", "Jni.Mp4Prepare success:"+ret);
//                    Jni.Mp4Start(surfaceHolder.getSurface());
//                }
                // 启动播放界面
                Intent intent = new Intent();
                intent.setClass(PlayMP4Activity.this,
                        OnPlayMP4Activity.class);
                startActivity(intent);                
                break;
            case R.id.btn_pause:
//                if(!bpause){
//                    bpause = true;
//                    Jni.Mp4Pause();
//                    pause.setText("继续");
//                }
//                else{
//                    bpause = false;
//                    Jni.Mp4Resume();
//                    pause.setText("暂停");                    
//                }
                break;
        }
    }

}
