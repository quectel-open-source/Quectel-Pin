#pragma once
#pragma execution_character_set("utf-8")

#include <QObject>
//#include <opencv2/opencv.hpp>
#include <opencv2/core/core.hpp>
//#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <Halcon.h>
#include <halconcpp/HalconCpp.h>
#include <QImage>

using namespace HalconCpp;
using namespace cv;
using namespace std;
class ImageTypeConvert  : public QObject
{
	Q_OBJECT

public:
	ImageTypeConvert();
	~ImageTypeConvert();
	static QImage Mat2QImage(const cv::Mat &mat);
	static HObject Mat2HObject(const cv::Mat &image);
	static Mat HObject2Mat(HObject image);
	static cv::Mat QImage2Mat(QImage image);
	//QImage HObjectToQImage(HObject himage);
	//int QImageToHObjbect(QImage image, HObject &Object);
};
