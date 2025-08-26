#pragma once

#include <QObject>
#include <QSettings>
#include <QVector>
#include <QDebug>
#include <QMessageBox>
#include <QFileDialog>
#include <QFileInfo>
#include <QtConcurrent>
#include <qdatetime.h>
#include <QDesktopWidget>
#include <QColorDialog>
#include <Halcon.h>
#include <halconcpp/HalconCpp.h>
#include <opencv2/opencv.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc.hpp>


class Common  : public QObject
{
	Q_OBJECT

public:
	Common();
	~Common();
	//HobjectתMat
	cv::Mat HImageToMat(const HalconCpp::HImage &hImg);
	cv::Mat HObjectToMat(const HalconCpp::HObject &hObj);
	//MatתHobject
	HalconCpp::HObject MatToHObject(const cv::Mat &image);
	HalconCpp::HImage MatToHImage(const cv::Mat &image);
	//QImageתMat
	cv::Mat QImage2Mat(const QImage image);
	//MatתQImage
	QImage Mat2QImage(const cv::Mat &mat);
};
