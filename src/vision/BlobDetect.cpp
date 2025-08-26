#include "BlobDetect.h"
BlobDetect::BlobDetect()
{}
BlobDetect::~BlobDetect()
{}
int BlobDetect::BlobFunc(const HObject &ho_Image, const RunParamsStruct_Blob &runParams_Blob, ResultParamsStruct_Blob &resultParams)
{
	try
	{
		//先清空结果
		GenEmptyObj(&resultParams.ho_DestImage);
		GenEmptyObj(&resultParams.ho_DestRegions);
		resultParams.hv_RetNum = 0;
		resultParams.hv_Area.Clear();
		resultParams.hv_Row.Clear();
		resultParams.hv_Column.Clear();
		resultParams.hv_Angle.Clear();
		resultParams.hv_Circularity.Clear();
		resultParams.hv_Compactness.Clear();
		resultParams.hv_Convexity.Clear();
		resultParams.hv_Rectangularity.Clear();
		resultParams.hv_Height.Clear();
		resultParams.hv_Width.Clear();
		resultParams.hv_OuterRadius.Clear();
		resultParams.hv_Rect2Len1.Clear();
		resultParams.hv_Rect2Len2.Clear();
		resultParams.hv_Rect2Angle.Clear();

		//检测代码
		HObject GrayImage;
		HTuple channels = 0;
		CountChannels(ho_Image, &channels);
		if (channels == 1)
		{
			GrayImage = ho_Image;
		}
		else
		{
			Rgb1ToGray(ho_Image, &GrayImage);
		}
		HObject ImageReduce;
		if (runParams_Blob.hv_IsUsSearchRoi == 1)
		{
			if (runParams_Blob.isFollow)
			{
				if (runParams_Blob.hv_HomMat2D == NULL)
				{
					resultParams.hv_RetNum = 0;
					mResultParams = resultParams;
					return 1;
				}
				else
				{
					HObject ho_SearchRegion;
					GenRectangle1(&ho_SearchRegion, runParams_Blob.P1_SearchRoi.y, runParams_Blob.P1_SearchRoi.x,
						runParams_Blob.P2_SearchRoi.y, runParams_Blob.P2_SearchRoi.x);
					HObject RegionTrans;
					AffineTransRegion(ho_SearchRegion, &RegionTrans, runParams_Blob.hv_HomMat2D, "nearest_neighbor");
					ReduceDomain(GrayImage, RegionTrans, &ImageReduce);
				}
			}
			else
			{
				HObject ho_SearchRegion;
				GenRectangle1(&ho_SearchRegion, runParams_Blob.P1_SearchRoi.y, runParams_Blob.P1_SearchRoi.x,
					runParams_Blob.P2_SearchRoi.y, runParams_Blob.P2_SearchRoi.x);
				ReduceDomain(GrayImage, ho_SearchRegion, &ImageReduce);
			}
		}
		else
		{
			ImageReduce = GrayImage;
		}

		//运行二值化
		HObject ho_Region;
		HTuple hv_DestArea, hv_DestRow, hv_DestColumn;
		TypeStruct_Binarization_Blob typeStruct;
		if (runParams_Blob.hv_ThresholdType == typeStruct.Auto_Threshold)
		{
			AutoThreshold(ImageReduce, &ho_Region, runParams_Blob.hv_Sigma);
			AreaCenter(ho_Region, &hv_DestArea, &hv_DestRow, &hv_DestColumn);
		}
		else if (runParams_Blob.hv_ThresholdType == typeStruct.Dyn_Threshold)
		{
			HObject ho_MeanImage;
			MeanImage(ImageReduce, &ho_MeanImage, runParams_Blob.hv_CoreWidth, runParams_Blob.hv_CoreHeight);
			DynThreshold(ImageReduce, ho_MeanImage, &ho_Region, runParams_Blob.hv_DynThresholdContrast.TupleInt(), runParams_Blob.hv_DynThresholdPolarity);
			AreaCenter(ho_Region, &hv_DestArea, &hv_DestRow, &hv_DestColumn);

		}
		else if (runParams_Blob.hv_ThresholdType == typeStruct.Fixed_Threshold)
		{
			if (runParams_Blob.hv_LowThreshold > runParams_Blob.hv_HighThreshold)
			{
				resultParams.hv_RetNum = 0;
				mResultParams = resultParams;
				return 1;
			}
			Threshold(ImageReduce, &ho_Region, runParams_Blob.hv_LowThreshold.TupleInt(), runParams_Blob.hv_HighThreshold.TupleInt());
			AreaCenter(ho_Region, &hv_DestArea, &hv_DestRow, &hv_DestColumn);
		}
		else
		{
			resultParams.hv_RetNum = 0;
			mResultParams = resultParams;
			return 1;
		}
		if (hv_DestArea <= 0)
		{
			resultParams.hv_RetNum = 0;
			mResultParams = resultParams;
			return 1;
		}
		//形态学
		TypeStruct_RegionOperator_Blob RegionOperator_Struct;
		int Num_Type = runParams_Blob.hv_RegionOperator_Type.TupleLength().I();
		int RectangleNum = 0;
		int CircleNum = 0;
		int FillUpShapeNum = 0;
		HObject ho_RegionDestTemp;
		ho_RegionDestTemp = ho_Region;
		if (Num_Type > 0)
		{
			for (int i = 0; i < Num_Type; i++)
			{
				if (runParams_Blob.hv_RegionOperator_Type[i] == RegionOperator_Struct.hv_BottomHat_Circle)
				{
					HObject ho_TempRegions, ho_StructRegion, ho_UnionRegion;
					HTuple hv_Count, hv_Area, hv_RowCenter, hv_ColumnCenter;

					Union1(ho_RegionDestTemp, &ho_UnionRegion);
					AreaCenter(ho_UnionRegion, &hv_Area, &hv_RowCenter, &hv_ColumnCenter);
					GenCircle(&ho_StructRegion, hv_RowCenter, hv_ColumnCenter, runParams_Blob.hv_CircleRadius_Operator[CircleNum]);
					BottomHat(ho_RegionDestTemp, ho_StructRegion, &ho_TempRegions);
					HTuple hv_DestArea, hv_DestRow, hv_DestColumn;
					AreaCenter(ho_TempRegions, &hv_DestArea, &hv_DestRow, &hv_DestColumn);

					if (hv_DestArea != 0)
					{
						ho_RegionDestTemp = ho_TempRegions;
					}
					CircleNum += 1;
				}
				else if (runParams_Blob.hv_RegionOperator_Type[i] == RegionOperator_Struct.hv_BottomHat_Rectangle)
				{
					HObject ho_TempRegions, ho_StructRegion, ho_UnionRegion;
					HTuple hv_Count, hv_Area, hv_RowCenter, hv_ColumnCenter;
					Union1(ho_RegionDestTemp, &ho_UnionRegion);
					AreaCenter(ho_UnionRegion, &hv_Area, &hv_RowCenter, &hv_ColumnCenter);
					GenRectangle2(&ho_StructRegion, hv_RowCenter, hv_ColumnCenter, 0, runParams_Blob.hv_Width_Operator[RectangleNum] / 2, runParams_Blob.hv_Height_Operator[RectangleNum] / 2);
					BottomHat(ho_RegionDestTemp, ho_StructRegion, &ho_TempRegions);
					HTuple hv_DestArea, hv_DestRow, hv_DestColumn;
					AreaCenter(ho_TempRegions, &hv_DestArea, &hv_DestRow, &hv_DestColumn);

					if (hv_DestArea != 0)
					{
						ho_RegionDestTemp = ho_TempRegions;
					}
					RectangleNum += 1;
				}
				else if (runParams_Blob.hv_RegionOperator_Type[i] == RegionOperator_Struct.hv_TopHat_Circle)
				{
					HObject ho_TempRegions, ho_StructRegion, ho_UnionRegion;
					HTuple hv_Count, hv_Area, hv_RowCenter, hv_ColumnCenter;
					Union1(ho_RegionDestTemp, &ho_UnionRegion);
					AreaCenter(ho_UnionRegion, &hv_Area, &hv_RowCenter, &hv_ColumnCenter);
					GenCircle(&ho_StructRegion, hv_RowCenter, hv_ColumnCenter, runParams_Blob.hv_CircleRadius_Operator[CircleNum]);
					TopHat(ho_RegionDestTemp, ho_StructRegion, &ho_TempRegions);
					HTuple hv_DestArea, hv_DestRow, hv_DestColumn;
					AreaCenter(ho_TempRegions, &hv_DestArea, &hv_DestRow, &hv_DestColumn);

					if (hv_DestArea != 0)
					{
						ho_RegionDestTemp = ho_TempRegions;
					}
					CircleNum += 1;
				}
				else if (runParams_Blob.hv_RegionOperator_Type[i] == RegionOperator_Struct.hv_TopHat_Rectangle)
				{
					HObject ho_TempRegions, ho_StructRegion, ho_UnionRegion;
					HTuple hv_Count, hv_Area, hv_RowCenter, hv_ColumnCenter;
					Union1(ho_RegionDestTemp, &ho_UnionRegion);
					AreaCenter(ho_UnionRegion, &hv_Area, &hv_RowCenter, &hv_ColumnCenter);
					GenRectangle2(&ho_StructRegion, hv_RowCenter, hv_ColumnCenter, 0, runParams_Blob.hv_Width_Operator[RectangleNum] / 2, runParams_Blob.hv_Height_Operator[RectangleNum] / 2);
					TopHat(ho_RegionDestTemp, ho_StructRegion, &ho_TempRegions);
					HTuple hv_DestArea, hv_DestRow, hv_DestColumn;
					AreaCenter(ho_TempRegions, &hv_DestArea, &hv_DestRow, &hv_DestColumn);

					if (hv_DestArea != 0)
					{
						ho_RegionDestTemp = ho_TempRegions;
					}
					RectangleNum += 1;
				}
				else if (runParams_Blob.hv_RegionOperator_Type[i] == RegionOperator_Struct.hv_Dilation_Circle)
				{
					HObject ho_TempRegions;
					DilationCircle(ho_RegionDestTemp, &ho_TempRegions, runParams_Blob.hv_CircleRadius_Operator[CircleNum]);
					HTuple hv_DestArea, hv_DestRow, hv_DestColumn;
					AreaCenter(ho_TempRegions, &hv_DestArea, &hv_DestRow, &hv_DestColumn);

					if (hv_DestArea != 0)
					{
						ho_RegionDestTemp = ho_TempRegions;
					}
					CircleNum += 1;
				}
				else if (runParams_Blob.hv_RegionOperator_Type[i] == RegionOperator_Struct.hv_Dilation_Rectangle)
				{
					HObject ho_TempRegions;
					DilationRectangle1(ho_RegionDestTemp, &ho_TempRegions, runParams_Blob.hv_Width_Operator[RectangleNum], runParams_Blob.hv_Height_Operator[RectangleNum]);
					HTuple hv_DestArea, hv_DestRow, hv_DestColumn;
					AreaCenter(ho_TempRegions, &hv_DestArea, &hv_DestRow, &hv_DestColumn);

					if (hv_DestArea != 0)
					{
						ho_RegionDestTemp = ho_TempRegions;
					}
					RectangleNum += 1;
				}
				else if (runParams_Blob.hv_RegionOperator_Type[i] == RegionOperator_Struct.hv_Erosion_Circle)
				{
					HObject ho_TempRegions;
					ErosionCircle(ho_RegionDestTemp, &ho_TempRegions, runParams_Blob.hv_CircleRadius_Operator[CircleNum]);
					HTuple hv_DestArea, hv_DestRow, hv_DestColumn;
					AreaCenter(ho_TempRegions, &hv_DestArea, &hv_DestRow, &hv_DestColumn);

					if (hv_DestArea != 0)
					{
						ho_RegionDestTemp = ho_TempRegions;
					}
					CircleNum += 1;
				}
				else if (runParams_Blob.hv_RegionOperator_Type[i] == RegionOperator_Struct.hv_Erosion_Rectangle)
				{
					HObject ho_TempRegions;
					ErosionRectangle1(ho_RegionDestTemp, &ho_TempRegions, runParams_Blob.hv_Width_Operator[RectangleNum], runParams_Blob.hv_Height_Operator[RectangleNum]);
					HTuple hv_DestArea, hv_DestRow, hv_DestColumn;
					AreaCenter(ho_TempRegions, &hv_DestArea, &hv_DestRow, &hv_DestColumn);

					if (hv_DestArea != 0)
					{
						ho_RegionDestTemp = ho_TempRegions;
					}
					RectangleNum += 1;
				}
				else if (runParams_Blob.hv_RegionOperator_Type[i] == RegionOperator_Struct.hv_Opening_Circle)
				{
					HObject ho_TempRegions;
					OpeningCircle(ho_RegionDestTemp, &ho_TempRegions, runParams_Blob.hv_CircleRadius_Operator[CircleNum]);
					HTuple hv_DestArea, hv_DestRow, hv_DestColumn;
					AreaCenter(ho_TempRegions, &hv_DestArea, &hv_DestRow, &hv_DestColumn);

					if (hv_DestArea != 0)
					{
						ho_RegionDestTemp = ho_TempRegions;
					}
					CircleNum += 1;
				}
				else if (runParams_Blob.hv_RegionOperator_Type[i] == RegionOperator_Struct.hv_Opening_Rectangle)
				{
					HObject ho_TempRegions;
					OpeningRectangle1(ho_RegionDestTemp, &ho_TempRegions, runParams_Blob.hv_Width_Operator[RectangleNum], runParams_Blob.hv_Height_Operator[RectangleNum]);
					HTuple hv_DestArea, hv_DestRow, hv_DestColumn;
					AreaCenter(ho_TempRegions, &hv_DestArea, &hv_DestRow, &hv_DestColumn);

					if (hv_DestArea != 0)
					{
						ho_RegionDestTemp = ho_TempRegions;
					}
					RectangleNum += 1;
				}
				else if (runParams_Blob.hv_RegionOperator_Type[i] == RegionOperator_Struct.hv_Closing_Circle)
				{
					HObject ho_TempRegions;
					ClosingCircle(ho_RegionDestTemp, &ho_TempRegions, runParams_Blob.hv_CircleRadius_Operator[CircleNum]);
					HTuple hv_DestArea, hv_DestRow, hv_DestColumn;
					AreaCenter(ho_TempRegions, &hv_DestArea, &hv_DestRow, &hv_DestColumn);

					if (hv_DestArea != 0)
					{
						ho_RegionDestTemp = ho_TempRegions;
					}
					CircleNum += 1;
				}
				else if (runParams_Blob.hv_RegionOperator_Type[i] == RegionOperator_Struct.hv_Closing_Rectangle)
				{
					HObject ho_TempRegions;
					ClosingRectangle1(ho_RegionDestTemp, &ho_TempRegions, runParams_Blob.hv_Width_Operator[RectangleNum], runParams_Blob.hv_Height_Operator[RectangleNum]);
					HTuple hv_DestArea, hv_DestRow, hv_DestColumn;
					AreaCenter(ho_TempRegions, &hv_DestArea, &hv_DestRow, &hv_DestColumn);

					if (hv_DestArea != 0)
					{
						ho_RegionDestTemp = ho_TempRegions;
					}
					RectangleNum += 1;
				}
				else if (runParams_Blob.hv_RegionOperator_Type[i] == RegionOperator_Struct.hv_Fillup)
				{
					HObject ho_TempRegions, ho_StructRegion, ho_UnionRegion;
					FillUp(ho_RegionDestTemp, &ho_TempRegions);
					HTuple hv_DestArea, hv_DestRow, hv_DestColumn;
					AreaCenter(ho_TempRegions, &hv_DestArea, &hv_DestRow, &hv_DestColumn);

					if (hv_DestArea != 0)
					{
						ho_RegionDestTemp = ho_TempRegions;
					}
				}
				else if (runParams_Blob.hv_RegionOperator_Type[i] == RegionOperator_Struct.hv_FillUp_Shape)
				{
					HObject ho_TempRegions, ho_StructRegion, ho_UnionRegion;
					FillUpShape(ho_RegionDestTemp, &ho_TempRegions, "area", runParams_Blob.hv_MinArea_FillUpShape_Operator[FillUpShapeNum], runParams_Blob.hv_MaxArea_FillUpShape_Operator[FillUpShapeNum]);
					HTuple hv_DestArea, hv_DestRow, hv_DestColumn;
					AreaCenter(ho_TempRegions, &hv_DestArea, &hv_DestRow, &hv_DestColumn);

					if (hv_DestArea != 0)
					{
						ho_RegionDestTemp = ho_TempRegions;
					}
					FillUpShapeNum += 1;
				}
				else if (runParams_Blob.hv_RegionOperator_Type[i] == RegionOperator_Struct.hv_Connection)
				{
					HObject ho_ConnectionRegions;
					HTuple hv_Count;
					Connection(ho_RegionDestTemp, &ho_ConnectionRegions);
					CountObj(ho_ConnectionRegions, &hv_Count);

					if (hv_Count.TupleInt().I() != 0)
					{
						ho_RegionDestTemp = ho_ConnectionRegions;
					}
				}
				else if (runParams_Blob.hv_RegionOperator_Type[i] == RegionOperator_Struct.hv_Union1)
				{
					HObject ho_UnionRegion;
					Union1(ho_RegionDestTemp, &ho_UnionRegion);
					HTuple hv_DestArea, hv_DestRow, hv_DestColumn;
					AreaCenter(ho_UnionRegion, &hv_DestArea, &hv_DestRow, &hv_DestColumn);

					if (hv_DestArea != 0)
					{
						ho_RegionDestTemp = ho_UnionRegion;
					}
				}
				else if (runParams_Blob.hv_RegionOperator_Type[i] == RegionOperator_Struct.hv_Complement)
				{
					HObject ho_ComplementRegion;
					Complement(ho_RegionDestTemp, &ho_ComplementRegion);
					HTuple hv_DestArea, hv_DestRow, hv_DestColumn;
					AreaCenter(ho_ComplementRegion, &hv_DestArea, &hv_DestRow, &hv_DestColumn);

					if (hv_DestArea != 0)
					{
						ho_RegionDestTemp = ho_ComplementRegion;
					}
				}
			}
			//计算输出
			HTuple hv_DestArea2, hv_DestRow2, hv_DestColumn2;
			AreaCenter(ho_RegionDestTemp, &hv_DestArea2, &hv_DestRow2, &hv_DestColumn2);
			if (hv_DestArea2 == 0)
			{
				resultParams.hv_RetNum = 0;
				mResultParams = resultParams;
				return 1;
			}
		}
		//区域筛选操作
		TypeStruct_FeaturesFilter_Blob FeaturesFilter_Struct;
		if (runParams_Blob.hv_FeaturesFilter_Type.TupleLength() > 0)
		{
			HObject ho_SelectRegions;
			HTuple hv_Count;

			//把运行参数赋值出来
			RunParamsStruct_Blob runParamsTemp = runParams_Blob;
			//角度转弧度
			HTuple index1;
			//查询筛选结构体里面是否存在角度特征
			TupleFind(runParamsTemp.hv_FeaturesFilter_Type, FeaturesFilter_Struct.hv_Angle_Filter, &index1);
			if (index1 != -1)
			{
				//存在角度特征，开始转换
				HTuple tempRad_Angle = runParamsTemp.hv_MinValue_Filter[index1];
				runParamsTemp.hv_MinValue_Filter[index1] = tempRad_Angle.TupleRad();
				tempRad_Angle = runParamsTemp.hv_MaxValue_Filter[index1];
				runParamsTemp.hv_MaxValue_Filter[index1] = tempRad_Angle.TupleRad();
			}
			index1 = -1;
			//查询筛选结构体里面是否存在旋转矩形角度特征
			TupleFind(runParamsTemp.hv_FeaturesFilter_Type, FeaturesFilter_Struct.hv_Rect2Angle_Filter, &index1);
			if (index1 != -1)
			{
				//存在旋转矩形角度特征，开始转换
				HTuple tempRad_Angle = runParamsTemp.hv_MinValue_Filter[index1];
				runParamsTemp.hv_MinValue_Filter[index1] = tempRad_Angle.TupleRad();
				tempRad_Angle = runParamsTemp.hv_MaxValue_Filter[index1];
				runParamsTemp.hv_MaxValue_Filter[index1] = tempRad_Angle.TupleRad();
			}

			Connection(ho_RegionDestTemp, &ho_RegionDestTemp);
			//中文转英文
			TypeStruct_FeaturesFilter_Blob mType;
			for (int i = 0; i < runParamsTemp.hv_FeaturesFilter_Type.TupleLength(); i++)
			{
				if (runParamsTemp.hv_FeaturesFilter_Type[i] == mType.hv_Area_Filter)
				{
					runParamsTemp.hv_FeaturesFilter_Type[i] = "area";
				}
				else if (runParamsTemp.hv_FeaturesFilter_Type[i] == mType.hv_Row_Filter)
				{
					runParamsTemp.hv_FeaturesFilter_Type[i] = "row";
				}
				else if (runParamsTemp.hv_FeaturesFilter_Type[i] == mType.hv_Column_Filter)
				{
					runParamsTemp.hv_FeaturesFilter_Type[i] = "column";
				}
				else if (runParamsTemp.hv_FeaturesFilter_Type[i] == mType.hv_Angle_Filter)
				{
					runParamsTemp.hv_FeaturesFilter_Type[i] = "orientation";
				}
				else if (runParamsTemp.hv_FeaturesFilter_Type[i] == mType.hv_Circularity_Filter)
				{
					runParamsTemp.hv_FeaturesFilter_Type[i] = "circularity";
				}
				else if (runParamsTemp.hv_FeaturesFilter_Type[i] == mType.hv_Compactness_Filter)
				{
					runParamsTemp.hv_FeaturesFilter_Type[i] = "compactness";
				}
				else if (runParamsTemp.hv_FeaturesFilter_Type[i] == mType.hv_Convexity_Filter)
				{
					runParamsTemp.hv_FeaturesFilter_Type[i] = "convexity";
				}
				else if (runParamsTemp.hv_FeaturesFilter_Type[i] == mType.hv_Rectangularity_Filter)
				{
					runParamsTemp.hv_FeaturesFilter_Type[i] = "rectangularity";
				}
				else if (runParamsTemp.hv_FeaturesFilter_Type[i] == mType.hv_Height_Filter)
				{
					runParamsTemp.hv_FeaturesFilter_Type[i] = "height";
				}
				else if (runParamsTemp.hv_FeaturesFilter_Type[i] == mType.hv_Width_Filter)
				{
					runParamsTemp.hv_FeaturesFilter_Type[i] = "width";
				}
				else if (runParamsTemp.hv_FeaturesFilter_Type[i] == mType.hv_OuterRadius_Filter)
				{
					runParamsTemp.hv_FeaturesFilter_Type[i] = "outer_radius";
				}
				else if (runParamsTemp.hv_FeaturesFilter_Type[i] == mType.hv_Rect2Len1_Filter)
				{
					runParamsTemp.hv_FeaturesFilter_Type[i] = "rect2_len1";
				}
				else if (runParamsTemp.hv_FeaturesFilter_Type[i] == mType.hv_Rect2Len2_Filter)
				{
					runParamsTemp.hv_FeaturesFilter_Type[i] = "rect2_len2";
				}
				else if (runParamsTemp.hv_FeaturesFilter_Type[i] == mType.hv_Rect2Angle_Filter)
				{
					runParamsTemp.hv_FeaturesFilter_Type[i] = "rect2_phi";
				}
			}
			SelectShape(ho_RegionDestTemp, &ho_SelectRegions, runParamsTemp.hv_FeaturesFilter_Type, runParamsTemp.hv_And_Or_Filter, runParamsTemp.hv_MinValue_Filter, runParamsTemp.hv_MaxValue_Filter);
			HTuple hv_AreaTemp, hv_RowTemp, hv_ColumnTemp;
			AreaCenter(ho_SelectRegions, &hv_AreaTemp, &hv_RowTemp, &hv_ColumnTemp);
			if (hv_AreaTemp == 0)
			{
				resultParams.hv_RetNum = 0;
				mResultParams = resultParams;
				return 1;
			}
			GenEmptyObj(&ho_RegionDestTemp);
			ho_RegionDestTemp.Clear();
			ho_RegionDestTemp = ho_SelectRegions;
		}
		//计算区域特征
		resultParams.ho_DestRegions = ho_RegionDestTemp;
		CountObj(resultParams.ho_DestRegions, &resultParams.hv_RetNum);
		RegionFeatures(resultParams.ho_DestRegions, "area", &resultParams.hv_Area);
		RegionFeatures(resultParams.ho_DestRegions, "row", &resultParams.hv_Row);
		RegionFeatures(resultParams.ho_DestRegions, "column", &resultParams.hv_Column);
		RegionFeatures(resultParams.ho_DestRegions, "orientation", &resultParams.hv_Angle);
		//弧度转角度输出
		resultParams.hv_Angle = resultParams.hv_Angle.TupleDeg();

		RegionFeatures(resultParams.ho_DestRegions, "circularity", &resultParams.hv_Circularity);
		RegionFeatures(resultParams.ho_DestRegions, "compactness", &resultParams.hv_Compactness);
		RegionFeatures(resultParams.ho_DestRegions, "convexity", &resultParams.hv_Convexity);
		RegionFeatures(resultParams.ho_DestRegions, "rectangularity", &resultParams.hv_Rectangularity);
		RegionFeatures(resultParams.ho_DestRegions, "height", &resultParams.hv_Height);
		RegionFeatures(resultParams.ho_DestRegions, "width", &resultParams.hv_Width);
		RegionFeatures(resultParams.ho_DestRegions, "outer_radius", &resultParams.hv_OuterRadius);
		RegionFeatures(resultParams.ho_DestRegions, "rect2_len1", &resultParams.hv_Rect2Len1);
		RegionFeatures(resultParams.ho_DestRegions, "rect2_len2", &resultParams.hv_Rect2Len2);
		RegionFeatures(resultParams.ho_DestRegions, "rect2_phi", &resultParams.hv_Rect2Angle);
		//弧度转角度输出
		resultParams.hv_Rect2Angle = resultParams.hv_Rect2Angle.TupleDeg();

		mResultParams = resultParams;
		if (resultParams.hv_RetNum == runParams_Blob.hv_SetBlobCount)
		{
			return 0;
		}
		return 1;
	}
	catch (...)
	{
		//先清空结果
		GenEmptyObj(&resultParams.ho_DestImage);
		GenEmptyObj(&resultParams.ho_DestRegions);
		resultParams.hv_RetNum = 0;
		resultParams.hv_Area.Clear();
		resultParams.hv_Row.Clear();
		resultParams.hv_Column.Clear();
		resultParams.hv_Angle.Clear();
		resultParams.hv_Circularity.Clear();
		resultParams.hv_Compactness.Clear();
		resultParams.hv_Convexity.Clear();
		resultParams.hv_Rectangularity.Clear();
		resultParams.hv_Height.Clear();
		resultParams.hv_Width.Clear();
		resultParams.hv_OuterRadius.Clear();
		resultParams.hv_Rect2Len1.Clear();
		resultParams.hv_Rect2Len2.Clear();
		resultParams.hv_Rect2Angle.Clear();

		mResultParams = resultParams;
		return -1;
	}
}

//获取系统时间
vector<string> BlobDetect::get_current_time()
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

int BlobDetect::getRefreshConfig(int processID, int modubleID, std::string modubleName)
{
	//XML读取参数
	ErrorCode_Xml errorCode = dataIOC.ReadParams_Blob(configPath, XMLPath, mRunParams, processID, modubleName, modubleID);
	if (errorCode != Ok_Xml)
	{
		return 1;
	}
	return 0;
}
std::vector<std::string> BlobDetect::getModubleResultTitleList()
{
	std::vector<std::string> result;
	modubleResultTitleList.clear();
	modubleResultTitleList.push_back("面积");
	modubleResultTitleList.push_back("X");
	modubleResultTitleList.push_back("Y");
	modubleResultTitleList.push_back("角度");
	modubleResultTitleList.push_back("圆度");
	modubleResultTitleList.push_back("紧密度");
	modubleResultTitleList.push_back("凸度");
	modubleResultTitleList.push_back("矩形度");
	modubleResultTitleList.push_back("高度");
	modubleResultTitleList.push_back("宽度");
	modubleResultTitleList.push_back("最小外接圆半径");
	modubleResultTitleList.push_back("最小外接矩半长");
	modubleResultTitleList.push_back("最小外接矩半宽");
	modubleResultTitleList.push_back("最小外接矩角度");

	result = modubleResultTitleList;

	return result;
}
std::vector<std::map<int, std::string>> BlobDetect::getModubleResultList()
{
	std::vector<std::map<int, std::string>> results;
	int count = mResultParams.hv_RetNum.I();
	if (count > 0)
	{
		for (int i = 0; i < count; i++)
		{
			std::map<int, std::string> result;
			int index = 0;
			result[index++] = formatDobleValue(HTuple(mResultParams.hv_Area[i]).TupleReal().D(), 0);
			result[index++] = formatDobleValue(HTuple(mResultParams.hv_Column[i]).TupleReal().D(), 2);
			result[index++] = formatDobleValue(HTuple(mResultParams.hv_Row[i]).TupleReal().D(), 2);
			result[index++] = formatDobleValue(HTuple(mResultParams.hv_Angle[i]).TupleReal().D(), 2);
			result[index++] = formatDobleValue(HTuple(mResultParams.hv_Circularity[i]).TupleReal().D(), 2);
			result[index++] = formatDobleValue(HTuple(mResultParams.hv_Compactness[i]).TupleReal().D(), 2);
			result[index++] = formatDobleValue(HTuple(mResultParams.hv_Convexity[i]).TupleReal().D(), 2);
			result[index++] = formatDobleValue(HTuple(mResultParams.hv_Rectangularity[i]).TupleReal().D(), 2);
			result[index++] = formatDobleValue(HTuple(mResultParams.hv_Height[i]).TupleReal().D(), 2);
			result[index++] = formatDobleValue(HTuple(mResultParams.hv_Width[i]).TupleReal().D(), 2);
			result[index++] = formatDobleValue(HTuple(mResultParams.hv_OuterRadius[i]).TupleReal().D(), 2);
			result[index++] = formatDobleValue(HTuple(mResultParams.hv_Rect2Len1[i]).TupleReal().D(), 2);
			result[index++] = formatDobleValue(HTuple(mResultParams.hv_Rect2Len2[i]).TupleReal().D(), 2);
			result[index++] = formatDobleValue(HTuple(mResultParams.hv_Rect2Angle[i]).TupleReal().D(), 2);

			results.push_back(result);
		}
	}
	return results;
}
std::string BlobDetect::formatDobleValue(double val, int fixed)
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