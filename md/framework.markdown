# 安卓手机端应用系统架构

面对用户对手机软件要求的不断提升，以及新网络库、播放库和解码库的不断提升，  
手机端软件正在吐火如荼的进行着整体的重构优化工作。

新的架构如下图所示：

![framework](framework.png)

+ `MainApplication` 负责全局的消息分发，包括底层所有回调和应用层活动之间  
的通信。除此之外，它还保存当前所显示的活动引用，以便快速有效的将消息以及  
控制信息传递给界面；另外值得一提的是，应用当前运行环境的状态信息统一在  
这里管理，摆脱一直使用静态变量的问题。

+ 统一的消息通知接口 `IHandlerLikeNotify`，将消息使用统一的形式发送给它  
的实现对象。这样也大大的简化了接口繁多，参数对不上等一系列问题。

+ 全局常量、Jni 接口等都定义在 `cloudsee` 包中，作为应用整体的定义管理。  
集合类都在 `cloudsee.beans` 包中。活动类都在 `cloudsee.activities` 包中，  
所有的活动都继承自 `BaseActivity`，在基类中指定了四个标准方法，初始化配置、  
界面，保存配置以及销毁释放，还有就是强制其派生类实现 `onNotify` 接口，  
保证消息送达。

+ 通用方法都在 `cloudsee.commons` 包中，细分 `net`，`resource` 等子包，  
作为轻量级的开发工具包，可快速开发以后的新应用。

+ 底层库接口统一写在 `Jni` 类中，回调统一发送给  
`MainApplication.onJniNotify`，对应着的 NDK 层的目录结构也做出了如下调整：

    + `depends`: 解码、显示、网络库等依赖头文件
    + `libs`: 所有依赖的动、静态库文件
    + `json`: jsoncpp 源码，不需要重复编译，`depends` 中的静态库为 armebai 架构
    + `network`: 网络库头文件，不需要引用，已封装在 `depends/net_*.h` 文件中
    + `defines.h`: 全局的宏定义头文件
    + `play.*`: 应用层对应接口
    + `utils/commons.*`: 底层用到的通用方法
    + `utils/callbacks.*`: 网络库回调函数
    + `utils/threads.*`: 底层用到的线程函数

