#ifndef PINLABEL_H
#define PINLABEL_H

#include <Halcon.h>
#include <halconcpp/HalconCpp.h>
#include "datas.h"
#include <opencv2/opencv.hpp>
#include "rs.h"
#include <unordered_set>

using namespace HalconCpp;

class pinLabel
{

public:
    //explicit pinLabel(QWidget* parent = nullptr,double in_Width = 400.0,double in_Height = 400.0);
    explicit pinLabel(double in_Width = 400.0, double in_Height = 400.0);
    virtual ~pinLabel();

    // ����Halconͼ�������
    void setImage(HObject img);
    void initializeHalconWindow();
    void allPinImage(const HObject& region);
    void setResult(const ResultParamsStruct_Pin result)
    {
        //��2D
        m_message_2D = result;
    };
    void setResult(const ResultParamsStruct_Pin result2D, ResultParamsStruct_Pin3DMeasure result3D)
    {
        //2D��3D
        m_message_2D = result2D;
        m_message_3D = result3D;
    };
    void setResult(const ResultParamsStruct_Pin3DMeasure result3D)
    {
        //��3D
        m_message_3D = result3D;
    };
    //2D����NG����������
    void DrawTextNearRegion(HTuple& hHalconID,HObject ho_Region_NGPin, int index);
    //ԭͼ�ü���ֻѡȡpin������
    void CropImageAroundRegion(HObject ho_Region_AllPin, HObject& ho_CroppedImage);
    //void wheelEvent(QWheelEvent* event) override;
    void showImage();
    void generateCompositeImage();
    void convertHalconToOpenCV(HObject& ho_image, cv::Mat& opencvImage);
    bool HObject2MatImg(HalconCpp::HObject& Hobj, cv::Mat& matImg);
    //2D,3D����NG����������
    void DrawTextNearRegion_2Dand3D(HTuple& hHalconID, HObject ho_Region_AllPin, int index);
    //MatתHObject
    HObject HMatToHObject(cv::Mat image);
    // չʾ3DͼƬ
    void show3DImage();
    HObject getShowImg() {
        return ho_showImg;
    }
    //2D�����������������
    void DrawTextAllNearRegion(HTuple& hHalconID, HObject ho_Region_AllPin, std::unordered_set<int> allNgIdx);
    //2D��3D�����������������
    void DrawTextAllNearRegion_2D3D(HTuple& hHalconID, HObject ho_Region_AllPin, std::vector<double> Height_3D, std::unordered_set<int> allNgIdx);
    //չʾ�Ӿ������ͼƬ
    void showVisionImage(bool isNG);
    void showVisionImage(Vision_Message message);
    // ��ʾԲ����ͼƬ
    void showCircleMeasure(ResultParamsStruct_Circle cir1, ResultParamsStruct_Circle cir2, PointItemConfig config, double dis, bool isNG);
    //ֱ����ʾ����NG��ͼƬ
    void showNGimage(HObject photo, PointItemConfig config, bool isNG = false);
protected:
    // ��д paintEvent ������������¼�������ͼ����������ʾ
    //void paintEvent(QPaintEvent* event) override;

private:

    ResultParamsStruct_Pin m_message_2D;
    ResultParamsStruct_Pin3DMeasure m_message_3D;
    HObject m_img;  // �洢ͼ��
    HObject ho_showImg; //չʾ��ͼ
    cv::Mat m_cvShowImg;

    HTuple m_hLabelID;            // QLabel �ؼ����
    HTuple m_hHalconID = NULL;            // Halcon ��ʾ���ھ��
    int m_width;
    int m_height;
};

#endif // PINLABEL_H
