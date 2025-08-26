#include "ImageTypeConvert.h"

ImageTypeConvert::ImageTypeConvert()
{}

ImageTypeConvert::~ImageTypeConvert()
{}
//Mat转QImage
QImage ImageTypeConvert::Mat2QImage(const cv::Mat &mat)
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

cv::Mat ImageTypeConvert::QImage2Mat(QImage image)
{
	cv::Mat mat;
	//qDebug() << image.format();
	switch (image.format())
	{
	case QImage::Format_ARGB32:
	case QImage::Format_RGB32:
	case QImage::Format_ARGB32_Premultiplied:
		mat = cv::Mat(image.height(), image.width(), CV_8UC4, (void*)image.constBits(), image.bytesPerLine());
		break;
	case QImage::Format_RGB888:
		mat = cv::Mat(image.height(), image.width(), CV_8UC3, (void*)image.constBits(), image.bytesPerLine());
		cv::cvtColor(mat, mat, COLOR_BGR2RGB);
		break;
	case QImage::Format_Indexed8:
		mat = cv::Mat(image.height(), image.width(), CV_8UC1, (void*)image.constBits(), image.bytesPerLine());
		break;
	}
	return mat;
}

HObject ImageTypeConvert::Mat2HObject(const cv::Mat &image)
{
	HObject Hobj = HObject();
	int hgt = image.rows;
	int wid = image.cols;
	int i;
	//  CV_8UC3  
	if (image.type() == CV_8UC3)
	{
		vector<cv::Mat> imgchannel;
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
		GenImage3(&Hobj, "byte", wid, hgt, (Hlong)dataR, (Hlong)dataG, (Hlong)dataB);
		delete[]dataR;
		delete[]dataG;
		delete[]dataB;
		dataR = NULL;
		dataG = NULL;
		dataB = NULL;
	}
	//  CV_8UCU1  
	else if (image.type() == CV_8UC1)
	{
		uchar* data = new uchar[hgt*wid];
		for (i = 0; i < hgt; i++)
			memcpy(data + wid * i, image.data + image.step*i, wid);
		GenImage1(&Hobj, "byte", wid, hgt, (Hlong)data);
		delete[] data;
		data = NULL;
	}
	return Hobj;
}
//Hobject转Mat
cv::Mat ImageTypeConvert::HObject2Mat(HObject Hobj)
{
	HTuple htCh;
	HString cType;
	cv::Mat Image;
	ConvertImageType(Hobj, &Hobj, "byte");
	CountChannels(Hobj, &htCh);
	Hlong wid = 0;
	Hlong hgt = 0;
	if (htCh[0].I() == 1)
	{
		HImage hImg(Hobj);
		void *ptr = hImg.GetImagePointer1(&cType, &wid, &hgt);//GetImagePointer1(Hobj, &ptr, &cType, &wid, &hgt);
		int W = wid;
		int H = hgt;
		Image.create(H, W, CV_8UC1);
		unsigned char *pdata = static_cast<unsigned char *>(ptr);
		memcpy(Image.data, pdata, W*H);
	}
	else if (htCh[0].I() == 3)
	{
		void *Rptr;
		void *Gptr;
		void *Bptr;
		HImage hImg(Hobj);
		hImg.GetImagePointer3(&Rptr, &Gptr, &Bptr, &cType, &wid, &hgt);
		int W = wid;
		int H = hgt;
		Image.create(H, W, CV_8UC3);
		vector<cv::Mat> VecM(3);
		VecM[0].create(H, W, CV_8UC1);
		VecM[1].create(H, W, CV_8UC1);
		VecM[2].create(H, W, CV_8UC1);
		unsigned char *R = (unsigned char *)Rptr;
		unsigned char *G = (unsigned char *)Gptr;
		unsigned char *B = (unsigned char *)Bptr;
		memcpy(VecM[2].data, R, W*H);
		memcpy(VecM[1].data, G, W*H);
		memcpy(VecM[0].data, B, W*H);
		cv::merge(VecM, Image);
	}
	return Image;
}
//
//QImage ImageTypeConvert::HObjectToQImage(HObject himage)
//{
//	QImage qimage;
//	HTuple hChannels;
//	HTuple   width, height;
//	width = height = 0;
//	HTuple htype;
//	HTuple hpointer;
//
//	ConvertImageType(himage, &himage, "byte");//将图片转化成byte类型
//	CountChannels(himage, &hChannels);       //判断图像通道数
//
//	if (hChannels[0].I() == 1)//单通道图
//	{
//		unsigned char *ptr;
//
//		GetImagePointer1(himage, &hpointer, &htype, &width, &height);
//
//		ptr = (unsigned char *)hpointer[0].L();
//		qimage = QImage(ptr, width, height, width, QImage::Format_Indexed8);//不知道是否已自动4字节对齐
//	}
//	else if (hChannels[0].I() == 3)//三通道图
//	{
//		HTuple hv_ptrRed, hv_ptrGreen, hv_ptrBlue;
//		GetImagePointer3(himage, &hv_ptrRed, &hv_ptrGreen, &hv_ptrBlue, &htype, &width, &height);
//
//		uchar *ptrRed = (uchar*)hv_ptrRed[0].L();
//		uchar *ptrGreen = (uchar*)hv_ptrGreen[0].L();
//		uchar *ptrBlue = (uchar*)hv_ptrBlue[0].L();
//		int bytesperline = (width * 8 * 3 + 31) / 32 * 4;//针对位深为8的三通道图进行每行4字节对齐补齐
//		int bytecount = bytesperline * height;//整个图像数据需要的字节数
//		uchar* data8 = new uchar[bytecount];
//		int lineheadid, pixid;
//		for (int i = 0; i < height; i++)
//		{
//			lineheadid = bytesperline * i;//计算出图像第i行的行首在图像数据中的地址
//			for (int j = 0; j < width; j++)
//			{
//				pixid = lineheadid + j * 3;//计算坐标为（i，j）的像素id
//				data8[pixid] = ptrRed[width*i + j];
//				data8[pixid + 1] = ptrGreen[width*i + j];
//				data8[pixid + 2] = ptrBlue[width*i + j];
//			}
//		}
//		qimage = QImage(data8, width, height, QImage::Format_RGB888);
//	}
//	return qimage;
//}
//
//int ImageTypeConvert::QImageToHObjbect(QImage image, HObject &Object)
//{
//	if (image.format() == QImage::Format_RGB888
//		|| image.format() == QImage::Format_RGB32
//		|| image.format() == QImage::Format_Indexed8)
//	{
//
//	}
//	else
//	{
//		return -1;
//	}
//
//	int ret = 0;
//	unsigned char  *image_red = nullptr;
//	unsigned char  *image_green = nullptr;
//	unsigned char  *image_blue = nullptr;
//	int            r, c;
//
//	int w = image.width();
//	int h = image.height();
//	try
//	{
//		image_red = (unsigned char  *)malloc(w*h);
//		if (image.format() == QImage::Format_RGB888
//			|| image.format() == QImage::Format_RGB32)
//		{
//			image_green = (unsigned char  *)malloc(w*h);
//			image_blue = (unsigned char  *)malloc(w*h);
//		}
//	}
//	catch (...)
//	{
//		free(image_red);
//		free(image_green);
//		free(image_blue);
//		return -1;
//	}
//
//
//	if (image.format() == QImage::Format_RGB888)
//	{
//		for (r = 0; r < h; r++)
//		{
//			uchar* pRow = image.scanLine(r);
//			for (c = 0; c < w; c++)
//			{
//				image_red[r*w + c] = pRow[3 * c + 0];
//				image_green[r*w + c] = pRow[3 * c + 1];
//				image_blue[r*w + c] = pRow[3 * c + 2];
//			}
//		}
//		GenImage3Extern(&Object, "byte", w, h, (Hlong)image_red, (Hlong)image_green, (Hlong)image_blue, (Hlong)free);
//		free(image_red); free(image_green); free(image_blue);
//
//	}
//	else if (image.format() == QImage::Format_RGB32)
//	{
//		for (r = 0; r < h; r++)
//		{
//			uchar* pRow = image.scanLine(r);
//			for (c = 0; c < w; c++)
//			{
//				image_red[r*w + c] = pRow[4 * c + 2];
//				image_green[r*w + c] = pRow[4 * c + 1];
//				image_blue[r*w + c] = pRow[4 * c + 0];
//			}
//		}
//		GenImage3Extern(&Object, "byte", w, h, (Hlong)image_red, (Hlong)image_green, (Hlong)image_blue, (Hlong)free);
//	}
//	else if (image.format() == QImage::Format_Indexed8)
//	{
//		for (r = 0; r < h; r++)
//		{
//			uchar* pRow = image.scanLine(r);
//			for (c = 0; c < w; c++)
//			{
//				image_red[r*w + c] = pRow[c];
//				//image_red[r*w + c] = 0.30 * pBmp[3 * (c + r*w) + 0] + 0.59 * pBmp[3 * (c + r*w) + 1] + 0.11 *pBmp[3 * (c + r*w) + 2];
//			}
//		}
//		GenImage1Extern(&Object, "byte", w, h, (Hlong)image_red, (Hlong)free);
//	}
//	return ret;
//}
