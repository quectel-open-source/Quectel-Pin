#pragma once
#pragma execution_character_set("utf-8")
#include <QObject>
#include "rs.h"
class LS_Motion :
    public QObject
{
public:
    //���ƿ���ʼ��
    bool motionInit();
    //canIO��ʼ��
    bool CANInit(int CardId, int NodeId);
    bool ReadAxisServeOn(int CardId, int AxisId);
    short WriteAxisServeOn(int CardId, int AxisId, bool isEnable);
    void WriteAxis(int CardId, int AxisId,
        double StartVel, double RunVel,
        double AccTime, double DccTime, double StopVel,
        double STime,int homeDir, int stopIO);
    void MapEmgIO(int CardId, int AxisId, int IOindex, ushort validlevel);
    void motionClose();
    void AxisStop(int CardId, int AxisId);
    void AxisEmgStop(int CardId);
    void CANClose(int CardId);
    void GoHome(int CardId, int AxisId);
    void AxisClearPosition(int CardId, int AxisId);
    bool IsRuning(int CardId, int AxisId);
    void AxisStopAll();
    void VMove(int CardId, int AxisId, bool pdir = true);

    double ReadPosition(int CardId, int AxisId);

    void WriteAxisRealSpeed(int CardId, int AxisId, double RealSpeed);


    // ��ȡ�ᱨ���˿ڵ�ƽ
    bool isAxisErc(int CardId, int AxisId);

    void AxisClear(int CardId, int AxisId);
    // ���������˶�
    int PMove_abs(int CardId, int AxisId, int value);
    // ��������˶�
    void PMove_rel(int CardId, int AxisId, int value);
};

