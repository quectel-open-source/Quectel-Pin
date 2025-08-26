#include "PaintDetect.h"
PaintDetect::PaintDetect()
{

}
PaintDetect::~PaintDetect()
{

}
int PaintDetect::PaintImage(HObject ho_Image, RunParamsStruct_PaintDetect &RunParams, cv::Mat &ho_DestImg)
{
	try
	{
		if (ho_Image.Key() == nullptr)
		{
			//图像为空，或者图像被clear
			//mutex1.unlock();
			return 1;
		}
		if (RunParams.vec_Ret.size() <= 0)
		{
			Mat mat_Img = HObject2Mat(ho_Image);
			if (RunParams.hv_Ret == 1)
			{
				DrawString(mat_Img, 1, RunParams.hv_Word_Size, RunParams.hv_Word_X, RunParams.hv_Word_Y);
			}
			else if (RunParams.hv_Ret == 0)
			{
				DrawString(mat_Img, 0, RunParams.hv_Word_Size, RunParams.hv_Word_X, RunParams.hv_Word_Y);
			}
			ho_DestImg = mat_Img;
		}
		else
		{
			if (RunParams.hv_LineWidth < 2)
			{
				RunParams.hv_LineWidth = 2;
			}
			HTuple count1 = 0;
			CountChannels(ho_Image, &count1);
			if (count1 == 1)
			{
				HObject imgPaint;
				GenEmptyObj(&imgPaint);
				imgPaint.Clear();
				for (int i = 0; i < RunParams.vec_InputRegion.size(); i++)
				{
					try
					{
						HObject BoundaryRegion;
						Boundary(RunParams.vec_InputRegion[i], &BoundaryRegion, "inner");
						HObject DilateRegion;
						DilationCircle(BoundaryRegion, &DilateRegion, RunParams.hv_LineWidth / 2);
						if (i == 0)
						{
							HObject img1, img2, img3;
							if (RunParams.vec_Ret[i] == 1)
							{
								PaintRegion(DilateRegion, ho_Image, &img1, 0, "fill");
								PaintRegion(DilateRegion, ho_Image, &img2, 255, "fill");
								PaintRegion(DilateRegion, ho_Image, &img3, 0, "fill");
							}
							else
							{
								PaintRegion(DilateRegion, ho_Image, &img1, 255, "fill");
								PaintRegion(DilateRegion, ho_Image, &img2, 0, "fill");
								PaintRegion(DilateRegion, ho_Image, &img3, 0, "fill");
							}
							Compose3(img1, img2, img3, &imgPaint);
						}
						else
						{
							if (RunParams.vec_Ret[i] == 1)
							{
								PaintRegion(DilateRegion, imgPaint, &imgPaint, ((HTuple(0).Append(255)).Append(0)), "fill");
							}
							else
							{
								PaintRegion(DilateRegion, imgPaint, &imgPaint, ((HTuple(255).Append(0)).Append(0)), "fill");
							}
						}
					}
					catch (...)
					{

					}
				}
				//输出画好的图
				Mat mat_Img;
				if (imgPaint.Key() == nullptr)
				{
					mat_Img = HObject2Mat(ho_Image);
				}
				else
				{
					mat_Img = HObject2Mat(imgPaint);
				}
				if (RunParams.hv_Ret == 1)
				{
					DrawString(mat_Img, 1, RunParams.hv_Word_Size, RunParams.hv_Word_X, RunParams.hv_Word_Y);
				}
				else if (RunParams.hv_Ret == 0)
				{
					DrawString(mat_Img, 0, RunParams.hv_Word_Size, RunParams.hv_Word_X, RunParams.hv_Word_Y);
				}
				ho_DestImg = mat_Img;
			}
			else if (count1 == 3)
			{
				for (int i = 0; i < RunParams.vec_InputRegion.size(); i++)
				{
					try
					{
						HObject BoundaryRegion;
						Boundary(RunParams.vec_InputRegion[i], &BoundaryRegion, "inner");
						HObject DilateRegion;
						DilationCircle(BoundaryRegion, &DilateRegion, RunParams.hv_LineWidth / 2);
						if (RunParams.vec_Ret[i] == 1)
						{
							PaintRegion(DilateRegion, ho_Image, &ho_Image, ((HTuple(0).Append(255)).Append(0)), "fill");
						}
						else
						{
							PaintRegion(DilateRegion, ho_Image, &ho_Image, ((HTuple(255).Append(0)).Append(0)), "fill");
						}
					}
					catch (...)
					{

					}
				}
				//输出画好的图
				Mat mat_Img = HObject2Mat(ho_Image);
				if (RunParams.hv_Ret == 1)
				{
					DrawString(mat_Img, 1, RunParams.hv_Word_Size, RunParams.hv_Word_X, RunParams.hv_Word_Y);
				}
				else if (RunParams.hv_Ret == 0)
				{
					DrawString(mat_Img, 0, RunParams.hv_Word_Size, RunParams.hv_Word_X, RunParams.hv_Word_Y);
				}
				ho_DestImg = mat_Img;
			}
		}
		return 0;
	}
	catch (...)
	{
		return -1;
	}
}

void PaintDetect::DrawString(Mat& OutImageMat, HTuple isOK, double fontSize, int leftUpX, int leftUpY) {
	int channels = OutImageMat.channels();

	int font_face = cv::FONT_HERSHEY_SIMPLEX;
	//double font_scale = 10;
	int lineType = 8;
	int temp;
	int thickness = 20;
	bool isok = isOK.I();
	std::string text0 = "";
	cv::Scalar color = cv::Scalar(0, 255, 255);
	if (isok) {
		text0 = "OK";
		color = cv::Scalar(0, 255, 0);
	}
	else {
		text0 = "NG";
		color = cv::Scalar(0, 0, 255);
	}
	cv::Size text_size0 = cv::getTextSize(text0, font_face, fontSize, thickness, &temp);
	int text_height = text_size0.height;
	cv::Point p1;
	p1.x = leftUpX;
	p1.y = leftUpY + text_height;
	if (channels == 1)
		cv::cvtColor(OutImageMat, OutImageMat, cv::COLOR_GRAY2BGR);

	cv::putText(OutImageMat, text0, p1, font_face, fontSize, color, thickness, lineType);

}

cv::Mat PaintDetect::HObject2Mat(HObject Hobj) {
	HTuple htCh;
	HString cType;
	cv::Mat Image;
	ConvertImageType(Hobj, &Hobj, "byte");
	CountChannels(Hobj, &htCh);
	Hlong wid = 0;
	Hlong hgt = 0;
	if (htCh[0].I() == 1) {
		HImage hImg(Hobj);
		void* ptr = hImg.GetImagePointer1(&cType, &wid, &hgt);//GetImagePointer1(Hobj, &ptr, &cType, &wid, &hgt);
		int W = wid;
		int H = hgt;
		Image.create(H, W, CV_8UC1);
		unsigned char* pdata = static_cast<unsigned char*>(ptr);
		memcpy(Image.data, pdata, W * H);
	}
	else if (htCh[0].I() == 3) {
		void* Rptr;
		void* Gptr;
		void* Bptr;
		HImage hImg(Hobj);
		hImg.GetImagePointer3(&Rptr, &Gptr, &Bptr, &cType, &wid, &hgt);
		int W = wid;
		int H = hgt;
		Image.create(H, W, CV_8UC3);
		vector<cv::Mat> VecM(3);

		VecM[0] = cv::Mat(H, W, CV_8UC1, Bptr);
		VecM[1] = cv::Mat(H, W, CV_8UC1, Gptr);
		VecM[2] = cv::Mat(H, W, CV_8UC1, Rptr);

		cv::merge(VecM, Image);
	}
	return Image;
}

HObject PaintDetect::Mat2HObject(cv::Mat image) {
	HObject Hobj = HObject();
	int hgt = image.rows;
	int wid = image.cols;
	int i;
	if (image.type() == CV_8UC3) {
		vector<cv::Mat> imgchannel;
		split(image, imgchannel);
		cv::Mat imgB = imgchannel[0];
		cv::Mat imgG = imgchannel[1];
		cv::Mat imgR = imgchannel[2];
		uchar* dataR = new uchar[hgt * wid];
		uchar* dataG = new uchar[hgt * wid];
		uchar* dataB = new uchar[hgt * wid];
		for (i = 0; i < hgt; i++) {
			memcpy(dataR + wid * i, imgR.data + imgR.step * i, wid);
			memcpy(dataG + wid * i, imgG.data + imgG.step * i, wid);
			memcpy(dataB + wid * i, imgB.data + imgB.step * i, wid);
		}
		GenImage3(&Hobj, "byte", wid, hgt, (Hlong)dataR, (Hlong)dataG, (Hlong)dataB);
		delete[]dataR;
		delete[]dataG;
		delete[]dataB;
		dataR = NULL;
		dataG = NULL;
		dataB = NULL;
	}
	else if (image.type() == CV_8UC1) {
		uchar* data = new uchar[hgt * wid];
		for (i = 0; i < hgt; i++)
			memcpy(data + wid * i, image.data + image.step * i, wid);
		GenImage1(&Hobj, "byte", wid, hgt, (Hlong)data);
		delete[] data;
		data = NULL;
	}
	else {
		vector<cv::Mat> imgchannel;
		split(image, imgchannel);
		cv::Mat imgB = imgchannel[0];
		cv::Mat imgG = imgchannel[1];
		cv::Mat imgR = imgchannel[2];
		uchar* dataR = new uchar[hgt * wid];
		uchar* dataG = new uchar[hgt * wid];
		uchar* dataB = new uchar[hgt * wid];
		for (i = 0; i < hgt; i++) {
			memcpy(dataR + wid * i, imgR.data + imgR.step * i, wid);
			memcpy(dataG + wid * i, imgG.data + imgG.step * i, wid);
			memcpy(dataB + wid * i, imgB.data + imgB.step * i, wid);
		}
		GenImage3(&Hobj, "byte", wid, hgt, (Hlong)dataR, (Hlong)dataG, (Hlong)dataB);
		delete[]dataR;
		delete[]dataG;
		delete[]dataB;
		dataR = NULL;
		dataG = NULL;
		dataB = NULL;
	}
	return Hobj;
}

