# ArduPilotLog 开源说明
概要
---
ArduPilotLog 是 ardupilot 日志数据绘图软件。为快速展示 Log 日志数据，以便通过分析数据排查程序 bug 或累积经验而设计。

打开"_\*.bin_"类型日志
---
打开方法如下图所示：
<br/>![ArduPilotLog_1_OpenLog.png](http://qiniu.suwp.fun/ArduPilotLog_1_OpenLog.png)</br>
<br/>![ArduPilotLog_2_OpenLog.png](http://qiniu.suwp.fun/ArduPilotLog_2_OpenLog.png)</br>

三种绘图方法
---
1. 点选绘图
> “点选绘图”在不明确看哪一种日志数据时使用。
> 比如：对于飞机的某种奇怪表现，往往不能立刻明确是哪里出了问题，这时怀疑对象泛围大，用点选方式为猜测快速提供依据。

![ArduPilotLog_3_ClickPlot.png](http://qiniu.suwp.fun/ArduPilotLog_3_ClickPlot.png)

2. 数据分析窗（点选绘图Plus）
> “数据分析窗”在小泛围锁定目标数据时使用。
> 比如：通过点选绘图已初步锁定某种现象与某几类数据表现相关`（注：少于10类）`，此时须要精确绘图。

<br/>打开“数据分析窗”</br>
<br/>![ArduPilotLog_4_DataAnalyze.png](http://qiniu.suwp.fun/ArduPilotLog_4_DataAnalyze.png)</br>
<br/>“数据分析窗”说明</br>
<br/>![ArduPilotLog_5_DataAnalyze.png](http://qiniu.suwp.fun/ArduPilotLog_5_DataAnalyze.png)</br>
<br/>通过“数据分析窗”绘图</br>
<br/>![ArduPilotLog_6_DataAnalyze.jpg](http://qiniu.suwp.fun/ArduPilotLog_6_DataAnalyze.jpg)</br>

3. 脚本绘图
> “脚本绘图”用于反复验证已锁定的目标数据是否合理的情形。
> 将与当前问题有关的数据锁定后，往往需要多次采集 Log 日志，重复分析以便证实。这时使用脚本绘图比较方便，以免去每次点选、调整比例等麻烦。

<br/>加载脚本</br>
<br/>![ArduPilotLog_7_ScriptPlot.png](http://qiniu.suwp.fun/ArduPilotLog_7_ScriptPlot.png)</br>
<br/>![ArduPilotLog_8_ScriptPlot.png](http://qiniu.suwp.fun/ArduPilotLog_8_ScriptPlot.png)</br>

<br/>脚本语法如下图（也可见 **[conf/ReadMe.md](https://github.com/SuWeipeng/ArduPilotLog/blob/master/conf/ReadMe.md)**）</br>
![ArduPilotLog_9_ScriptPlot.png](http://qiniu.suwp.fun/ArduPilotLog_9_ScriptPlot.png)

## 编译方法
ArduPilotLog 软件架构源于[qgroundcontrol](https://github.com/mavlink/qgroundcontrol)，因此[编译方法](https://dev.qgroundcontrol.com/en/getting_started/)与QGC相同。

## 软件的由来
<br/>起初只是为了学习QGC，QGC功能多、代码构架复杂不是一下就能看懂的。</br>
<br/>后来在研究ardupilot的过程中经常要分析Log。使用过MissionPlanner、APM Planner等软件看Log日志，有几个体验一直觉得不爽：</br>
1. MissionPlanner最难受的是有时看看日志会卡死。
2. 我更关心图线，不是很在意数据，而界面留了一部分展示数据，每次都要手动拉小数据部分。
3. “看Log日志数据”只是这些软件的“一部分”功能。也就是说每次为了看数据，必须先要打开这些软件，等待其余功能加载完毕，然后点到这个功能，浪费时间。
4. 于上位机的角度讲，QGC的界面划分和操作方式更适合我，却单单缺少查看Log日志数据这个功能。

<br/>基于以上原因，就自己写个软件搞定自己的需求吧。</br>

## 作者本人对此软件的评价
1. 这不是一个完美的软件。
2. 这个软件最初是以学习为目的而写的，没有走太完整的功能设计过程，业余时间想到哪写到哪。若要完美整合已实现的功能，就要重构了。
3. 代码适合想学QGC的朋友们看（看git的开发流程log，都是0起点开始的）。
4. 软件适合正在学习或调试ardupilot的朋友们使用。
由其是 **“脚本绘图”** 功能，**其目的就是给无形的经验累积提供一个容身之所**，让经验有形，便于自我反复推敲和与人交流。
5. 代码开发过程中经历了：QGC架构分析、用户体验设计、功能模块拆分与整合等过程，麻雀小可能还有点丑，但五脏全。

## 软件涉及到的知识点
> 跟我一样0起点想学QGC的朋友可能关心这些，在此简单例举。
1. QGC架构。
2. QT ui界面开发。
3. QT QML界面开发。
4. QT ui 与 QML 整合方法。
5. QT SQLite 数据库。
6. QT 界面与后台数据的分离与整合。
7. QT 信号与槽通信机制。
8. QT 二进制文件读写。
9. QT 文本文件读取。
10. QT 正则表达式使用。
11. ardupilot 日志文件结构。
12. qcustomplot 绘图插件。
13. QGC qCDebug()分类控制显示调试日志的方法。

## 对于更进一步的数据分析
本软件以快速显示图线为目的，对于三种绘图方式仍不能满足的数据分析需求，则须将数据导入MATLAB（诸如：给数据进行低通滤波、数据作为控制仿真的输入等需求。）
<br/>[将数据导入MATLAB的脚本点此链接](https://github.com/SuWeipeng/ArduPilotLog/tree/master/matlab)</br>

## arduplilot日志数据的交换
<br/>本软件设计的目的之一是：让不同平台能方便的使用Log日志数据。因此引入了SQLite数据库。</br>
<br/>数据的存在形式：ardupilot存入SD卡中的 \*.bin 二进制格式，转到 SQLite 数据库的 \*.db 格式。</br>
<br/>![ArduPilotLog_10_SQLite.png](http://qiniu.suwp.fun/ArduPilotLog_10_SQLite.png)</br>
* MATLAB、Excel等可通过\*.db文件获取日志内容。
* 对数据感兴趣的时候，可通过 SQLite Expert 之类软件直接展示数据内容。

## 后续的更新
本软件后续会把作者已知未实现的功能写个 feature list，已知 bug 写个 bug list。对于 feature 和 bug 的划分是软件开发纠分不清的问题，在此作者表示：后续这些只是个 list 而已，很可能不会再加入新 feature 或解决小 bug（致命 bug 除外）。

**后续更新会往以下几个方向发展：**
1. 分享一些开发过程中的心得。
2. 对某个知识点的剖析。
3. 加入新的 *.conf 绘图脚本（分享ardupilot“有形的”经验）
4. 软件开发经验分享：
1）说说软件架构是什么，为什么重要。
2）说说本软件开发的功能设计、功能模块划分与整合是怎样体现的。
3）说说本软件为提升用户体验感而做了哪些改进。
4）说说当有上位机开发需求的时候，应该怎样入手。
5）其他各种想到的值得分享的东西。

**这是一个因学习而生的项目，它未来更大的意义不是从丑小鸭进化成白天鹅，而是为同样0起点的后来人铺上一块前往更高层次的砖。**

## 喜欢交流的朋友可以加作者微信
<br/>![weixin.png](http://qiniu.suwp.fun/weixin.png)</br>
