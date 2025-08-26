#pragma once

#include <QObject>
#include <QTime>
#include "qc_log.h"
#include <QEventLoop>
//#include "rs_motion.h"
#include "datas.h"
#include "rs.h"

class Thread : public QObject
{
    Q_OBJECT

public:
    explicit Thread(QObject* parent = nullptr);
    /* 退出标志 */
    bool bExit{ false };
    bool m_confirmed = false;

public slots:
    //监控信号
    void triggerMotionTpyes();
    //位置，DIDO监控
    void globalTpyes();
    //相机采集
    void cameraGrabbing();
    //相机采集
    void cameraGrabbing_3D();
    //下层倍速链动作
    void doubleSpeedDownAction();
    //机器人皮带取料
    void robotLoadingAction();
    //机器人皮带下料
    void robotUnLoadingAction(bool isNG);
    void thread_showPin_slot(ResultParamsStruct_Pin result, PointItemConfig config);    //单独2Dpin针
    void thread_showPin2Dand3D_slot(ResultParamsStruct_Pin result2D, PointItemConfig config2D, ResultParamsStruct_Pin3DMeasure result3D, PointItemConfig config3D);    //2D和3Dpin针
    void thread_showPin3D_slot(ResultParamsStruct_Pin3DMeasure result, PointItemConfig config);    //单独3Dpin针
    //视觉功能的纯结果显示视图
    void thread_showVision_slot(Vision_Message message);
    // 显示圆测距的图片
    void thread_showCircleMeasure_slot(ResultParamsStruct_Circle cir1, ResultParamsStruct_Circle cir2, PointItemConfig config, double dis, bool isNG);

    void notifyDialogClosed(bool confirmed) {
        emit dialogClosed(confirmed);
    }
    //直接显示报错NG的图片
    void thread_showNG_slot(HObject photo, PointItemConfig config, bool isNG = false);
    //直接显示图片
    void thread_showImage_slot(QImage img);
signals:
    //刷新相机视图
    void signalFlushed();
    //切换工位表格视图
    void changeStationUI_signal();
    // 刷新控件状态
    void setControlEnable_signal(bool state);
    //按钮启动
    void di_runAction_signal();
    //按钮停止
    void di_stopAction_signal();
    //按钮复位
    void di_resetAction_signal();
    //清除图片展示
    void clearWindow_signal();
    //触发机器人下料
    void robotUnload_signal(bool isNG);
    //出料皮带满料报警 num 0 :出料皮带满，num 1:NG皮带满
    //void robotUnloadError_signal(int num,const QString& message);
    void confirmationRequested(const QString& message); // 弹窗请求信号
    void confirmationResult(bool confirmed);            // 用户操作结果信号
    void dialogClosed(bool confirmed);

    void thread_showPin_signal(ResultParamsStruct_Pin result, PointItemConfig config);    //单独2Dpin针
    void thread_showPin2Dand3D_signal(ResultParamsStruct_Pin result2D, PointItemConfig config2D, ResultParamsStruct_Pin3DMeasure result3D, PointItemConfig config3D);    //2D和3Dpin针
    void thread_showPin3D_signal(ResultParamsStruct_Pin3DMeasure result, PointItemConfig config);    //单独3Dpin针
    //视觉功能的纯结果显示视图
    void thread_showVision_signal(Vision_Message message);
    // 显示圆测距的图片
    void thread_showCircleMeasure_signal(ResultParamsStruct_Circle cir1, ResultParamsStruct_Circle cir2, PointItemConfig config, double dis, bool isNG);
    // 配方运行完成CT时间
    void showCTtime_signal(QString time);
    //直接显示报错NG的图片
    void thread_showNG_signal(HObject photo, PointItemConfig config, bool isNG = false);
    //直接显示图片
    void thread_showImage_signal(QImage img);
};

