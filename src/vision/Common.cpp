#include "Common.h"

Common::Common()
{}

Common::~Common()
{}
//Hobject转Mat
cv::Mat Common::HImageToMat(const HalconCpp::HImage &hImg)
{

	cv::Mat mat;
	int channels = hImg.CountChannels()[0].I();
	HalconCpp::HImage hImage = hImg.ConvertImageType("byte");

	Hlong hW = 0, hH = 0;
	HalconCpp::HString cType;

	if (channels == 1) {
		void *r = hImage.GetImagePointer1(&cType, &hW, &hH);
		mat.create(int(hH), int(hW), CV_8UC1);
		memcpy(mat.data, static_cast<unsigned char *>(r), int(hW*hH));
	}
	else if (channels == 3) {
		void *r = NULL, *g = NULL, *b = NULL;

		hImage.GetImagePointer3(&r, &g, &b, &cType, &hW, &hH);
		mat.create(int(hH), int(hW), CV_8UC3);

		std::vector<cv::Mat> vec(3);
		vec[0].create(int(hH), int(hW), CV_8UC1);
		vec[1].create(int(hH), int(hW), CV_8UC1);
		vec[2].create(int(hH), int(hW), CV_8UC1);

		memcpy(vec[2].data, static_cast<unsigned char *>(r), int(hW*hH));
		memcpy(vec[1].data, static_cast<unsigned char *>(g), int(hW*hH));
		memcpy(vec[0].data, static_cast<unsigned char *>(b), int(hW*hH));
		cv::merge(vec, mat);
	}
	return mat;

}
cv::Mat Common::HObjectToMat(const HalconCpp::HObject &hObj)
{
	HalconCpp::HImage hImg(hObj);
	return HImageToMat(hImg);
}
//Mat转Hobject
HalconCpp::HObject Common::MatToHObject(const cv::Mat &image)
{
	HalconCpp::HObject Hobj = HalconCpp::HObject();
	int hgt = image.rows;
	int wid = image.cols;
	int i;
	if (image.type() == CV_8UC3)
	{
		std::vector<cv::Mat> imgchannel;
		split(image, imgchannel);
		cv::Mat imgB = imgchannel[0];
		cv::Mat imgG = imgchannel[1];
		cv::Mat imgR = imgchannel[2];
		uchar* dataR = new uchar[hgt*wid];
		uchar* dataG = new uchar[hgt*wid];
		uchar* dataB = new uchar[hgt*wid];
		for (i = 0; i < hgt; i++)
		{
			memcpy(dataR + wid * i, imgR.data + imgR.step*i, wid);
			memcpy(dataG + wid * i, imgG.data + imgG.step*i, wid);
			memcpy(dataB + wid * i, imgB.data + imgB.step*i, wid);
		}
		HalconCpp::GenImage3(&Hobj, "byte", wid, hgt, (Hlong)dataR, (Hlong)dataG, (Hlong)dataB);
		delete[]dataR;
		delete[]dataG;
		delete[]dataB;
		dataR = NULL;
		dataG = NULL;
		dataB = NULL;
	}
	else if (image.type() == CV_8UC1)
	{
		uchar* data = new uchar[hgt*wid];
		for (i = 0; i < hgt; i++)
			memcpy(data + wid * i, image.data + image.step*i, wid);
		HalconCpp::GenImage1(&Hobj, "byte", wid, hgt, (Hlong)data);
		delete[] data;
		data = NULL;
	}
	return Hobj;
}
HalconCpp::HImage Common::MatToHImage(const cv::Mat &image)
{
	HalconCpp::HImage hImg(MatToHObject(image));
	return hImg;
}
//QImage转Mat
cv::Mat Common::QImage2Mat(const QImage image)
{
	if (image.isNull())
	{
		return cv::Mat();
	}

	cv::Mat mat;
	switch (image.format())
	{
	case QImage::Format_ARGB32:
	case QImage::Format_RGB32:
	case QImage::Format_ARGB32_Premultiplied:
		mat = cv::Mat(image.height(), image.width(), CV_8UC4, (void*)image.constBits(), image.bytesPerLine());
		break;
	case QImage::Format_RGB888:
		mat = cv::Mat(image.height(), image.width(), CV_8UC3, (void*)image.constBits(), image.bytesPerLine());
        cv::cvtColor(mat, mat, cv::COLOR_BGR2RGB);
		break;
	case QImage::Format_Grayscale8:
	case QImage::Format_Indexed8:
		mat = cv::Mat(image.height(), image.width(), CV_8UC1, (void*)image.constBits(), image.bytesPerLine());
		break;
	}
	return mat;
}
//Mat转QImage
QImage Common::Mat2QImage(const cv::Mat &mat)
{
	if (mat.empty()) return QImage();
	// 8-bits unsigned, NO. OF CHANNELS = 1
	if (mat.type() == CV_16UC1)
	{
		//qDebug() << "CV_16UC1转换";
		cv::Mat mat_8;
		mat.convertTo(mat_8, CV_8UC1, 255.0 / 4095.0);
		// 16位数据（2的16次方65535）归一化到8位数据（2的8次方）
		//mat.convertTo(mat_8, CV_8UC1,1/65535.0*255.0); 
		//mat.convertTo(mat_8, CV_8UC1,1/4096*255.0);

		QImage image(mat_8.cols, mat_8.rows, QImage::Format_Indexed8);
		image.setColorCount(256);
		for (int i = 0; i < 256; i++)
		{
			image.setColor(i, qRgb(i, i, i));
		}
		// Copy input Mat
		uchar *pSrc = mat_8.data;
		for (int row = 0; row < mat_8.rows; row++)
		{
			uchar *pDest = image.scanLine(row);
			memcpy(pDest, pSrc, mat_8.cols);
			pSrc += mat_8.step;
		}
		mat_8.release();
		return image;
	}
	if (mat.type() == CV_8UC1)
	{
		QImage image(mat.cols, mat.rows, QImage::Format_Indexed8);
		// Set the color table (used to translate colour indexes to qRgb values)
		image.setColorCount(256);
		for (int i = 0; i < 256; i++)
		{
			image.setColor(i, qRgb(i, i, i));
		}
		// Copy input Mat
		uchar *pSrc = mat.data;
		for (int row = 0; row < mat.rows; row++)
		{
			uchar *pDest = image.scanLine(row);
			memcpy(pDest, pSrc, mat.cols);
			pSrc += mat.step;
		}
		return image;
	}
	// 8-bits unsigned, NO. OF CHANNELS = 3
	else if (mat.type() == CV_8UC3)
	{
		// Copy input Mat
		const uchar *pSrc = (const uchar*)mat.data;
		// Create QImage with same dimensions as input Mat
		QImage image(pSrc, mat.cols, mat.rows, mat.step, QImage::Format_RGB888);
		return image.rgbSwapped();
	}
	else if (mat.type() == CV_8UC4)
	{
		// Copy input Mat
		const uchar *pSrc = (const uchar*)mat.data;
		// Create QImage with same dimensions as input Mat
		QImage image(pSrc, mat.cols, mat.rows, mat.step, QImage::Format_ARGB32);
		return image.copy();
	}
	else
	{
		return QImage();
	}
}
