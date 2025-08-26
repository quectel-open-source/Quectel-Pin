#include "TemplateDetect.h"

TemplateDetect::TemplateDetect()
{
}

TemplateDetect::~TemplateDetect()
{
}

//����ģ��
int TemplateDetect::FindTemplate(const HObject &ho_Image, const RunParamsStruct_Template &RunParams, ResultParamsStruct_Template &ResultParams)
{
	//mutex1.lock();
	try
	{
		if (ho_Image.Key() == nullptr)
		{
			//ͼ��Ϊ�գ�����ͼ��clear
			ResultParams.hv_RetNum = 0;
			mResultParams = ResultParams;
			//mutex1.unlock();
			return 1;
		}
		if (RunParams.hv_ModelID == NULL)
		{
			//ģ����Ϊ��
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

		//��ԭʼ�Ľ�ģ����ͽǶȸ�ֵ������ṹ��
		ResultParams.hv_CenterRow_Original = RunParams.hv_CenterRow_Original;
		ResultParams.hv_CenterCol_Original = RunParams.hv_CenterCol_Original;
		ResultParams.hv_Angle_Original = RunParams.hv_Angle_Original;

		//����ʱ����ģ�����
		HTuple Homat2d_Roi;
		HTuple Homat2d_Window;
		HObject mho_Image;
		GenEmptyObj(&ResultParams.ho_Contour_Template);
		GenEmptyObj(&ResultParams.ho_Region_Template);

		if (RunParams.ho_SearchRegion.Key() == nullptr)
		{
			//��������Ϊ�գ����߱�clear(��������Ϊ�գ�Ĭ��ȫͼ����)
			mho_Image = ImgTemp;
		}
		else
		{
			ReduceDomain(ImgTemp, RunParams.ho_SearchRegion, &mho_Image);
		}

		//���ó�ʱʱ��
		if (RunParams.hv_MatchMethod.S() == "auto_ncc" || RunParams.hv_MatchMethod.S() == "ncc")
		{
			//���ò�ѯ��ʱʱ��1000ms
			SetNccModelParam(RunParams.hv_ModelID, "timeout", RunParams.hv_TimeOut);
		}
		else
		{
			//���ò�ѯ��ʱʱ��1000ms
			SetShapeModelParam(RunParams.hv_ModelID, "timeout", RunParams.hv_TimeOut);
			//����������λ��ͼ��֮��
			SetShapeModelParam(RunParams.hv_ModelID, "border_shape_models", "true");
		}
		if (RunParams.hv_MatchMethod.S() == "none")
		{
			//���������غ�������������ϵ��(������Ϊ��λ����Χ��0-32������),Ϊ�˻��������ķ�ֵ����������ƥ�䣬���ǽ���ʼ�ս�����������С���˵�������
			HTuple SubPixelTemp;
			if (RunParams.hv_SubPixel == "least_squares" || RunParams.hv_SubPixel == "least_squares_high" || RunParams.hv_SubPixel == "least_squares_very_high")
			{
				SubPixelTemp = (HTuple(RunParams.hv_SubPixel).Append("max_deformation 2"));
			}
			else
			{
				SubPixelTemp = RunParams.hv_SubPixel;
			}
			//��ȡģ�����
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
					//����ת�Ƕ�
					HTuple tempRad_Angle = ResultParams.hv_Angle[i];
					ResultParams.hv_Angle[i] = tempRad_Angle.TupleDeg();

					//����ROI����任����(����ץ�߸���)
					Homat2d_Roi.Clear();
					VectorAngleToRigid(RunParams.hv_CenterRow_Original, RunParams.hv_CenterCol_Original, 0, ResultParams.hv_Row[i], ResultParams.hv_Column[i], tempRad_Angle,
						&Homat2d_Roi);
					ResultParams.hv_ROIHomMat2D.Append(Homat2d_Roi);

					//����ROI����任����(������ʾģ������)��HalconĬ��ģ��ͼ����ʾ�����Ͻ�(0,0,0),���÷���任������ʾ��ͼ���м�
					Homat2d_Window.Clear();
					VectorAngleToRigid(0, 0, 0, ResultParams.hv_Row[i], ResultParams.hv_Column[i], tempRad_Angle,
						&Homat2d_Window);
					//��ȡģ������
					HObject ho_ContourRet;
					GetShapeModelContours(&ho_ContourRet, RunParams.hv_ModelID, 1);
					//����任
					AffineTransContourXld(ho_ContourRet, &ho_ContourRet, Homat2d_Window);
					ConcatObj(ResultParams.ho_Contour_Template, ho_ContourRet, &ResultParams.ho_Contour_Template);
					//��ȡģ������
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
			//���������غ�������������ϵ��(������Ϊ��λ����Χ��0-32������),Ϊ�˻��������ķ�ֵ����������ƥ�䣬���ǽ���ʼ�ս�����������С���˵�������
			HTuple SubPixelTemp;
			if (RunParams.hv_SubPixel == "least_squares" || RunParams.hv_SubPixel == "least_squares_high" || RunParams.hv_SubPixel == "least_squares_very_high")
			{
				SubPixelTemp = (HTuple(RunParams.hv_SubPixel).Append("max_deformation 2"));
			}
			else
			{
				SubPixelTemp = RunParams.hv_SubPixel;
			}
			//��ȡģ�����
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
					//����ת�Ƕ�
					HTuple tempRad_Angle = ResultParams.hv_Angle[i];
					ResultParams.hv_Angle[i] = tempRad_Angle.TupleDeg();

					//����ROI����任����(����ץ�߸���)
					Homat2d_Roi.Clear();
					VectorAngleToRigid(RunParams.hv_CenterRow_Original, RunParams.hv_CenterCol_Original, 0, ResultParams.hv_Row[i], ResultParams.hv_Column[i], tempRad_Angle,
						&Homat2d_Roi);
					ResultParams.hv_ROIHomMat2D.Append(Homat2d_Roi);

					//����ROI����任����(������ʾģ������)��HalconĬ��ģ��ͼ����ʾ�����Ͻ�(0,0,0),���÷���任������ʾ��ͼ���м�
					Homat2d_Window.Clear();
					VectorAngleToRigid(0, 0, 0, ResultParams.hv_Row[i], ResultParams.hv_Column[i], tempRad_Angle,
						&Homat2d_Window);
					//��ȡģ������
					HObject ho_ContourRet;
					GetShapeModelContours(&ho_ContourRet, RunParams.hv_ModelID, 1);
					//����任
					AffineTransContourXld(ho_ContourRet, &ho_ContourRet, Homat2d_Window);
					ConcatObj(ResultParams.ho_Contour_Template, ho_ContourRet, &ResultParams.ho_Contour_Template);
					//��ȡģ������
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
			//���������غ�������������ϵ��(������Ϊ��λ����Χ��0-32������),Ϊ�˻��������ķ�ֵ����������ƥ�䣬���ǽ���ʼ�ս�����������С���˵�������
			HTuple SubPixelTemp;
			if (RunParams.hv_SubPixel == "least_squares" || RunParams.hv_SubPixel == "least_squares_high" || RunParams.hv_SubPixel == "least_squares_very_high")
			{
				SubPixelTemp = (HTuple(RunParams.hv_SubPixel).Append("max_deformation 2"));
			}
			else
			{
				SubPixelTemp = RunParams.hv_SubPixel;
			}
			//��ȡģ�����
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
					//����ת�Ƕ�
					HTuple tempRad_Angle = ResultParams.hv_Angle[i];
					ResultParams.hv_Angle[i] = tempRad_Angle.TupleDeg();

					//����ROI����任����(����ץ�߸���)
					Homat2d_Roi.Clear();
					VectorAngleToRigid(RunParams.hv_CenterRow_Original, RunParams.hv_CenterCol_Original, 0, ResultParams.hv_Row[i], ResultParams.hv_Column[i], tempRad_Angle,
						&Homat2d_Roi);
					ResultParams.hv_ROIHomMat2D.Append(Homat2d_Roi);

					//����ROI����任����(������ʾģ������)��HalconĬ��ģ��ͼ����ʾ�����Ͻ�(0,0,0),���÷���任������ʾ��ͼ���м�
					Homat2d_Window.Clear();
					VectorAngleToRigid(0, 0, 0, ResultParams.hv_Row[i], ResultParams.hv_Column[i], tempRad_Angle,
						&Homat2d_Window);
					//��ȡģ������
					HObject ho_ContourRet;
					GetShapeModelContours(&ho_ContourRet, RunParams.hv_ModelID, 1);
					//����任
					AffineTransContourXld(ho_ContourRet, &ho_ContourRet, Homat2d_Window);
					ConcatObj(ResultParams.ho_Contour_Template, ho_ContourRet, &ResultParams.ho_Contour_Template);
					//��ȡģ������
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
					//����ת�Ƕ�
					HTuple tempRad_Angle = ResultParams.hv_Angle[i];
					ResultParams.hv_Angle[i] = tempRad_Angle.TupleDeg();

					//����ROI����任����(����ץ�߸���)
					Homat2d_Roi.Clear();
					VectorAngleToRigid(RunParams.hv_CenterRow_Original, RunParams.hv_CenterCol_Original, 0, ResultParams.hv_Row[i], ResultParams.hv_Column[i], tempRad_Angle,
						&Homat2d_Roi);
					ResultParams.hv_ROIHomMat2D.Append(Homat2d_Roi);

					//����ROI����任����(������ʾģ������)��HalconĬ��ģ��ͼ����ʾ�����Ͻ�(0,0,0),���÷���任������ʾ��ͼ���м�
					Homat2d_Window.Clear();
					VectorAngleToRigid(0, 0, 0, ResultParams.hv_Row[i], ResultParams.hv_Column[i], tempRad_Angle,
						&Homat2d_Window);
					//��ȡģ������
					HObject ho_RegionRet, ho_BoundaryRet, ho_ContourRet;
					GetNccModelRegion(&ho_RegionRet, RunParams.hv_ModelID); //��ȡģ������
					//����任
					Boundary(ho_RegionRet, &ho_BoundaryRet, "inner");
					GenContourRegionXld(ho_BoundaryRet, &ho_ContourRet, "border");
					AffineTransContourXld(ho_ContourRet, &ho_ContourRet, Homat2d_Window);
					ConcatObj(ResultParams.ho_Contour_Template, ho_ContourRet, &ResultParams.ho_Contour_Template);
					//��ȡģ������
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
			/*	qDebug() << "����ģ�壬û����ȷѡ��ģ������ ";*/
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
//����ģ��
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
//д��־����
int TemplateDetect::WriteTxt(string content)
{
	if (content.empty())
	{
		return -1;
	}
	const char *p = content.c_str();
	ofstream in;
	in.open("C:\\AriLog.txt", ios::app); //ios::trunc��ʾ�ڴ��ļ�ǰ���ļ����,������д��,�ļ��������򴴽�
	in << p << "\r";
	in.close();//�ر��ļ�
	return 0;
}

//��ȡϵͳʱ��
vector<string> TemplateDetect::get_current_time()
{
	/*�ܾ�ȷ������*/
	/*
	#include <windows.h>
	#include <iostream>
	*/
	SYSTEMTIME st = { 0 };
	GetLocalTime(&st);  //��ȡ��ǰʱ�� �ɾ�ȷ��ms

	char tmpbuff[16];
	/*��*/
	sprintf(tmpbuff, "%d", st.wYear);
	std::string year = tmpbuff;
	/*��*/
	sprintf(tmpbuff, "%d", st.wMonth);
	std::string month = tmpbuff;
	/*��*/
	sprintf(tmpbuff, "%d", st.wDay);
	std::string day = tmpbuff;
	/*ʱ*/
	sprintf(tmpbuff, "%d", st.wHour);
	std::string hour = tmpbuff;
	/*��*/
	sprintf(tmpbuff, "%d", st.wMinute);
	std::string minute = tmpbuff;
	/*��*/
	sprintf(tmpbuff, "%d", st.wSecond);
	std::string second = tmpbuff;
	/*����*/
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
	//XML��ȡ����
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
	modubleResultTitleList.push_back("�Ƕ�");
	modubleResultTitleList.push_back("����");

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

