# 文件树结构

+ `depends`: 解码、显示、网络库等依赖头文件
+ `libs`: 依赖静态库文件
+ `defines.h`: 全局定义头文件
+ `play.*`: 应用层对应接口及实现
+ `utils/commons.*`: 底层通用方法
+ `utils/callbacks.*`: 网络库回调函数
+ `utils/threads.*`: 内部线程函数

# 回调说明

## ConnectChange

    id, window, type, json
    json: {"msg":"", "data":0}

## PlayData

    id, window, type, json
    json: {"count":0,
        "audio_type":0, "audio_sample_rate":0, "audio_bit":0, "audio_channel":0,
        "width":0, "height":0, "fps":0.0, "total":0}

    caution: type == PLAYOVER/PLAYE/PLTIMEOUT doesn't mean play stopped!

## NormalData

    id, window, type, json
    json: {"is05":true, "fps":0.0, "device_type":0, "start_code":0,
        "reserved":0, "width":0, "height":0, "auto_stop_recorder":false,
        "audio_type":0, "audio_enc_type":0,
        "audio_sample_rate":0, "audio_bit":0, "audio_channel":0}

## CheckResult

    id, window, 0, byte[]

## ChatData

    id, window, type, byte[]

## TextData

    id, window, type, json
    json: {"result":0, "msg":"", "flag":0, "type":0,
        "packet_type":0, "packet_count":0, "packet_id":0, "packet_length":0,
        "extend_type":0, "extend_arg1":0, "extend_arg2":0, "extend_arg3":0,
        "wifi":
            [{"name":"", "pwd":"", "quality":0, "keystat":0, "auth":0, "env":0}]
        }

## Download

    id, window, type, NULL

## SearchLanServer

    id, 0, 0, json
    json: {"ip":"", "gid":"", "no":0, "type":0, "count":0,
        "port":0, "variety":0, "timeout":0, "netmod":0, "curmod":0}

## queryDevice

    id, 0, 0, json
    json: {"ip":"", "gid":"", "no":0, "type":0, "count":0,
        "port":0, "variety":0, "timeout":0, "netmod":0, "curmod":0}

## stat

    id, 0, 0, jsonArray
    jsonArray: [{"window":0, "delay":0.0, "kbps":0.0,
        "audio_type":1, "audio_kbps":0.0,
        "audio_network_fps":0.0, "audio_decoder_fps":0.0,
        "audio_decoder_delay":0.0, "audio_play_delay":0.0, 
        "network_fps":0.0, "decoder_fps":0.0, "jump_fps":0.0,
        "decoder_delay":0.0, "render_delay":0.0,
        "space":0, "width":0, "height":0, "left":0,
        "is_omx":false, "is_turn":false, "is_playback":false}]

## play

    // screenshot
    id, window, result, null

    // frame I report
    id, window, try_omx, null

    // play doomed
    id, window, bad_status, null
    id, window, playback_done, null
    id, window, hdec_buffering, null

    // play audio
    id, window, is_chat_data, byte[]
    byte: pcm raw data

# 主控设备参数

## DVR - JFH(16bit alaw/ulaw), or 8bit PCM -8

+ `JVN_DSC_DVR`
+ `DVR8004_STARTCODE`

## card - JFH(16bit amr), or 8bit PCM -8

+ `JVSC950_STARTCODE`
+ `JVSC951_STARTCODE`: 16bit PCM
+ `JVN_DSC_960CARD`: 16bit amr
+ others: 8bit PCM

+ `JVN_DSC_CARD`: 04
+ `JVN_DSC_9800CARD`: 04

## IPC - JFH 16bit alaw/ulaw

+ `IPC3507_STARTCODE`
+ `IPC_DEC_STARTCODE`

## NVR, IPCs

+ `JVN_NVR_STARTCODE`

# 延迟统计

+ searchLanDevice -> `JVC_MOLANSerchDevice`: ms
+ queryDevice -> `JVC_QueryDevice`: ms
+ getChannelCount -> `JVC_WANGetChannelCount`: s
+ isDeviceOnlin -> `JVC_GetYSTStatus`: s

# 现阶段已实现

+ 添加硬解接口 -- New
+ 优化编译配置流程 -- New
+ 优化统计线程和方案 -- New
+ 优化播放库框架 -- New
+ 统一应用层通道和窗口索引下标 -- New
+ 添加与设备通信特殊通道处理 -- New
+ 添加统计数据回调给应用层 -- New
+ 添加硬解失败切换软件机制 -- New
+ 实现远程回放与视频播放参数以及数据分离 -- New
+ 添加音频双缓冲队列和解码线程 -- New
+ 实现音视频录像 -- New
+ 优化硬解自动调整分辨率，放弃，重新打开 -- New
+ 实现硬解自动开关 -- New
+ 添加远程回放结束后通知 -- New
+ 优化截图画面 -- New

+ 实现网络库基本接口
+ 添加新的播放缓冲队列结构
+ 添加软解库，处理缓冲队列中的数据
+ 实现多通道显示
+ 添加统计线程
+ 实现帧率控制
+ 添加外部缓解播放延迟接口
+ 优化网络库调用接口
+ 添加录像接口
+ 整理输出日志
+ 整理全局句柄，规范初始化、调用以及销毁
+ 添加内部误操作判断
+ 添加应用层多窗口管理类
+ 实现横竖屏切换后自动显示播放
+ 完善应用层多窗口管理类
+ 添加播放库黑屏接口
+ 添加截图接口
+ 添加静态音频编码库
+ 优化播放界面接口调用机制
+ 优化音频编码库
+ 优化音频类型判断和解码流程

# TODO

+ 低版本通测显示

# 流畅播放

在数据源稳定的情况下，举个 25 帧视频的例子，正常情况下每秒有 25 个不同的画面，  
每个刷新周期是 40ms(1000ms / 25)，换句话说，每 40ms 里需要拾取新的视频数据帧，  
解码并渲染刷新出来。如果这三件事儿需要用 100ms 做完，用户就会感到明显的卡顿。  
所以通过技术手段缩减解码和渲染时间是十分有必要的。另外说明一下，视频帧所携带的  
比特率和分辨率都跟解码和渲染两个模块的效率呈正相关。

除去硬件配置和不可变因素外，软件也是有很多提升空间的：

## 缓冲

如果有一个新的视频帧，解一次码，渲染一次，再等下一个新的视频帧，继续这样子  
循环。代码的确很容易写，但效果非常糟糕。而且还会带来另外一个不想看到的问题，  
那就是延迟。所以在重构时，最先优化的就是缓冲队列。网络库只要来了视频帧，就  
丢给缓冲区，保证快速有效的接收新数据，回调也不阻塞网络。与此同时，解码器在  
另外一个时空去读取缓冲区的内容，依次进行解码和渲染显示。

## 帧率控制

通过以上介绍的优化方案，基本上可以比较正常的播放了。但是网络不稳定或者视频帧  
变化大时，缓冲区会逐渐膨胀，继而出现延迟或卡顿。为了流畅播放，还需要额外进行  
帧率控制，这也手机端实现流畅播放的核心。现阶段的算法如下：

1. 将帧率计算出的刷新周期与当前解码和渲染周期做差，如果差值大于零，那么  
在下次循环开始之前强制休眠这段时间，可粗略控制播放速度；

2. 在解码之前，判断缓冲队列中视频帧是否大于两倍的帧率，如果符合条件，那么  
依次丢弃 P 帧，播放 I 帧，直到满足缓冲区个数小于帧率数值又刚好拾取到  
I 帧，恢复正常解码和渲染所有视频帧。这样子可以缓解延迟的现象；

3. 开始播放视频之后，每隔一秒钟统计一次当前网络接收到的 fps、bps，以及  
解码渲染的帧个数以及缓冲队列剩余数量。另外根据主控定义的 O 帧以及实际的  
帧间隔进行权衡，给出一个参考播放帧率，并计算出指定周期的平均值，应用在  
算法 I 中。通过这半个月的统计观察，在网络畅通的情况下，基本消除了视频  
卡顿和延迟的问题，普通安卓手机可软解流畅播放 25 帧 720p 的家用设备。  
较之前只能支持 15 帧 720p 播放结果有了很大改善。

4. 暴漏给应用层一个跳帧的接口，可以在播放的任何时刻调用。处理的机制大致  
与算法 II 中介绍的相同，只播放 I 帧，直到缓冲区剩余量小于帧率数值后，  
恢复全帧播放。因为统计线程的汇报也会给应用层，程序可在不修改底层播放库  
的前提下，直接在应用层对视频帧进行自定义的算法调节。

## 面向接口编程

队列

    class queue<T>

    // [Neo] 判空
    bool empty();
    // [Neo] 大小
    size_t size();
    // [Neo] 入队
    void push(T data);
    // [Neo] 出队
    T pop();

数据帧

    struct frame {
        int size;
        bool is_i_frame;
        unsigned char* buf;
    };

    typedef queue<frame*> buffer_queue;

    void offer(buffer_queue* queue, unsigned char* buf, int size, bool is_i_frame) {
        size_t s = sizeof(frame);
        frame* f = (frame*) malloc(s);
        memset(f, 0, s);

        if (size > 0) {
            f->size = size;
            f->is_i_frame = is_i_frame;
            f->buf = (unsigned char*) malloc(sizeof(unsigned char) * size);
            memcpy(f->buf, buf, size);
        } else {
            f->size = 0;
            f->buf = NULL;
            f->is_i_frame = false;
        }

        queue->push(f);
    }

    frame* poll(buffer_queue* queue) {
        frame* f = NULL;

        if(has_connected) {
            sem_wait();
        }

        f = queue->pop();

        return f;
    }

接收回调

    onCallBack(int type, unsigned char* data, int size) {
        offer(queue, data, size, (type == ID_FRAME_I));
    }

播放线程

    void* onPlay() {
        bool need_jump = true;
        while(has_connected) {
            frame* f = poll(queue);

            if(NULL == f || NULL == f->buf) {
                continue;
            }

            if(queue->size() > fps) {
                need_jump = true;
            }

            if(need_jump && false == f->is_i_frame) {
                // [Neo] 统计跳帧
            } else {
                need_jump = false;
                // [Neo] 解码、显示
            }

            // [Neo] 释放
            if(queue->size() > 0 && NULL != f) {
                if(NULL != f->buf) {
                    free(f->buf);
                }
                free(f);
            }

        }
    }

