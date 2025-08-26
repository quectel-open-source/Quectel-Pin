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

    // 设置Halcon图像和区域
    void setImage(HObject img);
    void initializeHalconWindow();
    void allPinImage(const HObject& region);
    void setResult(const ResultParamsStruct_Pin result)
    {
        //纯2D
        m_message_2D = result;
    };
    void setResult(const ResultParamsStruct_Pin result2D, ResultParamsStruct_Pin3DMeasure result3D)
    {
        //2D，3D
        m_message_2D = result2D;
        m_message_3D = result3D;
    };
    void setResult(const ResultParamsStruct_Pin3DMeasure result3D)
    {
        //纯3D
        m_message_3D = result3D;
    };
    //2D画出NG区域的坐标等
    void DrawTextNearRegion(HTuple& hHalconID,HObject ho_Region_NGPin, int index);
    //原图裁剪（只选取pin附近）
    void CropImageAroundRegion(HObject ho_Region_AllPin, HObject& ho_CroppedImage);
    //void wheelEvent(QWheelEvent* event) override;
    void showImage();
    void generateCompositeImage();
    void convertHalconToOpenCV(HObject& ho_image, cv::Mat& opencvImage);
    bool HObject2MatImg(HalconCpp::HObject& Hobj, cv::Mat& matImg);
    //2D,3D画出NG区域的坐标等
    void DrawTextNearRegion_2Dand3D(HTuple& hHalconID, HObject ho_Region_AllPin, int index);
    //Mat转HObject
    HObject HMatToHObject(cv::Mat image);
    // 展示3D图片
    void show3DImage();
    HObject getShowImg() {
        return ho_showImg;
    }
    //2D画出所有区域的坐标
    void DrawTextAllNearRegion(HTuple& hHalconID, HObject ho_Region_AllPin, std::unordered_set<int> allNgIdx);
    //2D和3D画出所有区域的坐标
    void DrawTextAllNearRegion_2D3D(HTuple& hHalconID, HObject ho_Region_AllPin, std::vector<double> Height_3D, std::unordered_set<int> allNgIdx);
    //展示视觉处理的图片
    void showVisionImage(bool isNG);
    void showVisionImage(Vision_Message message);
    // 显示圆测距的图片
    void showCircleMeasure(ResultParamsStruct_Circle cir1, ResultParamsStruct_Circle cir2, PointItemConfig config, double dis, bool isNG);
    //直接显示报错NG的图片
    void showNGimage(HObject photo, PointItemConfig config, bool isNG = false);
protected:
    // 重写 paintEvent 或者其他相关事件来处理图像和区域的显示
    //void paintEvent(QPaintEvent* event) override;

private:

    ResultParamsStruct_Pin m_message_2D;
    ResultParamsStruct_Pin3DMeasure m_message_3D;
    HObject m_img;  // 存储图像
    HObject ho_showImg; //展示的图
    cv::Mat m_cvShowImg;

    HTuple m_hLabelID;            // QLabel 控件句柄
    HTuple m_hHalconID = NULL;            // Halcon 显示窗口句柄
    int m_width;
    int m_height;
};

#endif // PINLABEL_H
