#include "TemplateDetect.h"

TemplateDetect::TemplateDetect()
{
}

TemplateDetect::~TemplateDetect()
{
}

//查找模板
int TemplateDetect::FindTemplate(const HObject &ho_Image, const RunParamsStruct_Template &RunParams, ResultParamsStruct_Template &ResultParams)
{
	//mutex1.lock();
	try
	{
		if (ho_Image.Key() == nullptr)
		{
			//图像为空，或者图像被clear
			ResultParams.hv_RetNum = 0;
			mResultParams = ResultParams;
			//mutex1.unlock();
			return 1;
		}
		if (RunParams.hv_ModelID == NULL)
		{
			//模板句柄为空
			ResultParams.hv_RetNum = 0;
			mResultParams = ResultParams;
			//mutex1.unlock();
			return 1;
		}
		
		HObject ImgTemp;
		ImgTemp = ho_Image;
		HTuple countChannel;
		CountChannels(ho_Image, &countChannel);
		if (countChannel.I() == 3)
		{
			Rgb1ToGray(ho_Image, &ImgTemp);
		}

		//把原始的建模坐标和角度赋值到结果结构体
		ResultParams.hv_CenterRow_Original = RunParams.hv_CenterRow_Original;
		ResultParams.hv_CenterCol_Original = RunParams.hv_CenterCol_Original;
		ResultParams.hv_Angle_Original = RunParams.hv_Angle_Original;

		//下面时是找模板代码
		HTuple Homat2d_Roi;
		HTuple Homat2d_Window;
		HObject mho_Image;
		GenEmptyObj(&ResultParams.ho_Contour_Template);
		GenEmptyObj(&ResultParams.ho_Region_Template);

		if (RunParams.ho_SearchRegion.Key() == nullptr)
		{
			//搜索区域为空，或者被clear(搜索区域为空，默认全图搜索)
			mho_Image = ImgTemp;
		}
		else
		{
			ReduceDomain(ImgTemp, RunParams.ho_SearchRegion, &mho_Image);
		}

		//设置超时时间
		if (RunParams.hv_MatchMethod.S() == "auto_ncc" || RunParams.hv_MatchMethod.S() == "ncc")
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
		if (RunParams.hv_MatchMethod.S() == "none")
		{
			//设置亚像素和允许轮廓变形系数(以像素为单位，范围是0-32个像素),为了获得有意义的分值并避免错误的匹配，我们建议始终将变形量与最小二乘调整相结合
			HTuple SubPixelTemp;
			if (RunParams.hv_SubPixel == "least_squares" || RunParams.hv_SubPixel == "least_squares_high" || RunParams.hv_SubPixel == "least_squares_very_high")
			{
				SubPixelTemp = (HTuple(RunParams.hv_SubPixel).Append("max_deformation 2"));
			}
			else
			{
				SubPixelTemp = RunParams.hv_SubPixel;
			}
			//获取模板参数
			HTuple NumLevelsTemp, AngleStartTemp, AngleExtentTemp, AngleStepTemp, ScaleMinTemp, ScaleMaxTemp, ScaleStepTemp, MetricTemp, MinContrastTemp;
			GetShapeModelParams(RunParams.hv_ModelID, &NumLevelsTemp, &AngleStartTemp, &AngleExtentTemp, &AngleStepTemp, &ScaleMinTemp,
				&ScaleMaxTemp, &ScaleStepTemp, &MetricTemp, &MinContrastTemp);
			FindShapeModel(mho_Image, RunParams.hv_ModelID, RunParams.hv_AngleStart.TupleRad(), 2 * (RunParams.hv_AngleExtent.TupleRad()).TupleAbs(),
				RunParams.hv_MinScore, RunParams.hv_NumMatches.TupleInt(), RunParams.hv_MaxOverlap, SubPixelTemp,
				(HTuple(NumLevelsTemp).Append(1)), RunParams.hv_Greediness, &ResultParams.hv_Row, &ResultParams.hv_Column, &ResultParams.hv_Angle,
				&ResultParams.hv_Score);

			HTuple _length = 0;
			TupleLength(ResultParams.hv_Row, &_length);
			ResultParams.hv_RetNum = _length;
			if (_length == 0)
			{
				ResultParams.hv_RetNum = 0;
				mResultParams = ResultParams;
				//mutex1.unlock();
				return 1;
			}
			else
			{
				for (int i = 0; i < _length; i++)
				{
					//弧度转角度
					HTuple tempRad_Angle = ResultParams.hv_Angle[i];
					ResultParams.hv_Angle[i] = tempRad_Angle.TupleDeg();

					//生成ROI仿射变换矩阵(用于抓边跟随)
					Homat2d_Roi.Clear();
					VectorAngleToRigid(RunParams.hv_CenterRow_Original, RunParams.hv_CenterCol_Original, 0, ResultParams.hv_Row[i], ResultParams.hv_Column[i], tempRad_Angle,
						&Homat2d_Roi);
					ResultParams.hv_ROIHomMat2D.Append(Homat2d_Roi);

					//生成ROI仿射变换矩阵(用于显示模板轮廓)，Halcon默认模板图形显示在左上角(0,0,0),利用仿射变换把他显示到图像中间
					Homat2d_Window.Clear();
					VectorAngleToRigid(0, 0, 0, ResultParams.hv_Row[i], ResultParams.hv_Column[i], tempRad_Angle,
						&Homat2d_Window);
					//获取模板轮廓
					HObject ho_ContourRet;
					GetShapeModelContours(&ho_ContourRet, RunParams.hv_ModelID, 1);
					//仿射变换
					AffineTransContourXld(ho_ContourRet, &ho_ContourRet, Homat2d_Window);
					ConcatObj(ResultParams.ho_Contour_Template, ho_ContourRet, &ResultParams.ho_Contour_Template);
					//获取模板区域
					HTuple countXld;
					CountObj(ho_ContourRet, &countXld);
					HObject region1;
					GenEmptyObj(&region1);
					for (int i = 0; i < countXld; i++)
					{
						HObject xldSelect;
						SelectObj(ho_ContourRet, &xldSelect, i+1);
						//HObject region2;
						//GenRegionContourXld(xldSelect, &region2, "filled");
						//ConcatObj(region1, region2, &region1);
						HTuple rowXld, colXld;
						GetContourXld(xldSelect, &rowXld, &colXld);
						HObject region2;
						GenRegionPoints(&region2, rowXld, colXld);
						ConcatObj(region1, region2, &region1);
					}
					HObject regionUnion;
					Union1(region1, &regionUnion);
					ConcatObj(ResultParams.ho_Region_Template, regionUnion, &ResultParams.ho_Region_Template);
				}
				//mutex1.unlock();
				mResultParams = ResultParams;
				return 0;
			}
		}
		else if (RunParams.hv_MatchMethod.S() == "scaled")
		{
			//设置亚像素和允许轮廓变形系数(以像素为单位，范围是0-32个像素),为了获得有意义的分值并避免错误的匹配，我们建议始终将变形量与最小二乘调整相结合
			HTuple SubPixelTemp;
			if (RunParams.hv_SubPixel == "least_squares" || RunParams.hv_SubPixel == "least_squares_high" || RunParams.hv_SubPixel == "least_squares_very_high")
			{
				SubPixelTemp = (HTuple(RunParams.hv_SubPixel).Append("max_deformation 2"));
			}
			else
			{
				SubPixelTemp = RunParams.hv_SubPixel;
			}
			//获取模板参数
			HTuple NumLevelsTemp, AngleStartTemp, AngleExtentTemp, AngleStepTemp, ScaleMinTemp, ScaleMaxTemp, ScaleStepTemp, MetricTemp, MinContrastTemp;
			GetShapeModelParams(RunParams.hv_ModelID, &NumLevelsTemp, &AngleStartTemp, &AngleExtentTemp, &AngleStepTemp, &ScaleMinTemp,
				&ScaleMaxTemp, &ScaleStepTemp, &MetricTemp, &MinContrastTemp);
			FindScaledShapeModel(mho_Image, RunParams.hv_ModelID, RunParams.hv_AngleStart.TupleRad(), 2 * (RunParams.hv_AngleExtent.TupleRad()).TupleAbs(),
				RunParams.hv_ScaleRMin, RunParams.hv_ScaleRMax, RunParams.hv_MinScore, RunParams.hv_NumMatches.TupleInt(), RunParams.hv_MaxOverlap,
				SubPixelTemp, (HTuple(NumLevelsTemp).Append(1)), RunParams.hv_Greediness, &ResultParams.hv_Row, &ResultParams.hv_Column,
				&ResultParams.hv_Angle, &ResultParams.hv_ScaleR, &ResultParams.hv_Score);
			HTuple _length = 0;
			TupleLength(ResultParams.hv_Row, &_length);
			ResultParams.hv_RetNum = _length;
			if (_length == 0)
			{
				ResultParams.hv_RetNum = 0;
				mResultParams = ResultParams;
				//mutex1.unlock();
				return 1;
			}
			else
			{
				for (int i = 0; i < _length; i++)
				{
					//弧度转角度
					HTuple tempRad_Angle = ResultParams.hv_Angle[i];
					ResultParams.hv_Angle[i] = tempRad_Angle.TupleDeg();

					//生成ROI仿射变换矩阵(用于抓边跟随)
					Homat2d_Roi.Clear();
					VectorAngleToRigid(RunParams.hv_CenterRow_Original, RunParams.hv_CenterCol_Original, 0, ResultParams.hv_Row[i], ResultParams.hv_Column[i], tempRad_Angle,
						&Homat2d_Roi);
					ResultParams.hv_ROIHomMat2D.Append(Homat2d_Roi);

					//生成ROI仿射变换矩阵(用于显示模板轮廓)，Halcon默认模板图形显示在左上角(0,0,0),利用仿射变换把他显示到图像中间
					Homat2d_Window.Clear();
					VectorAngleToRigid(0, 0, 0, ResultParams.hv_Row[i], ResultParams.hv_Column[i], tempRad_Angle,
						&Homat2d_Window);
					//获取模板轮廓
					HObject ho_ContourRet;
					GetShapeModelContours(&ho_ContourRet, RunParams.hv_ModelID, 1);
					//仿射变换
					AffineTransContourXld(ho_ContourRet, &ho_ContourRet, Homat2d_Window);
					ConcatObj(ResultParams.ho_Contour_Template, ho_ContourRet, &ResultParams.ho_Contour_Template);
					//获取模板区域
					HTuple countXld;
					CountObj(ho_ContourRet, &countXld);
					HObject region1;
					GenEmptyObj(&region1);
					for (int i = 0; i < countXld; i++)
					{
						HObject xldSelect;
						SelectObj(ho_ContourRet, &xldSelect, i + 1);
						//HObject region2;
						//GenRegionContourXld(xldSelect, &region2, "filled");
						//ConcatObj(region1, region2, &region1);
						HTuple rowXld, colXld;
						GetContourXld(xldSelect, &rowXld, &colXld);
						HObject region2;
						GenRegionPoints(&region2, rowXld, colXld);
						ConcatObj(region1, region2, &region1);
					}
					HObject regionUnion;
					Union1(region1, &regionUnion);
					ConcatObj(ResultParams.ho_Region_Template, regionUnion, &ResultParams.ho_Region_Template);
				}
				//mutex1.unlock();
				mResultParams = ResultParams;
				return 0;
			}
		}
		else if (RunParams.hv_MatchMethod.S() == "aniso")
		{
			//设置亚像素和允许轮廓变形系数(以像素为单位，范围是0-32个像素),为了获得有意义的分值并避免错误的匹配，我们建议始终将变形量与最小二乘调整相结合
			HTuple SubPixelTemp;
			if (RunParams.hv_SubPixel == "least_squares" || RunParams.hv_SubPixel == "least_squares_high" || RunParams.hv_SubPixel == "least_squares_very_high")
			{
				SubPixelTemp = (HTuple(RunParams.hv_SubPixel).Append("max_deformation 2"));
			}
			else
			{
				SubPixelTemp = RunParams.hv_SubPixel;
			}
			//获取模板参数
			HTuple NumLevelsTemp, AngleStartTemp, AngleExtentTemp, AngleStepTemp, ScaleMinTemp, ScaleMaxTemp, ScaleStepTemp, MetricTemp, MinContrastTemp;
			GetShapeModelParams(RunParams.hv_ModelID, &NumLevelsTemp, &AngleStartTemp, &AngleExtentTemp, &AngleStepTemp, &ScaleMinTemp,
				&ScaleMaxTemp, &ScaleStepTemp, &MetricTemp, &MinContrastTemp);
			FindAnisoShapeModel(mho_Image, RunParams.hv_ModelID, RunParams.hv_AngleStart.TupleRad(), 2 * (RunParams.hv_AngleExtent.TupleRad()).TupleAbs(),
				RunParams.hv_ScaleRMin, RunParams.hv_ScaleRMax, RunParams.hv_ScaleCMin, RunParams.hv_ScaleCMax, RunParams.hv_MinScore,
				RunParams.hv_NumMatches.TupleInt(), RunParams.hv_MaxOverlap, SubPixelTemp, (HTuple(NumLevelsTemp).Append(1)), RunParams.hv_Greediness,
				&ResultParams.hv_Row, &ResultParams.hv_Column, &ResultParams.hv_Angle, &ResultParams.hv_ScaleR, &ResultParams.hv_ScaleC,
				&ResultParams.hv_Score);

			HTuple _length = 0;
			TupleLength(ResultParams.hv_Row, &_length);
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
				for (int i = 0; i < _length; i++)
				{
					//弧度转角度
					HTuple tempRad_Angle = ResultParams.hv_Angle[i];
					ResultParams.hv_Angle[i] = tempRad_Angle.TupleDeg();

					//生成ROI仿射变换矩阵(用于抓边跟随)
					Homat2d_Roi.Clear();
					VectorAngleToRigid(RunParams.hv_CenterRow_Original, RunParams.hv_CenterCol_Original, 0, ResultParams.hv_Row[i], ResultParams.hv_Column[i], tempRad_Angle,
						&Homat2d_Roi);
					ResultParams.hv_ROIHomMat2D.Append(Homat2d_Roi);

					//生成ROI仿射变换矩阵(用于显示模板轮廓)，Halcon默认模板图形显示在左上角(0,0,0),利用仿射变换把他显示到图像中间
					Homat2d_Window.Clear();
					VectorAngleToRigid(0, 0, 0, ResultParams.hv_Row[i], ResultParams.hv_Column[i], tempRad_Angle,
						&Homat2d_Window);
					//获取模板轮廓
					HObject ho_ContourRet;
					GetShapeModelContours(&ho_ContourRet, RunParams.hv_ModelID, 1);
					//仿射变换
					AffineTransContourXld(ho_ContourRet, &ho_ContourRet, Homat2d_Window);
					ConcatObj(ResultParams.ho_Contour_Template, ho_ContourRet, &ResultParams.ho_Contour_Template);
					//获取模板区域
					HTuple countXld;
					CountObj(ho_ContourRet, &countXld);
					HObject region1;
					GenEmptyObj(&region1);
					for (int i = 0; i < countXld; i++)
					{
						HObject xldSelect;
						SelectObj(ho_ContourRet, &xldSelect, i + 1);
						//HObject region2;
						//GenRegionContourXld(xldSelect, &region2, "filled");
						//ConcatObj(region1, region2, &region1);
						HTuple rowXld, colXld;
						GetContourXld(xldSelect, &rowXld, &colXld);
						HObject region2;
						GenRegionPoints(&region2, rowXld, colXld);
						ConcatObj(region1, region2, &region1);
					}
					HObject regionUnion;
					Union1(region1, &regionUnion);
					ConcatObj(ResultParams.ho_Region_Template, regionUnion, &ResultParams.ho_Region_Template);
				}
				//mutex1.unlock();
				mResultParams = ResultParams;
				return 0;
			}
		}
		else if (RunParams.hv_MatchMethod.S() == "ncc")
		{
			HTuple NumLevelsTemp, AngleStartTemp, AngleExtentTemp, AngleStepTemp, MetricTemp;
			GetNccModelParams(RunParams.hv_ModelID, &NumLevelsTemp, &AngleStartTemp, &AngleExtentTemp, &AngleStepTemp, &MetricTemp);
			FindNccModel(mho_Image, RunParams.hv_ModelID, RunParams.hv_AngleStart.TupleRad(), 2 * (RunParams.hv_AngleExtent.TupleRad()).TupleAbs(),
				RunParams.hv_MinScore, RunParams.hv_NumMatches.TupleInt(), RunParams.hv_MaxOverlap, "true", (HTuple(NumLevelsTemp).Append(1)),
				&ResultParams.hv_Row, &ResultParams.hv_Column, &ResultParams.hv_Angle, &ResultParams.hv_Score);

			HTuple _length = 0;
			TupleLength(ResultParams.hv_Row, &_length);
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
				for (int i = 0; i < _length; i++)
				{
					//弧度转角度
					HTuple tempRad_Angle = ResultParams.hv_Angle[i];
					ResultParams.hv_Angle[i] = tempRad_Angle.TupleDeg();

					//生成ROI仿射变换矩阵(用于抓边跟随)
					Homat2d_Roi.Clear();
					VectorAngleToRigid(RunParams.hv_CenterRow_Original, RunParams.hv_CenterCol_Original, 0, ResultParams.hv_Row[i], ResultParams.hv_Column[i], tempRad_Angle,
						&Homat2d_Roi);
					ResultParams.hv_ROIHomMat2D.Append(Homat2d_Roi);

					//生成ROI仿射变换矩阵(用于显示模板轮廓)，Halcon默认模板图形显示在左上角(0,0,0),利用仿射变换把他显示到图像中间
					Homat2d_Window.Clear();
					VectorAngleToRigid(0, 0, 0, ResultParams.hv_Row[i], ResultParams.hv_Column[i], tempRad_Angle,
						&Homat2d_Window);
					//获取模板轮廓
					HObject ho_RegionRet, ho_BoundaryRet, ho_ContourRet;
					GetNccModelRegion(&ho_RegionRet, RunParams.hv_ModelID); //获取模板轮廓
					//仿射变换
					Boundary(ho_RegionRet, &ho_BoundaryRet, "inner");
					GenContourRegionXld(ho_BoundaryRet, &ho_ContourRet, "border");
					AffineTransContourXld(ho_ContourRet, &ho_ContourRet, Homat2d_Window);
					ConcatObj(ResultParams.ho_Contour_Template, ho_ContourRet, &ResultParams.ho_Contour_Template);
					//获取模板区域
					HObject regionAffine;
					AffineTransRegion(ho_RegionRet, &regionAffine, Homat2d_Window, "nearest_neighbor"); 
					ConcatObj(ResultParams.ho_Region_Template, regionAffine, &ResultParams.ho_Region_Template);
				}
				//mutex1.unlock();
				mResultParams = ResultParams;
				return 0;
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
	}
	catch (...)
	{
		ResultParams.hv_RetNum = 0;
		mResultParams = ResultParams;
		//mutex1.unlock();
		return -1;
	}
}
//创建模板
int TemplateDetect::CreateTemplate(const HObject &ho_Image, RunParamsStruct_Template &RunParams)
{
	try
	{
		HObject ImgTemp;
		ImgTemp = ho_Image;
		HTuple countChannel;
		CountChannels(ho_Image, &countChannel);
		if (countChannel.I() == 3)
		{
			Rgb1ToGray(ho_Image, &ImgTemp);
		}
		if (RunParams.hv_MatchMethod.S() == "none")
		{
			HObject ho_TemplateImage;
			ReduceDomain(ImgTemp, RunParams.ho_TrainRegion, &ho_TemplateImage);
			CreateShapeModel(ho_TemplateImage, RunParams.hv_NumLevels, RunParams.hv_AngleStart.TupleRad(), 2 * (RunParams.hv_AngleExtent.TupleRad()).TupleAbs(),
				RunParams.hv_AngleStep, RunParams.hv_Optimization, RunParams.hv_Metric, RunParams.hv_Contrast, RunParams.hv_MinContrast,
				&RunParams.hv_ModelID);
			//CreateShapeModel(ho_TemplateImage, "auto", RunParams.hv_AngleStart.TupleRad(), 2 * (RunParams.hv_AngleExtent.TupleRad()).TupleAbs(),
			//	"auto", "auto", RunParams.hv_Metric, "auto", "auto", &RunParams.hv_ModelID);

			HTuple hv_Area;
			AreaCenter(RunParams.ho_TrainRegion, &hv_Area, &RunParams.hv_CenterRow_Original, &RunParams.hv_CenterCol_Original);

			return 0;
		}
		else if (RunParams.hv_MatchMethod.S() == "scaled")
		{
			HObject ho_TemplateImage;
			ReduceDomain(ImgTemp, RunParams.ho_TrainRegion, &ho_TemplateImage);
			CreateScaledShapeModel(ho_TemplateImage, RunParams.hv_NumLevels, RunParams.hv_AngleStart.TupleRad(), 2 * (RunParams.hv_AngleExtent.TupleRad()).TupleAbs(),
				RunParams.hv_AngleStep, RunParams.hv_ScaleRMin, RunParams.hv_ScaleRMax, RunParams.hv_ScaleRStep, RunParams.hv_Optimization,
				RunParams.hv_Metric, RunParams.hv_Contrast, RunParams.hv_MinContrast, &RunParams.hv_ModelID);
			//CreateScaledShapeModel(ho_TemplateImage, "auto", RunParams.hv_AngleStart.TupleRad(), 2 * (RunParams.hv_AngleExtent.TupleRad()).TupleAbs(),
			//	"auto", RunParams.hv_ScaleRMin, RunParams.hv_ScaleRMax, "auto", "auto", RunParams.hv_Metric, "auto", "auto",
			//	&RunParams.hv_ModelID);
			HTuple hv_Area;
			AreaCenter(RunParams.ho_TrainRegion, &hv_Area, &RunParams.hv_CenterRow_Original, &RunParams.hv_CenterCol_Original);

			return 0;
		}
		else if (RunParams.hv_MatchMethod.S() == "aniso")
		{
			HObject ho_TemplateImage;
			ReduceDomain(ImgTemp, RunParams.ho_TrainRegion, &ho_TemplateImage);
			CreateAnisoShapeModel(ho_TemplateImage, RunParams.hv_NumLevels, RunParams.hv_AngleStart.TupleRad(), 2 * (RunParams.hv_AngleExtent.TupleRad()).TupleAbs(),
				RunParams.hv_AngleStep, RunParams.hv_ScaleRMin, RunParams.hv_ScaleRMax, RunParams.hv_ScaleRStep, RunParams.hv_ScaleCMin,
				RunParams.hv_ScaleCMax, RunParams.hv_ScaleCStep, RunParams.hv_Optimization, RunParams.hv_Metric, RunParams.hv_Contrast, RunParams.hv_MinContrast,
				&RunParams.hv_ModelID);
			//CreateAnisoShapeModel(ho_TemplateImage, "auto", RunParams.hv_AngleStart.TupleRad(), 2 * (RunParams.hv_AngleExtent.TupleRad()).TupleAbs(),
			//	"auto", RunParams.hv_ScaleRMin, RunParams.hv_ScaleRMax, "auto", RunParams.hv_ScaleCMin, RunParams.hv_ScaleCMax,
			//	"auto", "auto", RunParams.hv_Metric, "auto", "auto", &RunParams.hv_ModelID);
			HTuple hv_Area;
			AreaCenter(RunParams.ho_TrainRegion, &hv_Area, &RunParams.hv_CenterRow_Original, &RunParams.hv_CenterCol_Original);

			return 0;
		}
		else if (RunParams.hv_MatchMethod.S() == "ncc")
		{
			HObject ho_TemplateImage;
			ReduceDomain(ImgTemp, RunParams.ho_TrainRegion, &ho_TemplateImage);
			CreateNccModel(ho_TemplateImage, RunParams.hv_NumLevels, RunParams.hv_AngleStart.TupleRad(), 2 * (RunParams.hv_AngleExtent.TupleRad()).TupleAbs(),
				RunParams.hv_AngleStep, "use_polarity", &RunParams.hv_ModelID);
			//CreateNccModel(ho_TemplateImage, "auto", RunParams.hv_AngleStart.TupleRad(), 2 * (RunParams.hv_AngleExtent.TupleRad()).TupleAbs(),
			//	"auto", "use_polarity", &RunParams.hv_ModelID);
			
			HTuple hv_Area;
			AreaCenter(RunParams.ho_TrainRegion, &hv_Area, &RunParams.hv_CenterRow_Original, &RunParams.hv_CenterCol_Original);

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
//写日志函数
int TemplateDetect::WriteTxt(string content)
{
	if (content.empty())
	{
		return -1;
	}
	const char *p = content.c_str();
	ofstream in;
	in.open("C:\\AriLog.txt", ios::app); //ios::trunc表示在打开文件前将文件清空,由于是写入,文件不存在则创建
	in << p << "\r";
	in.close();//关闭文件
	return 0;
}

//获取系统时间
vector<string> TemplateDetect::get_current_time()
{
	/*能精确到毫秒*/
	/*
	#include <windows.h>
	#include <iostream>
	*/
	SYSTEMTIME st = { 0 };
	GetLocalTime(&st);  //获取当前时间 可精确到ms

	char tmpbuff[16];
	/*年*/
	sprintf(tmpbuff, "%d", st.wYear);
	std::string year = tmpbuff;
	/*月*/
	sprintf(tmpbuff, "%d", st.wMonth);
	std::string month = tmpbuff;
	/*日*/
	sprintf(tmpbuff, "%d", st.wDay);
	std::string day = tmpbuff;
	/*时*/
	sprintf(tmpbuff, "%d", st.wHour);
	std::string hour = tmpbuff;
	/*分*/
	sprintf(tmpbuff, "%d", st.wMinute);
	std::string minute = tmpbuff;
	/*秒*/
	sprintf(tmpbuff, "%d", st.wSecond);
	std::string second = tmpbuff;
	/*毫秒*/
	sprintf(tmpbuff, "%d", st.wMilliseconds);
	std::string millisecond = tmpbuff;

	std::vector<string>current_time = {};

	current_time.push_back(year + month + day);
	current_time.push_back(hour + minute + second);
	current_time.push_back(year + month + day + hour + minute + second);
	current_time.push_back(year + "_" + month + "_" + day + "_" + hour + "_" + minute + "_" + second + "_" + millisecond);

	return current_time;
}

int TemplateDetect::getRefreshConfig(int processID, int modubleID, std::string modubleName)
{
	//XML读取参数
	ErrorCode_Xml errorCode = dataIOC.ReadParams_Template(configPath, XMLPath, mRunParams, processID, modubleName, modubleID);
	if (errorCode != Ok_Xml)
	{
		return 1;
	}
	return 0;
}
std::vector<std::string> TemplateDetect::getModubleResultTitleList()
{
	std::vector<std::string> result;
	modubleResultTitleList.clear();
	modubleResultTitleList.push_back("X");
	modubleResultTitleList.push_back("Y");
	modubleResultTitleList.push_back("角度");
	modubleResultTitleList.push_back("分数");

	result = modubleResultTitleList;

	return result;
}
std::vector<std::map<int, std::string>> TemplateDetect::getModubleResultList()
{
	std::vector<std::map<int, std::string>> results;
	int count = mResultParams.hv_RetNum.I();
	if (count > 0)
	{
		for (int i = 0; i < count; i++)
		{
			std::map<int, std::string> result;
			int index = 0;
			result[index++] = formatDobleValue(mResultParams.hv_Column[i].D(), 2);
			result[index++] = formatDobleValue(mResultParams.hv_Row[i].D(), 2);
			result[index++] = formatDobleValue(mResultParams.hv_Angle[i].D(), 2);
			result[index++] = formatDobleValue(mResultParams.hv_Score[i].D(), 2);

			results.push_back(result);
		}
	}
	return results;
}
std::string TemplateDetect::formatDobleValue(double val, int fixed)
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

