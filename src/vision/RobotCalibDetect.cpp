#include "RobotCalibDetect.h"
RobotCalibDetect::RobotCalibDetect()
{
	PaintDetect1 = new PaintDetect();
}
RobotCalibDetect::~RobotCalibDetect()
{
	delete PaintDetect1;
}
//机器人定位
int RobotCalibDetect::RobotLocate(const cv::Mat &InputImage, const RunParamsStruct_RobotCalib &RunParams, ResultParamsStruct_RobotCalib &ResultParams)
{
	try
	{
		//清空结果
		ResultParams.DestImg = InputImage;

        ResultParams.hv_X_Robot = -99999;
        ResultParams.hv_Y_Robot = -99999;
        ResultParams.hv_A_Robot = -99999;

		//建模时的坐标和角度
		ResultParams.hv_Angle_Original = 0;
		ResultParams.hv_CenterRow_Original = 0;
		ResultParams.hv_CenterCol_Original = 0;
		//运行时找到的模板结果
		ResultParams.hv_Row.clear();
		ResultParams.hv_Column.clear();
		ResultParams.hv_Angle.clear();
		ResultParams.hv_Score.clear();
		ResultParams.hv_RetNum = 0;
		ResultParams.hv_ROIHomMat2D = NULL;


		//检测代码
		ResultParamsStruct_RobotCalib retParamsTemp;
		int ret = FindTemplate(InputImage, RunParams, retParamsTemp);
		if (ret != 0)
		{
			return 1;
		}
		if (retParamsTemp.hv_RetNum != 1)
		{
			return 1;
		}
		//计算机器人偏差
		RunParamsStruct_RobotCalib runParamsTemp;
		runParamsTemp = RunParams;
		runParamsTemp.hv_NowX = retParamsTemp.hv_Column.at(0);
		runParamsTemp.hv_NowY = retParamsTemp.hv_Row.at(0);
		runParamsTemp.hv_NowA = retParamsTemp.hv_Angle.at(0);
        double nowx = runParamsTemp.hv_NowX.D();
        double nowy = runParamsTemp.hv_NowY.D();
        double nowa = runParamsTemp.hv_NowA.D();

		CalculateFunc(runParamsTemp, ResultParams);
		ResultParams.DestImg = retParamsTemp.DestImg;
		return 0;
	}
	catch (...)
	{
		//清空结果
		ResultParams.DestImg = InputImage;

        ResultParams.hv_X_Robot = -99999;
        ResultParams.hv_Y_Robot = -99999;
        ResultParams.hv_A_Robot = -99999;

		//建模时的坐标和角度
		ResultParams.hv_Angle_Original = 0;
		ResultParams.hv_CenterRow_Original = 0;
		ResultParams.hv_CenterCol_Original = 0;
		//运行时找到的模板结果
		ResultParams.hv_Row.clear();
		ResultParams.hv_Column.clear();
		ResultParams.hv_Angle.clear();
		ResultParams.hv_Score.clear();
		ResultParams.hv_RetNum = 0;
		ResultParams.hv_ROIHomMat2D = NULL;

		return -1;
	}
}
//计算偏移量和机器人绝对坐标函数
int RobotCalibDetect::CalculateFunc(const RunParamsStruct_RobotCalib &RunParams, ResultParamsStruct_RobotCalib &ResultParams)
{
	try
	{
		if (RunParams.hv_IsAbsCoordinate)
		{
			//计算机器人绝对坐标
            double outX = -99999, outY = -99999;
			TransPoint(RunParams, outX, outY);
			ResultParams.hv_X_Robot = outX;
			ResultParams.hv_Y_Robot = outY;
            if (RunParams.hv_IsReverseAngle)
            {
                ResultParams.hv_A_Robot = -RunParams.hv_NowA;
            }
            else
            {
                ResultParams.hv_A_Robot = RunParams.hv_NowA;
            }
		}
		else
		{
			//计算偏移量
            double offsetX = -99999, offsetY = -99999, offsetA = -99999;
			CalcuOffset(RunParams, offsetX, offsetY, offsetA);
			ResultParams.hv_X_Robot = offsetX;
			ResultParams.hv_Y_Robot = offsetY;
			ResultParams.hv_A_Robot = offsetA;
		}
		return 0;
	}
	catch (...)
	{
		return -1;
	}
}
//九点标定函数
int RobotCalibDetect::NPointCalibrate(RunParamsStruct_RobotCalib &RunParams)
{
	try
	{

		if (RunParams.hv_NPointCamera_X.TupleLength() < 9 || RunParams.hv_NPointCamera_Y.TupleLength() < 9 || RunParams.hv_NPointRobot_X.TupleLength() < 9 || RunParams.hv_NPointRobot_Y.TupleLength() < 9)
		{
			return 1;
		}
		if (RunParams.hv_NPointCamera_X.TupleLength() != RunParams.hv_NPointCamera_Y.TupleLength() || RunParams.hv_NPointCamera_X.TupleLength() != RunParams.hv_NPointRobot_X.TupleLength()
			|| RunParams.hv_NPointCamera_X.TupleLength() != RunParams.hv_NPointRobot_Y.TupleLength())
		{
			return 1;
		}

		VectorToHomMat2d(RunParams.hv_NPointCamera_X, RunParams.hv_NPointCamera_Y, RunParams.hv_NPointRobot_X, RunParams.hv_NPointRobot_Y, &RunParams.hv_HomMat2D);
		return 0;
	}
	catch (...)
	{
		return -1;
	}
}
//旋转中心标定
int RobotCalibDetect::CenterCalibrate(RunParamsStruct_RobotCalib &RunParams)
{
	try
	{
		if (RunParams.hv_CenterRobot_X.TupleLength() < 3 || RunParams.hv_CenterRobot_Y.TupleLength() < 3)
		{
			return 1;
		}
		if (RunParams.hv_CenterRobot_X.TupleLength() != RunParams.hv_CenterRobot_Y.TupleLength())
		{
			return 1;
		}
        for (int i = 0; i < RunParams.hv_CenterRobot_X.TupleLength().I(); i++)
        {
            double x1 = RunParams.hv_CenterRobot_X[i].D();
            double y1 = RunParams.hv_CenterRobot_Y[i].D();
            double x2 = RunParams.hv_CenterCamera_X[i].D();
            double y2 = RunParams.hv_CenterCamera_Y[i].D();
            double aa = RunParams.hv_CenterCamera_Y[i].D();
        }
		//计算机器旋转中心
		HObject ho_Contour;
		//获取拟合圆XLD
		GenContourPolygonXld(&ho_Contour, RunParams.hv_CenterRobot_X, RunParams.hv_CenterRobot_Y);
		HTuple hv_StartPhi, hv_EndPhi, hv_PointOrder;
		//拟合圆。使用的算法是''geotukey''，该法减少了轮廓点与结果圆之间的几何距离，该法被统计是最优的，
		//但占用很多计算时间。如果轮廓点被噪声干扰严重，可以考虑使用该方法
		FitCircleContourXld(ho_Contour, "geotukey", -1, 0, 0, 3, 2,
			&RunParams.hv_Center_X, &RunParams.hv_Center_Y, &RunParams.hv_Center_Radius, &hv_StartPhi, &hv_EndPhi,
			&hv_PointOrder);

        double x3 = RunParams.hv_Center_X.D();
        double y3 = RunParams.hv_Center_Y.D();
        double r = RunParams.hv_Center_Radius.D();

		//计算像素旋转中心
		//获取拟合圆XLD
		ho_Contour.Clear();
		GenContourPolygonXld(&ho_Contour, RunParams.hv_CenterCamera_X, RunParams.hv_CenterCamera_Y);
		hv_StartPhi.Clear(); hv_EndPhi.Clear(); hv_PointOrder.Clear();
		//拟合圆。使用的算法是''geotukey''，该法减少了轮廓点与结果圆之间的几何距离，该法被统计是最优的，
		//但占用很多计算时间。如果轮廓点被噪声干扰严重，可以考虑使用该方法
		HTuple hv_Center_X_C, hv_Center_Y_C, hv_Center_Radius_C;
		FitCircleContourXld(ho_Contour, "geotukey", -1, 0, 0, 3, 2,
			&hv_Center_X_C, &hv_Center_Y_C, &hv_Center_Radius_C, &hv_StartPhi, &hv_EndPhi,
			&hv_PointOrder);

        double x4 = hv_Center_X_C.D();
        double y4 = hv_Center_Y_C.D();

		HObject circle;
        HalconCpp::GenCircleContourXld(&circle, hv_Center_Y_C, hv_Center_X_C, hv_Center_Radius_C, 0, HTuple(360).TupleRad(),"positive",1);
        //GenCircle(&circle, hv_Center_X_C, hv_Center_Y_C, hv_Center_Radius_C);
		HObject cross;
        GenCrossContourXld(&cross, hv_Center_Y_C, hv_Center_X_C, 16, 0.78);
		ConcatObj(cross, circle, &circle);
		RunParams.ho_CenterCircle = circle;
		return 0;
	}
	catch (...)
	{
		return -1;
	}

}
//计算偏移量
int RobotCalibDetect::CalcuOffset(const RunParamsStruct_RobotCalib &RunParams, double &OffsetX, double &OffsetY, double &OffsetA)
{
	try
	{
        //把相机坐标转换到机器人
        double nowx,nowy,nowa;
        TransPoint(RunParams, nowx, nowy);
        nowa = RunParams.hv_NowA;
		//计算旋转中心(当建模位机器人绝对坐标与计算旋转中心位的机器人绝对坐标不一致的时候，需要偏移旋转中心)
		double dCenterX = RunParams.hv_Center_X.D();
		double dCenterY = RunParams.hv_Center_Y.D();
		dCenterX = dCenterX + (RunParams.hv_ModelPosition_RobotX - RunParams.hv_CenterPosition_RobotX);
		dCenterY = dCenterY + (RunParams.hv_ModelPosition_RobotY - RunParams.hv_CenterPosition_RobotY);
		//启用旋转中心
		if (RunParams.hv_IsUseCenterCalib)
		{
			////************************第1种计算方式-利用矩阵计算***************************************************
			////先拍后抓(场景是取料或者贴合, 移动模板位置到当前拍照位置)
			//if (RunParams.hv_IsCamFirst)
			//{
			//	GetOffset_CamFirst1(RunParams.hv_IsReverseAngle, RunParams.hv_ModelX.D(), RunParams.hv_ModelY.D(), RunParams.hv_ModelA.D(),
			//		RunParams.hv_NowX.D(), RunParams.hv_NowY.D(), RunParams.hv_NowA.D(), dCenterX, dCenterY,
			//		OffsetX, OffsetY, OffsetA);
			//}
			//else
			//{
			//	//先抓后拍(场景是机器人吸取物料后, 移动到下相机的上方，拍照计算偏移量，然后再去贴合位置.移动当前位置到模板位置)
			//	GetOffset_GrabFirst1(RunParams.hv_IsReverseAngle, RunParams.hv_ModelX.D(), RunParams.hv_ModelY.D(), RunParams.hv_ModelA.D(),
			//		RunParams.hv_NowX.D(), RunParams.hv_NowY.D(), RunParams.hv_NowA.D(), dCenterX, dCenterY,
			//		OffsetX, OffsetY, OffsetA);
			//}
			//************************第2种计算方式-利用三角函数计算***************************************************
			//先拍后抓(场景是取料或者贴合, 移动模板位置到当前拍照位置)
			if (RunParams.hv_IsCamFirst)
			{
				GetOffset_CamFirst2(RunParams.hv_IsReverseAngle, RunParams.hv_ModelX.D(), RunParams.hv_ModelY.D(), RunParams.hv_ModelA.D(),
                    nowx, nowy, nowa, dCenterX, dCenterY,
					OffsetX, OffsetY, OffsetA);
			}
			else
			{
				//先抓后拍(场景是机器人吸取物料后, 移动到下相机的上方，拍照计算偏移量，然后再去贴合位置.移动当前位置到模板位置)
				GetOffset_GrabFirst2(RunParams.hv_IsReverseAngle, RunParams.hv_ModelX.D(), RunParams.hv_ModelY.D(), RunParams.hv_ModelA.D(),
                    nowx, nowy, nowa, dCenterX, dCenterY,
					OffsetX, OffsetY, OffsetA);
			}
		}
		else
		{
			//不启用旋转中心
		    //先拍后抓(场景是取料或者贴合, 移动模板位置到当前拍照位置)
			if (RunParams.hv_IsCamFirst)
			{
                OffsetX = nowx - RunParams.hv_ModelX.D();
                OffsetY = nowy - RunParams.hv_ModelY.D();
                OffsetA = nowa - RunParams.hv_ModelA.D();
				if (RunParams.hv_IsReverseAngle)
				{
					OffsetA = 0 - OffsetA;
				}
			}
			else
			{
				//先抓后拍(场景是机器人吸取物料后, 移动到下相机的上方，拍照计算偏移量，然后再去贴合位置.移动当前位置到模板位置)
                OffsetX = RunParams.hv_ModelX.D() - nowx;
                OffsetY = RunParams.hv_ModelY.D() - nowy;
                OffsetA = RunParams.hv_ModelA.D() - nowa;
				if (RunParams.hv_IsReverseAngle)
				{
					OffsetA = 0 - OffsetA;
				}
			}
		}

		return 0;
	}
	catch (...)
	{
        OffsetX = -99999;
        OffsetY = -99999;
        OffsetA = -99999;
		return 1;
	}
}
//坐标点转换
int RobotCalibDetect::TransPoint(const RunParamsStruct_RobotCalib &RunParams, double &Output_X, double &Output_Y)
{
	try
	{
        HTuple Qx, Qy;
        AffineTransPoint2d(RunParams.hv_HomMat2D, RunParams.hv_NowX, RunParams.hv_NowY, &Qx, &Qy);
		Output_X = Qx.D();
		Output_Y = Qy.D();
		return 0;
	}
	catch (...)
	{
        Output_X = -99999;
        Output_Y = -99999;
		return 1;
	}
}
//查找模板
int RobotCalibDetect::FindTemplate(const cv::Mat &InputImage, const RunParamsStruct_RobotCalib &RunParams, ResultParamsStruct_RobotCalib &ResultParams)
{
	//mutex1.lock();
	try
	{
		ResultParams.hv_RetNum = 0;
		ResultParams.hv_Row.clear();
		ResultParams.hv_Column.clear();
		ResultParams.hv_Angle.clear();
		ResultParams.hv_Score.clear();
		ResultParams.hv_Angle_Original = 0;
		ResultParams.hv_CenterRow_Original = 0;
		ResultParams.hv_CenterCol_Original = 0;
		ResultParams.hv_ROIHomMat2D = NULL;
		HObject ho_Region_Template;
		GenEmptyObj(&ho_Region_Template);
		ho_Region_Template.Clear();
		//检测代码
		ResultParams.DestImg = InputImage;
		if (RunParams.hv_ModelID == NULL)
		{
			//模板句柄为空
			ResultParams.hv_RetNum = 0;
			mResultParams = ResultParams;
			//mutex1.unlock();
			return 1;
		}
		HObject ho_DrawImg = PaintDetect1->Mat2HObject(InputImage);
		HObject ho_Image = ho_DrawImg.Clone();
		HTuple channles;
		CountChannels(ho_Image, &channles);
		if (channles == 3)
		{
			Rgb1ToGray(ho_Image, &ho_Image);
		}
		//把原始的建模坐标和角度赋值到结果结构体
		ResultParams.hv_CenterRow_Original = RunParams.hv_CenterRow_Original;
		ResultParams.hv_CenterCol_Original = RunParams.hv_CenterCol_Original;
		ResultParams.hv_Angle_Original = RunParams.hv_Angle_Original;

		//下面时是找模板代码
		HTuple Homat2d_Roi;
		HTuple Homat2d_Window;
		HObject mho_Image;
		if (RunParams.hv_IsUsSearchRoi == 1)
		{
			HObject ho_SearchRegion;
			GenRectangle1(&ho_SearchRegion, RunParams.P1_SearchRoi.y, RunParams.P1_SearchRoi.x, RunParams.P2_SearchRoi.y, RunParams.P2_SearchRoi.x);
			ReduceDomain(ho_Image, ho_SearchRegion, &mho_Image);
		}
		else
		{
			mho_Image = ho_Image;
		}
		//设置超时时间
		if (RunParams.hv_MatchMethod == "ncc")
		{
			//设置查询超时时间1000ms
			SetNccModelParam(RunParams.hv_ModelID, "timeout", RunParams.hv_TimeOut);
		}
		else
		{
			//设置查询超时时间1000ms
			SetShapeModelParam(RunParams.hv_ModelID, "timeout", RunParams.hv_TimeOut);
			//设置允许部分位于图像之外
			SetShapeModelParam(RunParams.hv_ModelID, "border_shape_models", "true");
		}

		if (RunParams.hv_MatchMethod == "aniso")
		{
			//设置亚像素和允许轮廓变形系数(以像素为单位，范围是0-32个像素),为了获得有意义的分值并避免错误的匹配，我们建议始终将变形量与最小二乘调整相结合
			HTuple SubPixelTemp;
			if (RunParams.hv_SubPixel == "least_squares" || RunParams.hv_SubPixel == "least_squares_high" || RunParams.hv_SubPixel == "least_squares_very_high")
			{
				SubPixelTemp = (HTuple(RunParams.hv_SubPixel.c_str()).Append("max_deformation 2"));
			}
			else
			{
				SubPixelTemp = HTuple(RunParams.hv_SubPixel.c_str());
			}
			//获取模板参数
			HTuple row, col, angle, score, scaleR, scaleC;
			HTuple NumLevelsTemp, AngleStartTemp, AngleExtentTemp, AngleStepTemp, ScaleMinTemp, ScaleMaxTemp, ScaleStepTemp, MetricTemp, MinContrastTemp;
			GetShapeModelParams(RunParams.hv_ModelID, &NumLevelsTemp, &AngleStartTemp, &AngleExtentTemp, &AngleStepTemp, &ScaleMinTemp,
				&ScaleMaxTemp, &ScaleStepTemp, &MetricTemp, &MinContrastTemp);
			FindAnisoShapeModel(mho_Image, RunParams.hv_ModelID, HTuple(RunParams.hv_AngleStart).TupleRad(), 2 * (HTuple(RunParams.hv_AngleExtent).TupleRad()).TupleAbs(),
				RunParams.hv_ScaleRMin, RunParams.hv_ScaleRMax, RunParams.hv_ScaleCMin, RunParams.hv_ScaleCMax, RunParams.hv_MinScore,
				RunParams.hv_NumMatches, RunParams.hv_MaxOverlap, SubPixelTemp, (HTuple(NumLevelsTemp).Append(1)), RunParams.hv_Greediness,
				&row, &col, &angle, &scaleR, &scaleC,
				&score);

			HTuple _length = 0;
			TupleLength(row, &_length);
			ResultParams.hv_RetNum = _length.I();
			if (_length == 0)
			{
				ResultParams.hv_RetNum = 0;
				//mutex1.unlock();
				mResultParams = ResultParams;
				return 1;
			}
			else
			{
				GenEmptyObj(&ho_Region_Template);
				for (int i = 0; i < _length; i++)
				{
					//弧度转角度
					HTuple tempRad_Angle = angle[i];
					HTuple tempDeg_Angle = tempRad_Angle.TupleDeg();
					if (tempDeg_Angle.TupleAbs() <= 0.01)
					{
						tempDeg_Angle = 0;
					}

                    ResultParams.hv_Row.push_back(HTuple(row[i]).D());
                    ResultParams.hv_Column.push_back(HTuple(col[i]).D());
                    ResultParams.hv_Angle.push_back(tempDeg_Angle.D());
                    ResultParams.hv_Score.push_back(HTuple(score[i]).D());

					//生成ROI仿射变换矩阵(用于抓边跟随)
					Homat2d_Roi.Clear();
					VectorAngleToRigid(RunParams.hv_CenterRow_Original, RunParams.hv_CenterCol_Original, 0, row[i], col[i], tempRad_Angle,
						&Homat2d_Roi);
					ResultParams.hv_ROIHomMat2D.Append(Homat2d_Roi);

					//生成ROI仿射变换矩阵(用于显示模板轮廓)，Halcon默认模板图形显示在左上角(0,0,0),利用仿射变换把他显示到图像中间
					Homat2d_Window.Clear();
					VectorAngleToRigid(0, 0, 0, row[i], col[i], tempRad_Angle,
						&Homat2d_Window);
					//获取模板轮廓
					HObject ho_ContourRet;
					GetShapeModelContours(&ho_ContourRet, RunParams.hv_ModelID, 1);
					//仿射变换
					AffineTransContourXld(ho_ContourRet, &ho_ContourRet, Homat2d_Window);
					//获取模板区域
					HTuple countXld;
					CountObj(ho_ContourRet, &countXld);
					HObject region1;
					GenEmptyObj(&region1);
					//for (int i = 0; i < countXld; i++)
					//{
					//	HObject xldSelect;
					//	SelectObj(ho_ContourRet, &xldSelect, i + 1);
					//	//HObject region2;
					//	//GenRegionContourXld(xldSelect, &region2, "filled");
					//	//ConcatObj(region1, region2, &region1);
					//	HTuple rowXld, colXld;
					//	GetContourXld(xldSelect, &rowXld, &colXld);
					//	HObject region2;
					//	GenRegionPoints(&region2, rowXld, colXld);
					//	ConcatObj(region1, region2, &region1);
					//}
					GenRegionContourXld(ho_ContourRet, &region1, "margin");
					HObject regionUnion;
					Union1(region1, &regionUnion);
					ConcatObj(ho_Region_Template, regionUnion, &ho_Region_Template);
				}
			}
		}
		else if (RunParams.hv_MatchMethod == "ncc")
		{
			HTuple row, col, angle, score;
			HTuple NumLevelsTemp, AngleStartTemp, AngleExtentTemp, AngleStepTemp, MetricTemp;
			GetNccModelParams(RunParams.hv_ModelID, &NumLevelsTemp, &AngleStartTemp, &AngleExtentTemp, &AngleStepTemp, &MetricTemp);
			FindNccModel(mho_Image, RunParams.hv_ModelID, HTuple(RunParams.hv_AngleStart).TupleRad(), 2 * (HTuple(RunParams.hv_AngleExtent).TupleRad()).TupleAbs(),
				RunParams.hv_MinScore, RunParams.hv_NumMatches, RunParams.hv_MaxOverlap, "true", (HTuple(NumLevelsTemp).Append(1)),
				&row, &col, &angle, &score);

			HTuple _length = 0;
			TupleLength(row, &_length);
			ResultParams.hv_RetNum = _length;
			if (_length == 0)
			{
				ResultParams.hv_RetNum = 0;
				//mutex1.unlock();
				mResultParams = ResultParams;
				return 1;
			}
			else
			{
				GenEmptyObj(&ho_Region_Template);
				for (int i = 0; i < _length; i++)
				{
					//弧度转角度
					HTuple tempRad_Angle = angle[i];
					HTuple tempDeg_Angle = tempRad_Angle.TupleDeg();
					if (tempDeg_Angle.TupleAbs() <= 0.01)
					{
						tempDeg_Angle = 0;
					}

					ResultParams.hv_Row.push_back(row[i]);
					ResultParams.hv_Column.push_back(col[i]);
					ResultParams.hv_Angle.push_back(tempDeg_Angle);
					ResultParams.hv_Score.push_back(score[i]);

					//生成ROI仿射变换矩阵(用于抓边跟随)
					Homat2d_Roi.Clear();
					VectorAngleToRigid(RunParams.hv_CenterRow_Original, RunParams.hv_CenterCol_Original, 0, row[i], col[i], tempRad_Angle,
						&Homat2d_Roi);
					ResultParams.hv_ROIHomMat2D.Append(Homat2d_Roi);

					//生成ROI仿射变换矩阵(用于显示模板轮廓)，Halcon默认模板图形显示在左上角(0,0,0),利用仿射变换把他显示到图像中间
					Homat2d_Window.Clear();
					VectorAngleToRigid(0, 0, 0, row[i], col[i], tempRad_Angle,
						&Homat2d_Window);
					//获取模板区域
					HObject ho_RegionRet;
					GetNccModelRegion(&ho_RegionRet, RunParams.hv_ModelID);
					HObject regionAffine;
					AffineTransRegion(ho_RegionRet, &regionAffine, Homat2d_Window, "nearest_neighbor");
					ConcatObj(ho_Region_Template, regionAffine, &ho_Region_Template);
				}
			}
		}
		else
		{
			/*	qDebug() << "查找模板，没有正确选择模板类型 ";*/
			ResultParams.hv_RetNum = 0;
			mResultParams = ResultParams;
			//mutex1.unlock();
			return 1;
		}
		//画结果图
		RunParamsStruct_PaintDetect runParams_Pain;
		runParams_Pain.vec_InputRegion.clear();
		runParams_Pain.vec_Ret.clear();
		runParams_Pain.hv_LineWidth = 5;
		runParams_Pain.hv_Word_Size = 10;
		runParams_Pain.hv_Word_X = 100;
		runParams_Pain.hv_Word_Y = 100;
		//不画ok/ng字符串
		runParams_Pain.hv_Ret = -1;
		if (ho_Region_Template.Key() != nullptr)
		{
			runParams_Pain.vec_Ret.push_back(1);
			HObject unionRegion;
			Union1(ho_Region_Template, &unionRegion);
			runParams_Pain.vec_InputRegion.push_back(unionRegion);
		}
		PaintDetect1->PaintImage(ho_DrawImg, runParams_Pain, ResultParams.DestImg);

		mResultParams = ResultParams;
		return 0;
	}
	catch (...)
	{
		ResultParams.hv_RetNum = 0;
		ResultParams.hv_Row.clear();
		ResultParams.hv_Column.clear();
		ResultParams.hv_Angle.clear();
		ResultParams.hv_Score.clear();
		ResultParams.hv_Angle_Original = 0;
		ResultParams.hv_CenterRow_Original = 0;
		ResultParams.hv_CenterCol_Original = 0;
		ResultParams.hv_ROIHomMat2D = NULL;

		mResultParams = ResultParams;
		//mutex1.unlock();
		return -1;
	}
}
//创建模板
int RobotCalibDetect::CreateTemplate(const cv::Mat &InputImage, RunParamsStruct_RobotCalib &RunParams)
{
	try
	{
		if (RunParams.VecRows_TrainRoi.size() <= 0 || RunParams.VecColumns_TrainRoi.size() <= 0)
		{
			return 1;
		}
		if (RunParams.VecRows_TrainRoi.size() != RunParams.VecColumns_TrainRoi.size())
		{
			return 1;
		}
		HObject ho_Image = PaintDetect1->Mat2HObject(InputImage);
		HTuple channles;
		CountChannels(ho_Image, &channles);
		if (channles == 3)
		{
			Rgb1ToGray(ho_Image, &ho_Image);
		}
		HObject RegionTrain;
		HTuple rows, columns;
		rows = HTuple((Hlong*)RunParams.VecRows_TrainRoi.data(), RunParams.VecRows_TrainRoi.size());
		columns = HTuple((Hlong*)RunParams.VecColumns_TrainRoi.data(), RunParams.VecColumns_TrainRoi.size());
		GenRegionPoints(&RegionTrain, rows, columns);
		//输出模板中心坐标
		HTuple area, row, col;
		AreaCenter(RegionTrain, &area, &row, &col);
		RunParams.hv_CenterRow_Original = row;
		RunParams.hv_CenterCol_Original = col;
		//裁切训练图
		HObject ImageTrain;
		ReduceDomain(ho_Image, RegionTrain, &ImageTrain);
		//训练模板
		HTuple numLevels, contrast, contrastMin;
		if (RunParams.hv_NumLevels == -1)
		{
			numLevels = "auto";
		}
		else
		{
			numLevels = RunParams.hv_NumLevels;
		}
		if (RunParams.hv_Contrast == 0)
		{
			contrast = "auto";
		}
		else
		{
			contrast = RunParams.hv_Contrast;
		}
		if (RunParams.hv_MinContrast == 0)
		{
			contrastMin = "auto";
		}
		else
		{
			contrastMin = RunParams.hv_MinContrast;
		}

		if (RunParams.hv_MatchMethod == "aniso")
		{
			CreateAnisoShapeModel(ImageTrain, numLevels, HTuple(RunParams.hv_AngleStart).TupleRad(), 2 * (HTuple(RunParams.hv_AngleExtent).TupleRad()).TupleAbs(),
				"auto", RunParams.hv_ScaleRMin, RunParams.hv_ScaleRMax, "auto", RunParams.hv_ScaleCMin,
				RunParams.hv_ScaleCMax, "auto", "auto", HTuple(RunParams.hv_Metric.c_str()), contrast, contrastMin,
				&RunParams.hv_ModelID);
			return 0;
		}
		else if (RunParams.hv_MatchMethod == "ncc")
		{
			CreateNccModel(ImageTrain, numLevels, HTuple(RunParams.hv_AngleStart).TupleRad(), 2 * (HTuple(RunParams.hv_AngleExtent).TupleRad()).TupleAbs(),
				"auto", "use_polarity", &RunParams.hv_ModelID);

			return 0;
		}
		else
		{
			return 1;
		}
	}
	catch (...)
	{
		return -1;
	}

}
//保存机器人定位引导参数(runParams 定位引导参数，processId 流程ID, nodeName 模块名称， processModbuleID 模块ID)
int RobotCalibDetect::WriteParams_RobotCalib(const string ConfigPath, const string XmlPath, const RunParamsStruct_RobotCalib &runParams, const int& processId, const string& nodeName, const int& processModbuleID)
{
	//mutex1.lock();
	try
	{
		XmlClass2 xmlC;//xml存取变量
		//路径判断
		PathSet(ConfigPath, XmlPath);
		//*************************标定参数保存*******************************************************************
		int typeCount = runParams.hv_NPointCamera_X.TupleLength().TupleInt().I();
		if (typeCount == 0)
		{
			return 1;
		}
		if (typeCount == 1)
		{
			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_NPointCamera_X", to_string(runParams.hv_NPointCamera_X.D()));
		}
		else
		{
			HTuple typeTemp = runParams.hv_NPointCamera_X[0] + ",";
			for (int i = 1; i < typeCount; i++)
			{
				if (i == typeCount - 1)
				{
					typeTemp += runParams.hv_NPointCamera_X[i];
				}
				else
				{
					typeTemp += runParams.hv_NPointCamera_X[i] + ",";
				}
			}
			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_NPointCamera_X", typeTemp.S().Text());
		}

		typeCount = runParams.hv_NPointCamera_Y.TupleLength().TupleInt().I();
		if (typeCount == 0)
		{
			return 1;
		}
		if (typeCount == 1)
		{
			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_NPointCamera_Y", to_string(runParams.hv_NPointCamera_Y.D()));
		}
		else
		{
			HTuple typeTemp = runParams.hv_NPointCamera_Y[0] + ",";
			for (int i = 1; i < typeCount; i++)
			{
				if (i == typeCount - 1)
				{
					typeTemp += runParams.hv_NPointCamera_Y[i];
				}
				else
				{
					typeTemp += runParams.hv_NPointCamera_Y[i] + ",";
				}
			}
			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_NPointCamera_Y", typeTemp.S().Text());
		}

		typeCount = runParams.hv_NPointRobot_X.TupleLength().TupleInt().I();
		if (typeCount == 0)
		{
			return 1;
		}
		if (typeCount == 1)
		{
			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_NPointRobot_X", to_string(runParams.hv_NPointRobot_X.D()));
		}
		else
		{
			HTuple typeTemp = runParams.hv_NPointRobot_X[0] + ",";
			for (int i = 1; i < typeCount; i++)
			{
				if (i == typeCount - 1)
				{
					typeTemp += runParams.hv_NPointRobot_X[i];
				}
				else
				{
					typeTemp += runParams.hv_NPointRobot_X[i] + ",";
				}
			}
			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_NPointRobot_X", typeTemp.S().Text());
		}

		typeCount = runParams.hv_NPointRobot_Y.TupleLength().TupleInt().I();
		if (typeCount == 0)
		{
			return 1;
		}
		if (typeCount == 1)
		{
			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_NPointRobot_Y", to_string(runParams.hv_NPointRobot_Y.D()));
		}
		else
		{
			HTuple typeTemp = runParams.hv_NPointRobot_Y[0] + ",";
			for (int i = 1; i < typeCount; i++)
			{
				if (i == typeCount - 1)
				{
					typeTemp += runParams.hv_NPointRobot_Y[i];
				}
				else
				{
					typeTemp += runParams.hv_NPointRobot_Y[i] + ",";
				}
			}
			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_NPointRobot_Y", typeTemp.S().Text());
		}

		typeCount = runParams.hv_CenterCamera_X.TupleLength().TupleInt().I();
		if (typeCount == 0)
		{
			return 1;
		}
		if (typeCount == 1)
		{
			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_CenterCamera_X", to_string(runParams.hv_CenterCamera_X.D()));
		}
		else
		{
			HTuple typeTemp = runParams.hv_CenterCamera_X[0] + ",";
			for (int i = 1; i < typeCount; i++)
			{
				if (i == typeCount - 1)
				{
					typeTemp += runParams.hv_CenterCamera_X[i];
				}
				else
				{
					typeTemp += runParams.hv_CenterCamera_X[i] + ",";
				}
			}
			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_CenterCamera_X", typeTemp.S().Text());
		}

		typeCount = runParams.hv_CenterCamera_Y.TupleLength().TupleInt().I();
		if (typeCount == 0)
		{
			return 1;
		}
		if (typeCount == 1)
		{
			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_CenterCamera_Y", to_string(runParams.hv_CenterCamera_Y.D()));
		}
		else
		{
			HTuple typeTemp = runParams.hv_CenterCamera_Y[0] + ",";
			for (int i = 1; i < typeCount; i++)
			{
				if (i == typeCount - 1)
				{
					typeTemp += runParams.hv_CenterCamera_Y[i];
				}
				else
				{
					typeTemp += runParams.hv_CenterCamera_Y[i] + ",";
				}
			}
			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_CenterCamera_Y", typeTemp.S().Text());
		}

		typeCount = runParams.hv_CenterRobot_X.TupleLength().TupleInt().I();
		if (typeCount == 0)
		{
			return 1;
		}
		if (typeCount == 1)
		{
			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_CenterRobot_X", to_string(runParams.hv_CenterRobot_X.D()));
		}
		else
		{
			HTuple typeTemp = runParams.hv_CenterRobot_X[0] + ",";
			for (int i = 1; i < typeCount; i++)
			{
				if (i == typeCount - 1)
				{
					typeTemp += runParams.hv_CenterRobot_X[i];
				}
				else
				{
					typeTemp += runParams.hv_CenterRobot_X[i] + ",";
				}
			}
			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_CenterRobot_X", typeTemp.S().Text());
		}

		typeCount = runParams.hv_CenterRobot_Y.TupleLength().TupleInt().I();
		if (typeCount == 0)
		{
			return 1;
		}
		if (typeCount == 1)
		{
			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_CenterRobot_Y", to_string(runParams.hv_CenterRobot_Y.D()));
		}
		else
		{
			HTuple typeTemp = runParams.hv_CenterRobot_Y[0] + ",";
			for (int i = 1; i < typeCount; i++)
			{
				if (i == typeCount - 1)
				{
					typeTemp += runParams.hv_CenterRobot_Y[i];
				}
				else
				{
					typeTemp += runParams.hv_CenterRobot_Y[i] + ",";
				}
			}
			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_CenterRobot_Y", typeTemp.S().Text());
		}
		//保存其余参数
		xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_Center_X", to_string(runParams.hv_Center_X.D()));
		xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_Center_Y", to_string(runParams.hv_Center_Y.D()));
		xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_Center_Radius", to_string(runParams.hv_Center_Radius.D()));
		xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_ModelX", to_string(runParams.hv_ModelX.D()));
		xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_ModelY", to_string(runParams.hv_ModelY.D()));
		xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_ModelA", to_string(runParams.hv_ModelA.D()));
		xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_ModelPosition_RobotX", to_string(runParams.hv_ModelPosition_RobotX.D()));
		xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_ModelPosition_RobotY", to_string(runParams.hv_ModelPosition_RobotY.D()));
		xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_ModelPosition_RobotA", to_string(runParams.hv_ModelPosition_RobotA.D()));
		xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_CenterPosition_RobotX", to_string(runParams.hv_CenterPosition_RobotX.D()));
		xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_CenterPosition_RobotY", to_string(runParams.hv_CenterPosition_RobotY.D()));
		xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_CenterPosition_RobotA", to_string(runParams.hv_CenterPosition_RobotA.D()));
		xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_DistancePP_Robot", to_string(runParams.hv_DistancePP_Robot.D()));
		xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_AngleRotate_Robot", to_string(runParams.hv_AngleRotate_Robot.D()));
		if (runParams.hv_IsCamFirst)
		{
			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_IsCamFirst", "true");
		}
		else
		{
			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_IsCamFirst", "false");
		}

		if (runParams.hv_IsReverseAngle)
		{
			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_IsReverseAngle", "true");
		}
		else
		{
			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_IsReverseAngle", "false");
		}

		if (runParams.hv_IsAbsCoordinate)
		{
			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_IsAbsCoordinate", "true");
		}
		else
		{
			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_IsAbsCoordinate", "false");
		}

		if (runParams.hv_IsUseCenterCalib)
		{
			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_IsUseCenterCalib", "true");
		}
		else
		{
			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_IsUseCenterCalib", "false");
		}

		//保存九点标定矩阵
		HTuple PathTuple = HTuple(ConfigPath.c_str()) + HTuple(to_string(processId).c_str()) + HTuple(nodeName.c_str()) + HTuple(to_string(processModbuleID).c_str()) + "_CalibId.tuple";
		WriteTuple(runParams.hv_HomMat2D, PathTuple);

		//*************************模板匹配参数保存**************************************************************************************
		xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_MatchMethod", runParams.hv_MatchMethod);
		xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_IsUsSearchRoi", to_string(runParams.hv_IsUsSearchRoi));
		xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_NumLevels", to_string(runParams.hv_NumLevels));

		xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_AngleStart", to_string(runParams.hv_AngleStart));
		xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_AngleExtent", to_string(runParams.hv_AngleExtent));

		xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_ScaleRMin", to_string(runParams.hv_ScaleRMin));
		xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_ScaleRMax", to_string(runParams.hv_ScaleRMax));

		xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_ScaleCMin", to_string(runParams.hv_ScaleCMin));
		xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_ScaleCMax", to_string(runParams.hv_ScaleCMax));

		xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_Metric", runParams.hv_Metric);

		xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_Contrast", to_string(runParams.hv_Contrast));
		xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_MinContrast", to_string(runParams.hv_MinContrast));

		//保存模板原始坐标和角度
		xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_Angle_Original", to_string(runParams.hv_Angle_Original));
		xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_CenterRow_Original", to_string(runParams.hv_CenterRow_Original));
		xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_CenterCol_Original", to_string(runParams.hv_CenterCol_Original));
		//保存运行参数
		xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_MinScore", to_string(runParams.hv_MinScore));
		xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_NumMatches", to_string(runParams.hv_NumMatches));
		xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_MaxOverlap", to_string(runParams.hv_MaxOverlap));
		xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_SubPixel", runParams.hv_SubPixel);
		xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_Greediness", to_string(runParams.hv_Greediness));
		xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_TimeOut", to_string(runParams.hv_TimeOut));

		//保存运行区域
		xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "P1x_SearchRoi", to_string(runParams.P1_SearchRoi.x));
		xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "P1y_SearchRoi", to_string(runParams.P1_SearchRoi.y));
		xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "P2x_SearchRoi", to_string(runParams.P2_SearchRoi.x));
		xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "P2y_SearchRoi", to_string(runParams.P2_SearchRoi.y));

		//保存训练好的模板ID模型
		if (runParams.hv_MatchMethod == "ncc")
		{
			HTuple modelIDPath = HTuple(ConfigPath.c_str()) + HTuple(to_string(processId).c_str()) + "_" + HTuple(nodeName.c_str()) + HTuple(to_string(processModbuleID).c_str()) + "_ModelId.ncm";
			WriteNccModel(runParams.hv_ModelID, modelIDPath);
		}
		else
		{
			HTuple modelIDPath = HTuple(ConfigPath.c_str()) + HTuple(to_string(processId).c_str()) + "_" + HTuple(nodeName.c_str()) + HTuple(to_string(processModbuleID).c_str()) + "_ModelId.shm";
			WriteShapeModel(runParams.hv_ModelID, modelIDPath);
		}
		//mutex1.unlock();
		return 0;
	}
	catch (...)
	{
		//mutex1.unlock();
		return -1;
	}
}
//读取机器人定位参数(runParams 定位引导参数，processId 流程ID, nodeName 模块名称， processModbuleID 模块ID, value 输出读取到的值)
ErrorCode_Xml RobotCalibDetect::ReadParams_RobotCalib(const string ConfigPath, const string XmlPath, RunParamsStruct_RobotCalib &runParams, const int& processId, const string& nodeName, const int& processModbuleID)
{
	//mutex1.lock();
	try
	{
		XmlClass2 xmlC;//xml存取变量
		//路径判断
		PathSet(ConfigPath, XmlPath);

		string valueXml;
		ErrorCode_Xml errorCode;
		int tempIntValue;
		double tempDoubleValue;

		//****************************读取标定参数****************************************************************************************
		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_NPointCamera_X", valueXml);
		if (errorCode != Ok_Xml)
		{
			return errorCode;
		}
		vector<string> typeArray;
		stringToken(valueXml, ",", typeArray);
		runParams.hv_NPointCamera_X.Clear();
		for (int i = 0; i < typeArray.size(); i++)
		{
			xmlC.StrToDouble(typeArray[i], tempDoubleValue);
			runParams.hv_NPointCamera_X.Append(tempDoubleValue);
		}
		int typeCount = runParams.hv_NPointCamera_X.TupleLength().TupleInt().I();
		if (typeCount == 0)
		{
			return nullKeyName_Xml;
		}

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_NPointCamera_Y", valueXml);
		if (errorCode != Ok_Xml)
		{
			return errorCode;
		}
		typeArray.clear();
		stringToken(valueXml, ",", typeArray);
		runParams.hv_NPointCamera_Y.Clear();
		for (int i = 0; i < typeArray.size(); i++)
		{
			xmlC.StrToDouble(typeArray[i], tempDoubleValue);
			runParams.hv_NPointCamera_Y.Append(tempDoubleValue);
		}
		typeCount = runParams.hv_NPointCamera_Y.TupleLength().TupleInt().I();
		if (typeCount == 0)
		{
			return nullKeyName_Xml;
		}

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_NPointRobot_X", valueXml);
		if (errorCode != Ok_Xml)
		{
			return errorCode;
		}
		typeArray.clear();
		stringToken(valueXml, ",", typeArray);
		runParams.hv_NPointRobot_X.Clear();
		for (int i = 0; i < typeArray.size(); i++)
		{
			xmlC.StrToDouble(typeArray[i], tempDoubleValue);
			runParams.hv_NPointRobot_X.Append(tempDoubleValue);
		}
		typeCount = runParams.hv_NPointRobot_X.TupleLength().TupleInt().I();
		if (typeCount == 0)
		{
			return nullKeyName_Xml;
		}

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_NPointRobot_Y", valueXml);
		if (errorCode != Ok_Xml)
		{
			return errorCode;
		}
		typeArray.clear();
		stringToken(valueXml, ",", typeArray);
		runParams.hv_NPointRobot_Y.Clear();
		for (int i = 0; i < typeArray.size(); i++)
		{
			xmlC.StrToDouble(typeArray[i], tempDoubleValue);
			runParams.hv_NPointRobot_Y.Append(tempDoubleValue);
		}
		typeCount = runParams.hv_NPointRobot_Y.TupleLength().TupleInt().I();
		if (typeCount == 0)
		{
			return nullKeyName_Xml;
		}

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_CenterCamera_X", valueXml);
		if (errorCode != Ok_Xml)
		{
			return errorCode;
		}
		typeArray.clear();
		stringToken(valueXml, ",", typeArray);
		runParams.hv_CenterCamera_X.Clear();
		for (int i = 0; i < typeArray.size(); i++)
		{
			xmlC.StrToDouble(typeArray[i], tempDoubleValue);
			runParams.hv_CenterCamera_X.Append(tempDoubleValue);
		}
		typeCount = runParams.hv_CenterCamera_X.TupleLength().TupleInt().I();
		if (typeCount == 0)
		{
			return nullKeyName_Xml;
		}

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_CenterCamera_Y", valueXml);
		if (errorCode != Ok_Xml)
		{
			return errorCode;
		}
		typeArray.clear();
		stringToken(valueXml, ",", typeArray);
		runParams.hv_CenterCamera_Y.Clear();
		for (int i = 0; i < typeArray.size(); i++)
		{
			xmlC.StrToDouble(typeArray[i], tempDoubleValue);
			runParams.hv_CenterCamera_Y.Append(tempDoubleValue);
		}
		typeCount = runParams.hv_CenterCamera_Y.TupleLength().TupleInt().I();
		if (typeCount == 0)
		{
			return nullKeyName_Xml;
		}

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_CenterRobot_X", valueXml);
		if (errorCode != Ok_Xml)
		{
			return errorCode;
		}
		typeArray.clear();
		stringToken(valueXml, ",", typeArray);
		runParams.hv_CenterRobot_X.Clear();
		for (int i = 0; i < typeArray.size(); i++)
		{
			xmlC.StrToDouble(typeArray[i], tempDoubleValue);
			runParams.hv_CenterRobot_X.Append(tempDoubleValue);
		}
		typeCount = runParams.hv_CenterRobot_X.TupleLength().TupleInt().I();
		if (typeCount == 0)
		{
			return nullKeyName_Xml;
		}

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_CenterRobot_Y", valueXml);
		if (errorCode != Ok_Xml)
		{
			return errorCode;
		}
		typeArray.clear();
		stringToken(valueXml, ",", typeArray);
		runParams.hv_CenterRobot_Y.Clear();
		for (int i = 0; i < typeArray.size(); i++)
		{
			xmlC.StrToDouble(typeArray[i], tempDoubleValue);
			runParams.hv_CenterRobot_Y.Append(tempDoubleValue);
		}
		typeCount = runParams.hv_CenterRobot_Y.TupleLength().TupleInt().I();
		if (typeCount == 0)
		{
			return nullKeyName_Xml;
		}

		//读取其余参数
		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_Center_X", valueXml);
		if (errorCode != Ok_Xml)
		{
			return errorCode;
		}
		xmlC.StrToDouble(valueXml, tempDoubleValue);
		runParams.hv_Center_X = tempDoubleValue;

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_Center_Y", valueXml);
		if (errorCode != Ok_Xml)
		{
			return errorCode;
		}
		xmlC.StrToDouble(valueXml, tempDoubleValue);
		runParams.hv_Center_Y = tempDoubleValue;

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_Center_Radius", valueXml);
		if (errorCode != Ok_Xml)
		{
			return errorCode;
		}
		xmlC.StrToDouble(valueXml, tempDoubleValue);
		runParams.hv_Center_Radius = tempDoubleValue;

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_ModelX", valueXml);
		if (errorCode != Ok_Xml)
		{
			return errorCode;
		}
		xmlC.StrToDouble(valueXml, tempDoubleValue);
		runParams.hv_ModelX = tempDoubleValue;

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_ModelY", valueXml);
		if (errorCode != Ok_Xml)
		{
			return errorCode;
		}
		xmlC.StrToDouble(valueXml, tempDoubleValue);
		runParams.hv_ModelY = tempDoubleValue;

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_ModelA", valueXml);
		if (errorCode != Ok_Xml)
		{
			return errorCode;
		}
		xmlC.StrToDouble(valueXml, tempDoubleValue);
		runParams.hv_ModelA = tempDoubleValue;

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_ModelPosition_RobotX", valueXml);
		if (errorCode != Ok_Xml)
		{
			return errorCode;
		}
		xmlC.StrToDouble(valueXml, tempDoubleValue);
		runParams.hv_ModelPosition_RobotX = tempDoubleValue;

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_ModelPosition_RobotY", valueXml);
		if (errorCode != Ok_Xml)
		{
			return errorCode;
		}
		xmlC.StrToDouble(valueXml, tempDoubleValue);
		runParams.hv_ModelPosition_RobotY = tempDoubleValue;

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_ModelPosition_RobotA", valueXml);
		if (errorCode != Ok_Xml)
		{
			return errorCode;
		}
		xmlC.StrToDouble(valueXml, tempDoubleValue);
		runParams.hv_ModelPosition_RobotA = tempDoubleValue;

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_CenterPosition_RobotX", valueXml);
		if (errorCode != Ok_Xml)
		{
			return errorCode;
		}
		xmlC.StrToDouble(valueXml, tempDoubleValue);
		runParams.hv_CenterPosition_RobotX = tempDoubleValue;

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_CenterPosition_RobotY", valueXml);
		if (errorCode != Ok_Xml)
		{
			return errorCode;
		}
		xmlC.StrToDouble(valueXml, tempDoubleValue);
		runParams.hv_CenterPosition_RobotY = tempDoubleValue;

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_CenterPosition_RobotA", valueXml);
		if (errorCode != Ok_Xml)
		{
			return errorCode;
		}
		xmlC.StrToDouble(valueXml, tempDoubleValue);
		runParams.hv_CenterPosition_RobotA = tempDoubleValue;

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_DistancePP_Robot", valueXml);
		if (errorCode != Ok_Xml)
		{
			return errorCode;
		}
		xmlC.StrToDouble(valueXml, tempDoubleValue);
		runParams.hv_DistancePP_Robot = tempDoubleValue;

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_AngleRotate_Robot", valueXml);
		if (errorCode != Ok_Xml)
		{
			return errorCode;
		}
		xmlC.StrToDouble(valueXml, tempDoubleValue);
		runParams.hv_AngleRotate_Robot = tempDoubleValue;

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_IsCamFirst", valueXml);
		if (errorCode != Ok_Xml)
		{
			return errorCode;
		}
		if (valueXml == "true")
		{
			runParams.hv_IsCamFirst = true;
		}
		else
		{
			runParams.hv_IsCamFirst = false;
		}

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_IsReverseAngle", valueXml);
		if (errorCode != Ok_Xml)
		{
			return errorCode;
		}
		if (valueXml == "true")
		{
			runParams.hv_IsReverseAngle = true;
		}
		else
		{
			runParams.hv_IsReverseAngle = false;
		}

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_IsAbsCoordinate", valueXml);
		if (errorCode != Ok_Xml)
		{
			return errorCode;
		}
		if (valueXml == "true")
		{
			runParams.hv_IsAbsCoordinate = true;
		}
		else
		{
			runParams.hv_IsAbsCoordinate = false;
		}

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_IsUseCenterCalib", valueXml);
		if (errorCode != Ok_Xml)
		{
			return errorCode;
		}
		if (valueXml == "true")
		{
			runParams.hv_IsUseCenterCalib = true;
		}
		else
		{
			runParams.hv_IsUseCenterCalib = false;
		}

		//九点标定矩阵加载
		HTuple PathTuple = HTuple(ConfigPath.c_str()) + HTuple(to_string(processId).c_str()) + HTuple(nodeName.c_str()) + HTuple(to_string(processModbuleID).c_str()) + "_CalibId.tuple";
		HTuple isExist;
		FileExists(PathTuple, &isExist);
		if (isExist > 0)
		{
			ReadTuple(PathTuple, &runParams.hv_HomMat2D);
		}

		//********************************读取模板匹配参数*******************************************************************************
		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_MatchMethod", valueXml);
		if (errorCode != Ok_Xml)
		{
			return errorCode;
		}
		runParams.hv_MatchMethod = valueXml;

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_IsUsSearchRoi", valueXml);
		if (errorCode != Ok_Xml)
		{
			return errorCode;
		}
		xmlC.StrToInt(valueXml, tempIntValue);
		runParams.hv_IsUsSearchRoi = tempIntValue;


		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_NumLevels", valueXml);
		if (errorCode != Ok_Xml)
		{
			return errorCode;
		}
		xmlC.StrToInt(valueXml, tempIntValue);
		runParams.hv_NumLevels = tempIntValue;

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_AngleStart", valueXml);
		if (errorCode != Ok_Xml)
		{
			return errorCode;
		}
		xmlC.StrToDouble(valueXml, tempDoubleValue);
		runParams.hv_AngleStart = tempDoubleValue;

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_AngleExtent", valueXml);
		if (errorCode != Ok_Xml)
		{
			return errorCode;
		}
		xmlC.StrToDouble(valueXml, tempDoubleValue);
		runParams.hv_AngleExtent = tempDoubleValue;

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_ScaleRMin", valueXml);
		if (errorCode != Ok_Xml)
		{
			return errorCode;
		}
		xmlC.StrToDouble(valueXml, tempDoubleValue);
		runParams.hv_ScaleRMin = tempDoubleValue;

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_ScaleRMax", valueXml);
		if (errorCode != Ok_Xml)
		{
			return errorCode;
		}
		xmlC.StrToDouble(valueXml, tempDoubleValue);
		runParams.hv_ScaleRMax = tempDoubleValue;

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_ScaleCMin", valueXml);
		if (errorCode != Ok_Xml)
		{
			return errorCode;
		}
		xmlC.StrToDouble(valueXml, tempDoubleValue);
		runParams.hv_ScaleCMin = tempDoubleValue;

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_ScaleCMax", valueXml);
		if (errorCode != Ok_Xml)
		{
			return errorCode;
		}
		xmlC.StrToDouble(valueXml, tempDoubleValue);
		runParams.hv_ScaleCMax = tempDoubleValue;

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_Metric", valueXml);
		if (errorCode != Ok_Xml)
		{
			return errorCode;
		}
		runParams.hv_Metric = valueXml;

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_Contrast", valueXml);
		if (errorCode != Ok_Xml)
		{
			return errorCode;
		}
		xmlC.StrToInt(valueXml, tempIntValue);
		runParams.hv_Contrast = tempIntValue;

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_MinContrast", valueXml);
		if (errorCode != Ok_Xml)
		{
			return errorCode;
		}
		xmlC.StrToInt(valueXml, tempIntValue);
		runParams.hv_MinContrast = tempIntValue;

		//读取模板原始坐标和角度
		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_Angle_Original", valueXml);
		if (errorCode != Ok_Xml)
		{
			return errorCode;
		}
		xmlC.StrToDouble(valueXml, tempDoubleValue);
		runParams.hv_Angle_Original = tempDoubleValue;

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_CenterRow_Original", valueXml);
		if (errorCode != Ok_Xml)
		{
			return errorCode;
		}
		xmlC.StrToDouble(valueXml, tempDoubleValue);
		runParams.hv_CenterRow_Original = tempDoubleValue;

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_CenterCol_Original", valueXml);
		if (errorCode != Ok_Xml)
		{
			return errorCode;
		}
		xmlC.StrToDouble(valueXml, tempDoubleValue);
		runParams.hv_CenterCol_Original = tempDoubleValue;

		//读取运行参数
		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_MinScore", valueXml);
		if (errorCode != Ok_Xml)
		{
			return errorCode;
		}
		xmlC.StrToDouble(valueXml, tempDoubleValue);
		runParams.hv_MinScore = tempDoubleValue;

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_NumMatches", valueXml);
		if (errorCode != Ok_Xml)
		{
			return errorCode;
		}
		xmlC.StrToInt(valueXml, tempIntValue);
		runParams.hv_NumMatches = tempIntValue;

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_MaxOverlap", valueXml);
		if (errorCode != Ok_Xml)
		{
			return errorCode;
		}
		xmlC.StrToDouble(valueXml, tempDoubleValue);
		runParams.hv_MaxOverlap = tempDoubleValue;

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_SubPixel", valueXml);
		if (errorCode != Ok_Xml)
		{
			return errorCode;
		}
		runParams.hv_SubPixel = valueXml;

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_Greediness", valueXml);
		if (errorCode != Ok_Xml)
		{
			return errorCode;
		}
		xmlC.StrToDouble(valueXml, tempDoubleValue);
		runParams.hv_Greediness = tempDoubleValue;

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_TimeOut", valueXml);
		if (errorCode != Ok_Xml)
		{
			return errorCode;
		}
		xmlC.StrToInt(valueXml, tempIntValue);
		runParams.hv_TimeOut = tempIntValue;

		//读取运行区域
		int p1x, p1y, p2x, p2y;
		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "P1x_SearchRoi", valueXml);
		if (errorCode != Ok_Xml)
		{
			return errorCode;
		}
		xmlC.StrToInt(valueXml, tempIntValue);
		p1x = tempIntValue;

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "P1y_SearchRoi", valueXml);
		if (errorCode != Ok_Xml)
		{
			return errorCode;
		}
		xmlC.StrToInt(valueXml, tempIntValue);
		p1y = tempIntValue;

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "P2x_SearchRoi", valueXml);
		if (errorCode != Ok_Xml)
		{
			return errorCode;
		}
		xmlC.StrToInt(valueXml, tempIntValue);
		p2x = tempIntValue;

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "P2y_SearchRoi", valueXml);
		if (errorCode != Ok_Xml)
		{
			return errorCode;
		}
		xmlC.StrToInt(valueXml, tempIntValue);
		p2y = tempIntValue;

		runParams.P1_SearchRoi.x = p1x;
		runParams.P1_SearchRoi.y = p1y;
		runParams.P2_SearchRoi.x = p2x;
		runParams.P2_SearchRoi.y = p2y;

		//读取本地模板模型
		if (runParams.hv_MatchMethod == "ncc")
		{
			HTuple modelIDPath = HTuple(ConfigPath.c_str()) + HTuple(to_string(processId).c_str()) + "_" + HTuple(nodeName.c_str()) + HTuple(to_string(processModbuleID).c_str()) + "_ModelId.ncm";
			ReadNccModel(modelIDPath, &runParams.hv_ModelID);
		}
		else
		{
			HTuple modelIDPath = HTuple(ConfigPath.c_str()) + HTuple(to_string(processId).c_str()) + "_" + HTuple(nodeName.c_str()) + HTuple(to_string(processModbuleID).c_str()) + "_ModelId.shm";
			ReadShapeModel(modelIDPath, &runParams.hv_ModelID);
		}
		//mutex1.unlock();
		return Ok_Xml;

	}
	catch (...)
	{
		//mutex1.unlock();
		return nullUnKnow_Xml;
	}
}

std::string RobotCalibDetect::formatDobleValue(double val, int fixed)
{
	double val2 = val;
	if (val2 < 0) {
		val2 = -val2;
	}
	int intNum = 0;
	int temp = val2;
	while (temp) {
		temp /= 10;
		intNum++;
	}
	int intNum2 = 0;
	if (intNum == 0 && val2 > 0) {
		while ((int)val2 <= 0) {
			val2 *= 10;
			intNum2++;
		}
		if (intNum2 > 0)
			intNum2--;
	}

	std::ostringstream oss;
	oss << std::setprecision(fixed + intNum - intNum2) << val;
	return oss.str();
}
int RobotCalibDetect::getRefreshConfig(int processID, int modubleID, std::string modubleName)
{
	//XML读取参数
	ErrorCode_Xml errorCode = ReadParams_RobotCalib(configPath, XMLPath, mRunParams, processID, modubleName, modubleID);
	if (errorCode != Ok_Xml)
	{
		return 1;
	}
	return 0;
}
//返回一个不含strOld
string RobotCalibDetect::strReplaceAll(const string& strResource, const string& strOld, const string& strNew)
{
	string::size_type pos = 0;
	string strTemp = strResource;
	do
	{
		if ((pos = strTemp.find(strOld)) != string::npos)
		{
			strTemp.replace(pos, strOld.length(), strNew);
		}
	} while (pos != string::npos);
	return strTemp;
}
//路径设置
int RobotCalibDetect::PathSet(const string ConfigPath, const string XmlPath)
{
	try
	{
		XmlClass2 xmlC;//xml存取变量
		// 判定目录或文件是否存在的标识符
		int mode = 0;
    #ifdef _WIN32

        if (_access(ConfigPath.c_str(), mode))
        {
            //system("mkdir head");
            _mkdir(ConfigPath.c_str());
        }
    #else

        if (access(ConfigPath.c_str(), mode))
        {
            mkdir(ConfigPath.c_str(), 0777);
        }
    #endif
		//创建XML(存在就不会创建)
		int Ret = xmlC.CreateXML(XmlPath);
		if (Ret != 0)
		{
			return 1;
		}
		return 0;
	}
	catch (...)
	{
		return -1;
	}
}
//按照指定字符分割字符串到vector
void RobotCalibDetect::stringToken(const string sToBeToken, const string sSeperator, vector<string>& vToken)
{
	string sCopy = sToBeToken;
	int iPosEnd = 0;
	while (true)
	{
		iPosEnd = sCopy.find(sSeperator);
		if (iPosEnd == -1)
		{
			vToken.push_back(sCopy);
			break;
		}
		vToken.push_back(sCopy.substr(0, iPosEnd));
		sCopy = sCopy.substr(iPosEnd + 1);
	}
}
/// <summary>
/// 先拍后抓偏移量算法
/// </summary>
/// <param name="Model_X">模板X</param>
/// <param name="Model_Y">模板Y</param>
/// <param name="Model_A">模板A</param>
/// <param name="Now_X">当前X</param>
/// <param name="Now_Y">当前Y</param>
/// <param name="Now_A">当前A</param>
/// <param name="Center_X">旋转中心X</param>
/// <param name="Center_Y">旋转中心Y</param>
/// <param name="offset_x">偏移量X</param>
/// <param name="offset_y">偏移量Y</param>
/// <param name="offset_a">偏移量A</param>
void RobotCalibDetect::GetOffset_CamFirst2(bool isReverseAngle, double Model_X, double Model_Y, double Model_A, double Now_X, double Now_Y,
	double Now_A, double Center_X, double Center_Y, double &offset_x, double &offset_y, double &offset_a)
{
	try
	{
		double dAngle = Now_A - Model_A;
		if (isReverseAngle)
		{
			dAngle = 0 - dAngle;
		}

		double x = 0;
		double y = 0;
		x = (Model_X - Center_X)*cos(dAngle * PI / 180) - (Model_Y - Center_Y)*sin(dAngle * PI / 180) + Center_X;
		y = (Model_Y - Center_Y)*cos(dAngle * PI / 180) + (Model_X - Center_X)*sin(dAngle * PI / 180) + Center_Y;
		offset_x = Now_X - x;
		offset_y = Now_Y - y;
		offset_a = dAngle;
	}
	catch (...)
	{
		offset_a = -1;
		offset_x = -1;
		offset_y = -1;
	}

}
/// <summary>
/// 先抓后拍偏移量算法
/// </summary>
/// <param name="Model_X">模板X</param>
/// <param name="Model_Y">模板Y</param>
/// <param name="Model_A">模板A</param>
/// <param name="Now_X">当前X</param>
/// <param name="Now_Y">当前Y</param>
/// <param name="Now_A">当前A</param>
/// <param name="Center_X">旋转中心X</param>
/// <param name="Center_Y">旋转中心Y</param>
/// <param name="offset_x">偏移量X</param>
/// <param name="offset_y">偏移量Y</param>
/// <param name="offset_a">偏移量A</param>
void RobotCalibDetect::GetOffset_GrabFirst2(bool isReverseAngle, double Model_X, double Model_Y, double Model_A, double Now_X, double Now_Y,
	double Now_A, double Center_X, double Center_Y, double &offset_x, double &offset_y, double &offset_a)
{
	try
	{
		double dAngle = Model_A - Now_A;
		if (isReverseAngle)
		{
			dAngle = 0 - dAngle;
		}

		double x = 0;
		double y = 0;
		x = (Now_X - Center_X)*cos(dAngle * PI / 180) - (Now_Y - Center_Y)*sin(dAngle * PI / 180) + Center_X;
		y = (Now_Y - Center_Y)*cos(dAngle * PI / 180) + (Now_X - Center_X)*sin(dAngle * PI / 180) + Center_Y;
		offset_x = Model_X - x;
		offset_y = Model_Y - y;
		offset_a = dAngle;
	}
	catch (...)
	{
		offset_a = -1;
		offset_x = -1;
		offset_y = -1;
	}

}

