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
    /* �˳���־ */
    bool bExit{ false };
    bool m_confirmed = false;

public slots:
    //����ź�
    void triggerMotionTpyes();
    //λ�ã�DIDO���
    void globalTpyes();
    //����ɼ�
    void cameraGrabbing();
    //����ɼ�
    void cameraGrabbing_3D();
    //�²㱶��������
    void doubleSpeedDownAction();
    //������Ƥ��ȡ��
    void robotLoadingAction();
    //������Ƥ������
    void robotUnLoadingAction(bool isNG);
    void thread_showPin_slot(ResultParamsStruct_Pin result, PointItemConfig config);    //����2Dpin��
    void thread_showPin2Dand3D_slot(ResultParamsStruct_Pin result2D, PointItemConfig config2D, ResultParamsStruct_Pin3DMeasure result3D, PointItemConfig config3D);    //2D��3Dpin��
    void thread_showPin3D_slot(ResultParamsStruct_Pin3DMeasure result, PointItemConfig config);    //����3Dpin��
    //�Ӿ����ܵĴ������ʾ��ͼ
    void thread_showVision_slot(Vision_Message message);
    // ��ʾԲ����ͼƬ
    void thread_showCircleMeasure_slot(ResultParamsStruct_Circle cir1, ResultParamsStruct_Circle cir2, PointItemConfig config, double dis, bool isNG);

    void notifyDialogClosed(bool confirmed) {
        emit dialogClosed(confirmed);
    }
    //ֱ����ʾ����NG��ͼƬ
    void thread_showNG_slot(HObject photo, PointItemConfig config, bool isNG = false);
    //ֱ����ʾͼƬ
    void thread_showImage_slot(QImage img);
signals:
    //ˢ�������ͼ
    void signalFlushed();
    //�л���λ�����ͼ
    void changeStationUI_signal();
    // ˢ�¿ؼ�״̬
    void setControlEnable_signal(bool state);
    //��ť����
    void di_runAction_signal();
    //��ťֹͣ
    void di_stopAction_signal();
    //��ť��λ
    void di_resetAction_signal();
    //���ͼƬչʾ
    void clearWindow_signal();
    //��������������
    void robotUnload_signal(bool isNG);
    //����Ƥ�����ϱ��� num 0 :����Ƥ������num 1:NGƤ����
    //void robotUnloadError_signal(int num,const QString& message);
    void confirmationRequested(const QString& message); // ���������ź�
    void confirmationResult(bool confirmed);            // �û���������ź�
    void dialogClosed(bool confirmed);

    void thread_showPin_signal(ResultParamsStruct_Pin result, PointItemConfig config);    //����2Dpin��
    void thread_showPin2Dand3D_signal(ResultParamsStruct_Pin result2D, PointItemConfig config2D, ResultParamsStruct_Pin3DMeasure result3D, PointItemConfig config3D);    //2D��3Dpin��
    void thread_showPin3D_signal(ResultParamsStruct_Pin3DMeasure result, PointItemConfig config);    //����3Dpin��
    //�Ӿ����ܵĴ������ʾ��ͼ
    void thread_showVision_signal(Vision_Message message);
    // ��ʾԲ����ͼƬ
    void thread_showCircleMeasure_signal(ResultParamsStruct_Circle cir1, ResultParamsStruct_Circle cir2, PointItemConfig config, double dis, bool isNG);
    // �䷽�������CTʱ��
    void showCTtime_signal(QString time);
    //ֱ����ʾ����NG��ͼƬ
    void thread_showNG_signal(HObject photo, PointItemConfig config, bool isNG = false);
    //ֱ����ʾͼƬ
    void thread_showImage_signal(QImage img);
};

