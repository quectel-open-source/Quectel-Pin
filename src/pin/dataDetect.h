#pragma once
#pragma execution_character_set("utf-8")

#include <iostream>
#include <vector>
#include <stdio.h>
#include <iomanip>
#include <sstream>
#include <fstream>
#include <Halcon.h>
#include <halconCpp/HalconCpp.h>
#include <thread>
#include <string>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/opencv.hpp>

using namespace std;
using namespace HalconCpp;
using namespace cv;

//����任���ͽṹ��
struct TypeStruct_AffineMatrix
{
	HTuple Create_HomMat2D_Translate = "����ƽ�ƾ���";
	HTuple CreateHomMat2D_Rotate = "������ת����";
	HTuple CreateHomMat2D_Scale = "�������ž���";
	HTuple CreateHomMat2D_SlantX = "����X��б�о���";
	HTuple CreateHomMat2D_SlantY = "����Y��б�о���";
	HTuple CreateHomMat2D_RigidShift_PP = "���������Ա任����";//����������
	HTuple CreateHomMat2D_RigidShift_PA = "������Ƕȸ��Ա任����";
	HTuple CreateHomMat2D_NPoints = "����N����α任����";//����������
	HTuple Add_Translate_ToHomMat2D = "���ƽ�ƾ���";
	HTuple Add_Rotate_ToHomMat2D = "�����ת����";
	HTuple Add_Scale_ToHomMat2D = "������ž���";
	HTuple Add_SlantX_ToHomMat2D = "���X��б�о���";
	HTuple Add_SlantY_ToHomMat2D = "���Y��б�о���";
};
// ��������ӷ���任���в����ṹ��
struct RunParamsStruct_CrateHomMat2D_AffineMatrix
{
	HTuple hv_HomMat2D_InPut;			//������µľ������͵ľ���(��������ʱ�ò�������ֻ�����о�������ӱ������ʱ���ã��������ƽ������)

	HTuple hv_Type_HomMat2D;			//��������

	HTuple OffsetX_Translat;			//ƽ�ƾ���X����ƽ�ƾ���
	HTuple OffsetY_Translat;			//ƽ�ƾ���Y����ƽ�ƾ���

	HTuple CenterX_Rotate;				//��ת��������X����
	HTuple CenterY_Rotate;				//��ת��������Y����
	HTuple Phi_Rotate;				    //��ת������ת�Ƕ�

	HTuple Sx_Scale;					//���ž���X��������ϵ��
	HTuple Sy_Scale;					//���ž���Y��������ϵ��
	HTuple Px_Scale;					//���ž����������ĵ�X
	HTuple Py_Scale;					//���ž����������ĵ�Y

	HTuple Theta_Slant;					//б�нǶ�
	HTuple Px_Slant;					//б�����ĵ�X
	HTuple Py_Slant;					//б�����ĵ�Y

	HTuple OriginalX_RigidShift_PP;     //�����Ա任����ԭʼ����X����
	HTuple OriginalY_RigidShift_PP;     //�����Ա任����ԭʼ����Y����
	HTuple DestX_RigidShift_PP;         //�����Ա任����Ŀ������X����
	HTuple DestY_RigidShift_PP;         //�����Ա任����Ŀ������Y����

	HTuple OriginalX_RigidShift_PA;     //��Ƕȸ��Ա任����ԭʼ����X
	HTuple OriginalY_RigidShift_PA;     //��Ƕȸ��Ա任����ԭʼ����Y
	HTuple OriginalA_RigidShift_PA;     //��Ƕȸ��Ա任����ԭʼ�Ƕ�
	HTuple DestX_RigidShift_PA;         //��Ƕȸ��Ա任����Ŀ������X
	HTuple DestY_RigidShift_PA;         //��Ƕȸ��Ա任����Ŀ������Y
	HTuple DestA_RigidShift_PA;         //��Ƕȸ��Ա任����Ŀ��Ƕ�

	HTuple Px_NPoints;					//N����α任����ԭʼ����X
	HTuple Py_NPoints;					//N����α任����ԭʼ����Y
	HTuple Qx_NPoints;					//N����α任����Ŀ������X
	HTuple Qy_NPoints;					//N����α任����Ŀ������X

};

//��������ṹ��
struct ResultParamsStruct_AffineMatrix
{
	HTuple hv_HomMat2D;					 //�������

	HTuple hv_Row_Affine;			     //����ת�����������
	HTuple hv_Column_Affine;			 //����ת�����������
	HObject ho_Image_Affine;			 //����ת�����ͼƬ
	HObject ho_Region_Affine;			 //����ת���������
	HObject ho_ContourXld_Affine;	     //����ת���������
	HTuple hv_RetNum = 0;				 //OK�������

};

//����任���ͽṹ��
struct TypeStruct_AffineMatrixDetect
{
	//�����������
	HTuple Create_HomMat2D_Translate = "����ƽ�ƾ���";
	HTuple CreateHomMat2D_Rotate = "������ת����";
	HTuple CreateHomMat2D_Scale = "�������ž���";
	HTuple CreateHomMat2D_SlantX = "����X��б�о���";
	HTuple CreateHomMat2D_SlantY = "����Y��б�о���";
	HTuple CreateHomMat2D_RigidShift_PA = "������Ƕȸ��Ա任����";
	HTuple CreateHomMat2D_ProjectionTrans = "����ͶӰ�任����";
	//Ӧ�þ������
	HTuple AffineTrans_Image = "ͼ��任";
	HTuple AffineTrans_Region = "����任";
	HTuple AffineTrans_Xld = "�����任";
	HTuple AffineTrans_Point = "�����任";
	HTuple ProjectionTrans_Image = "ͼ��ͶӰ�任";
	HTuple ProjectionTrans_Region = "����ͶӰ�任";
	HTuple ProjectionTrans_Xld = "����ͶӰ�任";
	HTuple hv_NULL = "null";
};
// ���в����ṹ��
struct RunParamsStruct_AffineMatrixDetect
{
	//**************�����������*******************************************
	HTuple hv_Type_CreateHomMat2D = "null";			//������������

	HTuple OffsetRow_Translate = 0;			//ƽ�ƾ���Row����ƽ�ƾ���
	HTuple OffsetColumn_Translate = 0;			//ƽ�ƾ���Column����ƽ�ƾ���

	HTuple CenterRow_Rotate = 0;				//��ת��������Row����
	HTuple CenterColumn_Rotate = 0;				//��ת��������Column����
	HTuple Phi_Rotate = 0;				    //��ת������ת�Ƕ�
	HTuple hv_ReverseAngle = 0;				//ǿ��ת��(0�����ã�1����)

	HTuple Srow_Scale = 1;					//���ž���Row��������ϵ��
	HTuple Scolumn_Scale = 1;					//���ž���Column��������ϵ��
	HTuple Row_Scale = 0;					//���ž����������ĵ�Row
	HTuple Column_Scale = 0;					//���ž����������ĵ�Column

	HTuple Theta_Slant = 0;					//б�нǶ�
	HTuple Row_Slant = 0;					//б�����ĵ�Row
	HTuple Column_Slant = 0;					//б�����ĵ�Column

	HTuple OriginalRow_RigidShift_PA = 0;     //��Ƕȸ��Ա任����ԭʼ����Row
	HTuple OriginalColumn_RigidShift_PA = 0;     //��Ƕȸ��Ա任����ԭʼ����Column
	HTuple OriginalA_RigidShift_PA = 0;     //��Ƕȸ��Ա任����ԭʼ�Ƕ�
	HTuple DestRow_RigidShift_PA = 0;         //��Ƕȸ��Ա任����Ŀ������Row
	HTuple DestColumn_RigidShift_PA = 0;         //��Ƕȸ��Ա任����Ŀ������Column
	HTuple DestA_RigidShift_PA = 0;         //��Ƕȸ��Ա任����Ŀ��Ƕ�

	HTuple hv_Row_LeftTop = 0;		//ͶӰ�任�ı������Ͻ�Row����			
	HTuple hv_Column_LeftTop = 0;		//ͶӰ�任���Ͻ�Column����		
	HTuple hv_Row_RightTop = 0;		//ͶӰ�任�ı������Ͻ�Row����			
	HTuple hv_Column_RightTop = 100;		//ͶӰ�任���Ͻ�Column����	
	HTuple hv_Row_RightBottom = 100;	//ͶӰ�任�ı������½�Row����			
	HTuple hv_Column_RightBottom = 100;	//ͶӰ�任���½�Column����	
	HTuple hv_Row_LeftBottom = 100;		//ͶӰ�任�ı������½�Row����			
	HTuple hv_Column_LeftBottom = 0;		//ͶӰ�任���½�Column����	

	//*********************Ӧ�þ������*********************************************
	HTuple hv_Type_ApplyHomMat2D = "null";			//Ӧ�þ���任����
	HObject ho_ImgInput;					//����ͼ�񣬴�ת��
	HObject ho_RegionInput;					//�������򣬴�ת��
	HObject ho_XldInput;					//������������ת��
	HTuple hv_RowInput = 0;					//�����Row���꣬��ת��
	HTuple hv_ColumnInput = 0;					//�����Column���꣬��ת��

};

//����任����ṹ��
struct ResultParamsStruct_AffineMatrixDetect
{
	//�������
	HTuple hv_HomMat2D = NULL;
	//����任�����
	HObject ho_ImgDest;
	HObject ho_RegionDest;
	HObject ho_XldDest;
	HTuple hv_RowDest;
	HTuple hv_ColumnDest;
};
//���������ͽṹ��
struct CodeTypeStruct_BarCode
{
	HTuple CODE128 = "Code 128";
	HTuple CODE39 = "Code 39";
	HTuple CODE93 = "Code 93";
	HTuple EAN13 = "EAN-13";
	HTuple EAN8 = "EAN-8";
	HTuple ITF25 = "2/5 Industrial";
	HTuple UPCA = "UPC-A";
	HTuple UPCE = "UPC-E";
	HTuple CODABAR = "Codabar";
	HTuple PHARMACODE = "PharmaCode";
	HTuple AUTO = "auto";
};
//һά�������в����ṹ��
struct RunParamsStruct_BarCode
{
	HObject ho_SearchRegion;					    //��������
	HTuple hv_CodeType_Run;							//��������(Ĭ��"auto"������ѡ��ṹ��CodeTypeStruct_BarCode����Ķ����),��ʱ����Ĭ��ֵ����Ϊ�����������͵�ʱ��auto����������ǰ�棬���±���
	HTuple hv_CodeNum = 1;						    //ʶ������(����Ϊ0����ɨ����������)
	HTuple hv_Tolerance = "high";                   //ʶ���ݲ�(Ĭ�ϸ߼���Ĭ��high������������"low")
	HTuple hv_HeadChar = "null";					    //�ض��ַ���ͷ
	HTuple hv_EndChar = "null";						    //�ض��ַ���β
	HTuple hv_ContainChar = "null";						//�����ض��ַ�
	HTuple hv_NotContainChar = "null";					//�������ض��ַ�
	HTuple hv_CodeLength_Run = 2;               //������С����
	HTuple hv_TimeOut = 50;						//��ʱʱ��(��λms)
	HTuple hv_HomMat2D = NULL;            //����任�����������
	bool isFollow = false;                //�Ƿ����
};
//������ʶ�����ṹ��
struct ResultParamsStruct_BarCode
{
	//����������
	HTuple hv_CodeType_Result;         //��������
	HTuple hv_CodeContent;			  //��������
	HTuple hv_CodeLength_Result;	  //���볤��
	HObject ho_ContourBarCode;		  //��������
	HObject ho_RegionBarCode;		  //����ʶ��������
	HTuple hv_Row;                    //��������������
	HTuple hv_Column;                 //��������������
	HTuple hv_RetNum = 0;			 //�ҵ����������(Ĭ����0��ʶ����������0)
	HTuple hv_QualityGrade;		    //���������ȼ�0-4(iso15416)
	
	void clear()
	{
		ho_ContourBarCode.Clear();
		ho_RegionBarCode.Clear();
	};

	~ResultParamsStruct_BarCode()
	{
		clear();
	}
};

//ͼ���ֵ�����ͽṹ��
struct TypeStruct_BinarizationDetect
{
	HTuple Fixed_Threshold = "�̶���ֵ";
	HTuple Auto_Threshold = "����Ӧ��ֵ";
	HTuple Dyn_Threshold = "��̬��ֵ";
	HTuple DynPolarity_Light = "light";
	HTuple DynPolarity_Dark = "dark";
	HTuple DynPolarity_Equal = "equal";
	HTuple DynPolarity_NotEqual = "not_equal";
	HTuple hv_Null = "null";
};
//ͼ���ֵ�����в����ṹ��
struct RunParamsStruct_BinarizationDetect
{
	HTuple hv_ThresholdType = "�̶���ֵ";					//ͼ���ֵ������
	HTuple hv_LowThreshold = 50;						//����ֵ(�̶���ֵ)
	HTuple hv_HighThreshold = 128;					//����ֵ(�̶���ֵ)
	HTuple hv_Sigma = 1;						//ƽ����ֵ
	HTuple hv_CoreWidth = 3;					//��̬��ֵ(�˲��˿��)
	HTuple hv_CoreHeight = 3;					//��̬��ֵ(�˲��˸߶�)
	HTuple hv_DynThresholdContrast = 10;		//��̬��ֵ�ָ�ͼ��Աȶ�
	HTuple hv_DynThresholdPolarity = "dark";	//��̬��ֵ����ѡ��(ѡ����ȡ���������������)��light,dark

	HObject ho_SearchRegion;			 //��������
	HTuple hv_HomMat2D = NULL;            //����任����ץ�߸���
	bool isFollow = false;                //�Ƿ����

};
//ͼ���ֵ����������ṹ��
struct ResultParamsStruct_BinarizationDetect
{
	HObject ho_DestImage;  //��ֵ��ͼ��
	HObject ho_DestRegions;     //��ֵ������
	HTuple  hv_RetNum = 0;    //��ֵ����������(Ĭ����0��ok����0)
	//��ֵ��������̬ѧ�������
	HTuple hv_Area;
	HTuple hv_Row;
	HTuple hv_Column;
	HTuple hv_Angle;
	HTuple hv_Circularity;
	HTuple hv_Compactness;
	HTuple hv_Convexity;
	HTuple hv_Rectangularity;
	HTuple hv_Height;
	HTuple hv_Width;
	HTuple hv_OuterRadius;
	HTuple hv_InnerRadius;
	HTuple hv_Rect2Len1;
	HTuple hv_Rect2Len2;
	HTuple hv_Rect2Angle;

};

//��������������ͽṹ��
struct TypeStruct_RegionOperator_MorphologyDetect
{
	HTuple hv_Dilation_Rectangle = "��������";
	HTuple hv_Dilation_Circle = "Բ������";
	HTuple hv_Erosion_Rectangle = "���θ�ʴ";
	HTuple hv_Erosion_Circle = "Բ�θ�ʴ";
	HTuple hv_Opening_Rectangle = "���ο�����";
	HTuple hv_Opening_Circle = "Բ�ο�����";
	HTuple hv_Closing_Rectangle = "���α�����";
	HTuple hv_Closing_Circle = "Բ�α�����";
	HTuple hv_BottomHat_Rectangle = "��ñ������νṹ��";
	HTuple hv_BottomHat_Circle = "��ñ����Բ�νṹ��";
	HTuple hv_TopHat_Rectangle = "��ñ������νṹ��";
	HTuple hv_TopHat_Circle = "��ñ����Բ�νṹ��";
	HTuple hv_Fillup = "�׶�ȫ�����";
	HTuple hv_FillUp_Shape = "�׶�������";
	HTuple hv_Connection = "������ͨ��";
	HTuple hv_Union1 = "��������ϲ�";
	HTuple hv_Union2 = "��������ϲ�";
	HTuple hv_Complement = "����";
	HTuple hv_Difference = "�";
	HTuple hv_Intersection = "����";
	HTuple hv_ConcatObj = "�ͼ�";//������ӵõ���������
	HTuple hv_Null = "null";
};

//����ɸѡ�������ͽṹ��
struct TypeStruct_FeaturesFilter_MorphologyDetect
{
	//HTuple hv_Area_Filter = "area";//���
	//HTuple hv_Row_Filter = "row";//�����꣨Y��
	//HTuple hv_Column_Filter = "column";//������(X)
	//HTuple hv_Angle_Filter = "orientation";//�Ƕ�
	//HTuple hv_Circularity_Filter = "circularity";//Բ��
	//HTuple hv_Compactness_Filter = "compactness";//���ܶ�
	//HTuple hv_Convexity_Filter = "convexity";//͹��
	//HTuple hv_Rectangularity_Filter = "rectangularity";//���ζ�
	//HTuple hv_Height_Filter = "height";//�߶�
	//HTuple hv_Width_Filter = "width";//���
	//HTuple hv_OuterRadius_Filter = "outer_radius";//��С���Բ�뾶
	//HTuple hv_InnerRadius_Filter = "inner_radius";//����ڽ�Բ�뾶
	//HTuple hv_Rect2Len1_Filter = "rect2_len1";//��С��Ӿذ볤
	//HTuple hv_Rect2Len2_Filter = "rect2_len2";//��С��Ӿذ��
	//HTuple hv_Rect2Angle_Filter = "rect2_phi";//��С��ӾؽǶ�
	HTuple hv_Area_Filter = "���";//area
	HTuple hv_Column_Filter = "X";//������(X)column
	HTuple hv_Row_Filter = "Y";//�����꣨Y��row
	HTuple hv_Angle_Filter = "�Ƕ�";//orientation
	HTuple hv_Circularity_Filter = "Բ��";//circularity
	HTuple hv_Compactness_Filter = "���ܶ�";//compactness
	HTuple hv_Convexity_Filter = "͹��";//convexity
	HTuple hv_Rectangularity_Filter = "���ζ�";//rectangularity
	HTuple hv_Height_Filter = "�߶�";//height
	HTuple hv_Width_Filter = "���";//width
	HTuple hv_OuterRadius_Filter = "��С���Բ�뾶";//outer_radius
	HTuple hv_InnerRadius_Filter = "����ڽ�Բ�뾶";//inner_radius
	HTuple hv_Rect2Len1_Filter = "��С��Ӿذ볤";//rect2_len1
	HTuple hv_Rect2Len2_Filter = "��С��Ӿذ��";//rect2_len2
	HTuple hv_Rect2Angle_Filter = "��С��ӾؽǶ�";//rect2_phi
};

//��̬ѧ���в����ṹ��
struct RunParamsStruct_MorphologyDetect
{
	//�����������
	HTuple hv_RegionOperator_Type;				 //���飬�����������(��Ҫʹ�ýṹ��TypeStruct_RegionOperator_MorphologyDetect����Ĳ���)
	HTuple hv_Height_Operator;					//���飬���νṹԪ�س���
	HTuple hv_Width_Operator;					//���飬���νṹԪ�ؿ��
	HTuple hv_CircleRadius_Operator;			//���飬Բ�νṹԪ�ذ뾶
	HTuple hv_MinArea_FillUpShape_Operator;				//�׶������С���
	HTuple hv_MaxArea_FillUpShape_Operator;				//�׶����������

	//*******************************************************************************************************************************
	//ɸѡ�㷨����
	//����ɸѡ����
	HTuple hv_FeaturesFilter_Type;				 //���飬����ɸѡ��������(��Ҫʹ�ýṹ��TypeStruct_FeaturesFilter_MorphologyDetect����Ĳ���)
	HTuple hv_And_Or_Filter = "and";			//����ɸѡ�������֮��ѡ��"and"����"or",Ĭ��"and"
	HTuple hv_MinValue_Filter;					//���飬����ɸѡ������Сֵ
	HTuple hv_MaxValue_Filter;					//���飬����ɸѡ�������ֵ
};


//��̬ѧ��������ṹ��
struct ResultParamsStruct_MorphologyDetect
{
	HObject ho_DestRegions;     //��̬ѧ�������
	HTuple hv_RetNum = 0;    //�����������
	//����ɸѡ�������
	HTuple hv_Area;
	HTuple hv_Row;
	HTuple hv_Column;
	HTuple hv_Angle;
	HTuple hv_Circularity;
	HTuple hv_Compactness;
	HTuple hv_Convexity;
	HTuple hv_Rectangularity;
	HTuple hv_Height;
	HTuple hv_Width;
	HTuple hv_OuterRadius;
	HTuple hv_InnerRadius;
	HTuple hv_Rect2Len1;
	HTuple hv_Rect2Len2;
	HTuple hv_Rect2Angle;
};

//ͼ���ֵ�����ͽṹ��
struct TypeStruct_Binarization_Blob
{
	HTuple Fixed_Threshold = "�̶���ֵ";
	HTuple Auto_Threshold = "����Ӧ��ֵ";
	HTuple Dyn_Threshold = "��̬��ֵ";
	HTuple DynPolarity_Light = "light";
	HTuple DynPolarity_Dark = "dark";
	HTuple DynPolarity_Equal = "equal";
	HTuple DynPolarity_NotEqual = "not_equal";
	HTuple hv_Null = "null";
};
//��������������ͽṹ��
struct TypeStruct_RegionOperator_Blob
{
	HTuple hv_Dilation_Rectangle = "��������";
	HTuple hv_Dilation_Circle = "Բ������";
	HTuple hv_Erosion_Rectangle = "���θ�ʴ";
	HTuple hv_Erosion_Circle = "Բ�θ�ʴ";
	HTuple hv_Opening_Rectangle = "���ο�����";
	HTuple hv_Opening_Circle = "Բ�ο�����";
	HTuple hv_Closing_Rectangle = "���α�����";
	HTuple hv_Closing_Circle = "Բ�α�����";
	HTuple hv_BottomHat_Rectangle = "��ñ������νṹ��";
	HTuple hv_BottomHat_Circle = "��ñ����Բ�νṹ��";
	HTuple hv_TopHat_Rectangle = "��ñ������νṹ��";
	HTuple hv_TopHat_Circle = "��ñ����Բ�νṹ��";
	HTuple hv_Fillup = "�׶�ȫ�����";
	HTuple hv_FillUp_Shape = "�׶�������";
	HTuple hv_Connection = "������ͨ��";
	HTuple hv_Union1 = "��������ϲ�";
	HTuple hv_Complement = "����";
	HTuple hv_Null = "null";
};

//����ɸѡ�������ͽṹ��
struct TypeStruct_FeaturesFilter_Blob
{
	HTuple hv_Area_Filter = "���";//area
	HTuple hv_Column_Filter = "X";//������(X)column
	HTuple hv_Row_Filter = "Y";//�����꣨Y��row
	HTuple hv_Angle_Filter = "�Ƕ�";//orientation
	HTuple hv_Circularity_Filter = "Բ��";//circularity
	HTuple hv_Compactness_Filter = "���ܶ�";//compactness
	HTuple hv_Convexity_Filter = "͹��";//convexity
	HTuple hv_Rectangularity_Filter = "���ζ�";//rectangularity
	HTuple hv_Height_Filter = "�߶�";//height
	HTuple hv_Width_Filter = "���";//width
	HTuple hv_OuterRadius_Filter = "��С���Բ�뾶";//outer_radius
	HTuple hv_Rect2Len1_Filter = "��С��Ӿذ볤";//rect2_len1
	HTuple hv_Rect2Len2_Filter = "��С��Ӿذ��";//rect2_len2
	HTuple hv_Rect2Angle_Filter = "��С��ӾؽǶ�";//rect2_phi
};
//Blob���в����ṹ��
struct RunParamsStruct_Blob
{
	HTuple hv_IsUsSearchRoi = 0;//0 ��������������1 ������������
	//�������򣬾��ε������ǵ�
	cv::Point P1_SearchRoi = cv::Point(10, 200);
	cv::Point P2_SearchRoi = cv::Point(10, 200);
	//��ֵ�����в���
	HTuple hv_ThresholdType = "�̶���ֵ";					//ͼ���ֵ������
	HTuple hv_LowThreshold = 50;						//����ֵ(�̶���ֵ)
	HTuple hv_HighThreshold = 128;					//����ֵ(�̶���ֵ)
	HTuple hv_Sigma = 1;						//ƽ����ֵ
	HTuple hv_CoreWidth = 3;					//��̬��ֵ(�˲��˿��)
	HTuple hv_CoreHeight = 3;					//��̬��ֵ(�˲��˸߶�)
	HTuple hv_DynThresholdContrast = 10;		//��̬��ֵ�ָ�ͼ��Աȶ�
	HTuple hv_DynThresholdPolarity = "dark";	//��̬��ֵ����ѡ��(ѡ����ȡ���������������)��light,dark

	//��̬ѧ���в���
	HTuple hv_RegionOperator_Type;				 //���飬�����������(��Ҫʹ�ýṹ��TypeStruct_RegionOperator_MorphologyDetect����Ĳ���)
	HTuple hv_Height_Operator;					//���飬���νṹԪ�س���
	HTuple hv_Width_Operator;					//���飬���νṹԪ�ؿ��
	HTuple hv_CircleRadius_Operator;			//���飬Բ�νṹԪ�ذ뾶
	HTuple hv_MinArea_FillUpShape_Operator;				//�׶������С���
	HTuple hv_MaxArea_FillUpShape_Operator;				//�׶����������
	//����ɸѡ����
	HTuple hv_FeaturesFilter_Type;				 //���飬����ɸѡ��������(��Ҫʹ�ýṹ��TypeStruct_FeaturesFilter_MorphologyDetect����Ĳ���)
	HTuple hv_And_Or_Filter = "and";			//����ɸѡ�������֮��ѡ��"and"����"or",Ĭ��"and"
	HTuple hv_MinValue_Filter;					//���飬����ɸѡ������Сֵ
	HTuple hv_MaxValue_Filter;					//���飬����ɸѡ�������ֵ

	HTuple hv_SetBlobCount = 1;

	HTuple hv_HomMat2D = NULL;            //����任����ץ�߸���
	bool isFollow = false;                //�Ƿ����
};
//Blob��������ṹ��
struct ResultParamsStruct_Blob
{
	HObject ho_DestImage;  //��ֵ��ͼ��
	HObject ho_DestRegions;     //�������
	HTuple hv_RetNum = 0;    //�����������
	//����ɸѡ�������
	HTuple hv_Area;
	HTuple hv_Row;
	HTuple hv_Column;
	HTuple hv_Angle;
	HTuple hv_Circularity;
	HTuple hv_Compactness;
	HTuple hv_Convexity;
	HTuple hv_Rectangularity;
	HTuple hv_Height;
	HTuple hv_Width;
	HTuple hv_OuterRadius;
	HTuple hv_Rect2Len1;
	HTuple hv_Rect2Len2;
	HTuple hv_Rect2Angle;

	void clear()
	{
		ho_DestImage.Clear();
		ho_DestRegions.Clear();
	};

	~ResultParamsStruct_Blob()
	{
		clear();
	}
};

//���в����ṹ��
struct MapImgType_CalibBoardDetect
{
	HTuple hv_Deformity_MapImg = "�������";
	HTuple hv_Deformity_Projection_MapImg = "�����͸�ӽ���";
	HTuple hv_Null = "null";
};
//���в����ṹ��
struct RunParamsStruct_CalibBoardDetect
{
	HObject ho_Image;//�궨��ͼ����ʱʹ��һ��(������һ�Ż����Ƕ���)
	HTuple hv_StartCamPar;//�궨��Ҫ�������
	HTuple hv_CaltabDescrPath;//�궨�������ļ�·��(HalconԲ��궨�壬��".descr"��ʽ��)
};
//��������ṹ��
struct ResultParamsStruct_CalibBoardDetect
{
	HObject ho_CalibContours;// �궨���·����
	HTuple hv_CamParam;//�궨�������ڲ�
	HTuple hv_CamPose;//�궨���������
	HTuple hv_Error;//�궨���(�Ż���ͶӰ�ľ�������RMSE������������Ϊ��λ������)
	HTuple hv_Resolution;//���ص���
	HTuple hv_DistanceCC; //�궨�������Բ�ľ�
	HTuple hv_MapImgType = "null"; //ͼ���������
	HObject hv_DestImg;//����������ͼ��
};

// ����ֱ�߲����ṹ��

//ץ�����в����ṹ��
struct RunParamsStruct_Line
{
	//��������
	//ֱ��ץ��roi��ʼ��
	HTuple hv_Row1 = 10;
	HTuple hv_Column1 = 10;
	HTuple hv_Row2 = 10;
	HTuple hv_Column2 = 50;

	//ץ��ģ����Ҫ��ץ�߲���
	HTuple hv_MeasureLength1 = 50;		//���߰볤
	HTuple hv_MeasureLength2 = 5;		//���߰��
	HTuple hv_MeasureSigma = 1.0;		//ƽ��ֵ 
	HTuple hv_MeasureThreshold = 20;	//��Ե��ֵ
	HTuple hv_MeasureTransition = "all";	//���߼���
	HTuple hv_MeasureSelect = "first";		//��Եѡ��
	HTuple hv_MeasureNum = 10;		    //���߸���
	HTuple hv_InstancesNum = 1;		//ץ������
	HTuple hv_MeasureMinScore = 0.5;     //��С�÷�
	HTuple hv_DistanceThreshold = 3.5;   //������ֵ
	HTuple hv_MeasureInterpolation = "nearest_neighbor";//��ֵ�㷨

	HTuple hv_HomMat2D = NULL;            //����任����ץ�߸���
	bool isFollow = false;                //�Ƿ����
	HTuple hv_MmPixel = 1;	//���ص���
};
//ץ�߽�������ṹ��
struct ResultParamsStruct_Line
{
	//��⵽��ֱ�߽������
	HTuple hv_LineRowBegin;
	HTuple hv_LineColumnBegin;
	HTuple hv_LineRowEnd;
	HTuple hv_LineColumnEnd;
	//�����ҵ������е�����
	HTuple hv_PointsRow;
	HTuple hv_PointsColumn;

	HTuple hv_RetNum = 0;           //�ҵ���ֱ�߸���(Ĭ����0���ҵ�ֱ�ߺ���ֵ����0)

	HTuple hv_PointsNum = 0;		//�����ҵ��ĵ������
	HObject ho_Contour_Line;      // ֱ������
	HObject ho_Contour_Calipers;  //��������
	HObject ho_Contour_CrossPoints;//���е�ʮ������

	//�������е�������ֱ�ߵľ�������
	HTuple hv_DisToLineArray;
	//ֱ�߶�
	HTuple hv_Straightness = 99999;
};

//ץԲ���в����ṹ��
struct RunParamsStruct_Circle
{
	//��������
	//ץԲroiԲ�ĺͰ뾶
	HTuple hv_Row = 50;
	HTuple hv_Column = 50;
	HTuple hv_Radius = 50;

	//ץԲģ����Ҫ�Ĳ���
	HTuple hv_MeasureLength1 = 50;		//���߰볤
	HTuple hv_MeasureLength2 = 5;		//���߰��
	HTuple hv_MeasureSigma = 1.0;		//ƽ��ֵ 
	HTuple hv_MeasureThreshold = 20;	//��Ե��ֵ
	HTuple hv_MeasureTransition = "all";	//���߼���
	HTuple hv_MeasureSelect = "first";		//��Եѡ��
	HTuple hv_MeasureNum = 10;		    //���߸���
	HTuple hv_InstancesNum = 1;		//ץ������
	HTuple hv_MeasureMinScore = 0.5;     //��С�÷�
	HTuple hv_DistanceThreshold = 3.5;   //������ֵ
	HTuple hv_MeasureInterpolation = "nearest_neighbor";//��ֵ�㷨

	HTuple hv_HomMat2D = NULL;            //����任����ץ�߸���
	bool isFollow = false;                //�Ƿ����
	HTuple hv_MmPixel = 1;	//���ص���
		//�Ƿ�ѡ�þ��ο�
	HTuple hv_DectType = 0;
};
//ץԲ��������ṹ��
struct ResultParamsStruct_Circle
{
	//��⵽��Բ�������
	HTuple hv_CircleRowCenter;
	HTuple hv_CircleColumnCenter;
	HTuple hv_CircleRadius;
	//�����ҵ������е�����
	HTuple hv_PointsRow;
	HTuple hv_PointsColumn;

	HTuple hv_RetNum = 0;           //�ҵ���Բ����(Ĭ����0���ҵ�Բ����ֵ����0)

	HTuple hv_PointsNum = 0;		//�����ҵ��ĵ������
	HObject ho_Contour_Circle;      // Բ����
	HObject ho_Contour_Calipers;  //��������
	HObject ho_Contour_CrossPoints;//���е�ʮ������

	//�������е�������Բ��Բ�ļ�ȥ�뾶��������
	HTuple hv_DisToCircleArray;
	//Բ��
	HTuple hv_Roundness = 99999;

	void clear()
	{
		ho_Contour_Circle.Clear();
		ho_Contour_Calipers.Clear();
		ho_Contour_CrossPoints.Clear();
	};

	~ResultParamsStruct_Circle()
	{
		clear();
	}
};

//���в����ṹ��
struct RunParamsStruct_Rectangle
{
	//��������
	//���μ����ʼ����
	HTuple hv_Row = 10;
	HTuple hv_Column = 10;
	HTuple hv_Angle = 0;
	HTuple hv_Length1 = 50;
	HTuple hv_Length2 = 5;

	//ģ����Ҫ�Ĳ���
	HTuple hv_MeasureLength1 = 50;		//���߰볤
	HTuple hv_MeasureLength2 = 5;		//���߰��
	HTuple hv_MeasureSigma = 1.0;		//ƽ��ֵ 
	HTuple hv_MeasureThreshold = 20;	//��Ե��ֵ
	HTuple hv_MeasureTransition = "all";	//���߼���
	HTuple hv_MeasureSelect = "first";		//��Եѡ��
	HTuple hv_MeasureNum = 10;		    //���߸���
	HTuple hv_InstancesNum = 1;		//ץ������
	HTuple hv_MeasureMinScore = 0.5;     //��С�÷�
	HTuple hv_DistanceThreshold = 3.5;   //������ֵ
	HTuple hv_MeasureInterpolation = "nearest_neighbor";//��ֵ�㷨

	HTuple hv_HomMat2D = NULL;            //����任����ץ�߸���
	HTuple hv_TemplateAngle = 0;	//�����ģ��Ƕ�
	bool isFollow = false;                //�Ƿ����
};
//���μ���������ṹ��
struct ResultParamsStruct_Rectangle
{
	//��⵽�ľ��ν������
	HTuple hv_RectangleRow;
	HTuple hv_RectangleColumn;
	HTuple hv_RectangleAngle;
	HTuple hv_RectangleLength1;
	HTuple hv_RectangleLength2;
	//�����ҵ������е�����
	HTuple hv_PointsRow;
	HTuple hv_PointsColumn;

	HTuple hv_RetNum = 0;           //�ҵ��ľ��θ���(Ĭ����0���ҵ�ֱ�ߺ���ֵ����0)

	HTuple hv_PointsNum = 0;		//�����ҵ��ĵ������
	HObject ho_Contour_Rectangle;      // ��������
	HObject ho_Contour_Calipers;  //��������
	HObject ho_Contour_CrossPoints;//���е�ʮ������
};

//���в����ṹ��
struct RunParamsStruct_Ellipse
{
	//��������
	//��Բ�μ����ʼ����
	HTuple hv_Row = 10;
	HTuple hv_Column = 10;
	HTuple hv_Angle = 0;
	HTuple hv_Radius1 = 50;
	HTuple hv_Radius2 = 25;

	//ģ����Ҫ�Ĳ���
	HTuple hv_MeasureLength1 = 50;		//���߰볤
	HTuple hv_MeasureLength2 = 5;		//���߰��
	HTuple hv_MeasureSigma = 1.0;		//ƽ��ֵ 
	HTuple hv_MeasureThreshold = 20;	//��Ե��ֵ
	HTuple hv_MeasureTransition = "all";	//���߼���
	HTuple hv_MeasureSelect = "first";		//��Եѡ��
	HTuple hv_MeasureNum = 10;		    //���߸���
	HTuple hv_InstancesNum = 1;		//ץ������
	HTuple hv_MeasureMinScore = 0.5;     //��С�÷�
	HTuple hv_DistanceThreshold = 3.5;   //������ֵ
	HTuple hv_MeasureInterpolation = "nearest_neighbor";//��ֵ�㷨

	HTuple hv_HomMat2D = NULL;            //����任����ץ�߸���
	HTuple hv_TemplateAngle = 0;	//�����ģ��Ƕ�
	bool isFollow = false;                //�Ƿ����
};
//��Բ����������ṹ��
struct ResultParamsStruct_Ellipse
{
	//��⵽����Բ�ν������
	HTuple hv_EllipseRow;
	HTuple hv_EllipseColumn;
	HTuple hv_EllipseAngle;
	HTuple hv_EllipseRadius1;
	HTuple hv_EllipseRadius2;
	HTuple hv_EllipseStartAngle = 0;
	HTuple hv_EllipseEndAngle = 360;
	//�����ҵ������е�����
	HTuple hv_PointsRow;
	HTuple hv_PointsColumn;

	HTuple hv_RetNum = 0;           //�ҵ�����Բ�θ���(Ĭ����0���ҵ���Բ����ֵ����0)

	HTuple hv_PointsNum = 0;		//�����ҵ��ĵ������
	HObject ho_Contour_Ellipse;      // ��Բ����
	HObject ho_Contour_Calipers;  //��������
	HObject ho_Contour_CrossPoints;//���е�ʮ������
};

//��ɫ������в����ṹ��
struct RunParamsStruct_ColorDetect
{
	HObject ho_SearchRegion;				 //��������
	HTuple hv_ColorHandle = NULL;			//��ɫ�����
	HObject ho_TrainRegions;		 //��ѵ������ɫ����
	HTuple hv_MeanValue_R = 128;			//Rͨ��ƽ���Ҷ�ֵ
	HTuple hv_MeanValue_G = 128;			//Gͨ��ƽ���Ҷ�ֵ
	HTuple hv_MeanValue_B = 128;			//Bͨ��ƽ���Ҷ�ֵ
	HTuple hv_MinScore = 0.5;				//��С����(���ڴ˷�������ΪNG)
	HTuple hv_HomMat2D = NULL;            //����任����ץ�߸���
	bool isFollow = false;                //�Ƿ����

};
//��ɫ����������ṹ��
struct ResultParamsStruct_ColorDetect
{
	HTuple hv_Score;			//ÿ����ɫ������Եķ���(������Ϊ��ǰͼƬ�Ǻ�ɫ�����Ŷ���0.65����Ϊ��ǰͼƬ����ɫ�����Ŷ���0.15)
	HObject ho_DestRegions;     //��ɫʶ��������
	HTuple hv_AreaTotal;		//ÿ����ɫ������Ե������(�����ҵ���ɫ�Ĳ�Ʒ3���������600.�����ҵ���ɫɫ�Ĳ�Ʒ1���������100)

};

//��ɫ������в����ṹ��
struct RunParamsStruct_ColorDetect_MoreNum
{
	HObject ho_SearchRegion;				 //��������
	HTuple hv_ColorHandle = NULL;			//��ɫ�����
	vector<HTuple> hv_ColorName;			 //��Ҫʶ�����ɫ����
	vector<HObject> ho_TrainRegions;		 //��ѵ������ɫ����
	vector<HTuple> hv_MeanValue_R;			//Rͨ��ƽ���Ҷ�ֵ
	vector<HTuple> hv_MeanValue_G;			//Gͨ��ƽ���Ҷ�ֵ
	vector<HTuple> hv_MeanValue_B;			//Bͨ��ƽ���Ҷ�ֵ

};
//��ɫ����������ṹ��
struct ResultParamsStruct_ColorDetect_MoreNum
{
	vector<HTuple>  hv_ColorName;		//ʶ�𵽵���ɫ��������
	vector<HObject> ho_DestRegions;     //��ɫʶ������������
	vector<HTuple> hv_Score;			//ÿ����ɫ������Եķ���(������Ϊ��ǰͼƬ�Ǻ�ɫ�����Ŷ���0.65����Ϊ��ǰͼƬ����ɫ�����Ŷ���0.15)
	vector<HTuple> hv_AreaTotal;		//ÿ����ɫ������Ե������(�����ҵ���ɫ�Ĳ�Ʒ3���������600.�����ҵ���ɫɫ�Ĳ�Ʒ1���������100)

};

//����任���ͽṹ��
struct TypeStruct_CreateContourDetect
{
	HTuple hv_Null = "null";
	HTuple hv_Polygon = "polygon"; //"��������е�������������";
	HTuple hv_Region = "region"; //"������������"
	HTuple hv_Skeleton = "skeleton"; // "�Ǽ���������"
};
// ���в����ṹ��
struct RunParamsStruct_CreateContourDetect
{
	HTuple hv_TypeCreateContour = "region";		//ת������
	HTuple hv_Rows_Polygon;	//����ε�����������
	HTuple hv_Cols_Polygon;	//����ε�����������
	HObject hv_InputRegion;	//��������
	HObject hv_InputSkeleton;//����Ǽ�
};
//��������ṹ��
struct ResultParamsStruct_CreateContourDetect
{
	HObject hv_DestXLD;					 //�������

	HTuple hv_Rows_XLD;					 //������������
	HTuple hv_Columns_XLD;				 //������������
};

//��Ҫ����ROI�����ͽṹ��
struct TypeStruct_CreateRoiDetect
{
	HTuple hv_Circle = "Բ";
	HTuple hv_Ellipse = "��Բ";
	HTuple hv_Rectangle1 = "����";
	HTuple hv_Rectangle2 = "����ת����";
	HTuple hv_Line = "ֱ��";
	HTuple hv_NULL = "null";
};
//��������������ͽṹ��
struct RunParamsStruct_CreateRoiDetect
{
	HTuple hv_TypeRoi = "null";				//ROI����

	HTuple hv_Row_Circle = 0;			//Բ��row
	HTuple hv_Column_Circle = 0;		//Բ��column
	HTuple hv_Radius_Circle = 10;		//Բ�뾶

	HTuple hv_Row_Ellipse = 0;			//��ԲԲ��row
	HTuple hv_Column_Ellipse = 0;		//��ԲԲ��column
	HTuple hv_Angle_Ellipse = 0;		//��Բ��������ˮƽ�߽Ƕ�
	HTuple hv_Radius1_Ellipse = 20;		//��Բ������
	HTuple hv_Radius2_Ellipse = 10;		//��Բ�̰���

	HTuple hv_Row1_Rectangle1 = 0;		//�������Ͻ�row
	HTuple hv_Column1_Rectangle1 = 0;	//�������Ͻ�column
	HTuple hv_Row2_Rectangle1 = 20;		//�������½�row
	HTuple hv_Column2_Rectangle1 = 10;	//�������½�column

	HTuple hv_Row_Rectangle2 = 0;		//��ת��������row
	HTuple hv_Column_Rectangle2 = 0;	//��ת��������column
	HTuple hv_Angle_Rectangle2 = 0;     //��ת����������ˮƽ�߽Ƕ�
	HTuple hv_Length1_Rectangle2 = 20;   //��ת���ΰ��
	HTuple hv_Length2_Rectangle2 = 10;   //��ת���ΰ��

	HTuple hv_RowBegin_Line = 0;					//ֱ�����Row
	HTuple hv_ColumnBegin_Line = 0;				//ֱ�����Col
	HTuple hv_RowEnd_Line = 20;					//ֱ���յ�Row
	HTuple hv_ColumnEnd_Line = 10;				//ֱ���յ�Col
};

//�ü����ͽṹ��
struct TypeStruct_CutImgDetect
{
	HTuple hv_CropDomain = "CropDomain";
	HTuple hv_ReduceDomain = "ReduceDomain";
	HTuple hv_NULL = "null";
};
// ���в����ṹ��
struct RunParamsStruct_CutImgDetect
{
	HTuple hv_CutImgType = "ReduceDomain";	//��ͼ����
};

//������ͽṹ��
struct TypeStruct_FitDetect
{
	HTuple hv_Circle = "Բ";
	HTuple hv_Ellipse = "��Բ";
	HTuple hv_Rectangle = "����";
	HTuple hv_Line = "ֱ��";
	HTuple hv_NULL = "null";
};
//������в����ṹ��
struct RunParamsStruct_FitDetect
{
	HTuple hv_FitType = "null";
	vector<HTuple> hv_Rows;//���������������
	vector<HTuple> hv_Columns;//���������������
};
//��Ͻ�������ṹ��
struct ResultParamsStruct_FitDetect
{
	ResultParamsStruct_Line Line_FitResult;
	ResultParamsStruct_Circle Circle_FitResult;
	ResultParamsStruct_Rectangle Rectangle_FitResult;
	ResultParamsStruct_Ellipse Ellipse_FitResult;
};

//����Ҷ�任���ͼ�����ͽṹ��
struct ResultType_FourierTransDetect
{
	HTuple hv_Byte = "byte";
	HTuple hv_Real = "real";
};
//����Ҷ�任����˲������ͽṹ��
struct FilterType_FourierTransDetect
{
	HTuple hv_GaussFilter = "��˹�˲���";
	HTuple hv_MeanFilter = "��ֵ�˲���";
	HTuple hv_Highpass = "��ͨ�˲���";
	HTuple hv_Lowpass = "��ͨ�˲���";
	HTuple hv_SinBandPass = "���Ҵ�ͨ�˲���";
	HTuple hv_BandPass = "��ͨ�˲���";
	HTuple hv_BandFilter = "�����˲���";
	HTuple hv_GaussBandFilter = "��˹��ͨ�˲���";
};
//���в����ṹ��
struct RunParamsStruct_FourierTransDetect
{
	HTuple hv_ResultType = "real";		 //�任��Ľ��ͼ����
	HTuple hv_FilterType = "��˹�˲���";		 //�������
	HTuple hv_Sigma = 1.0;			 //��˹ƽ����ֵ
	HTuple hv_MaskWidth = 3;		 //��ֵ�˲�����
	HTuple hv_MaskHeight = 3;		 //��ֵ�˲�����	 
	HTuple hv_Frequency = 0.1;		 //��ͨ���ͨ�������ҽ�ֹƵ��(��Χ0-1)
	HTuple hv_BandMinFrequency = 0.1;		 //��ͨ��СƵ��(��Χ0-1)
	HTuple hv_BandMaxFrequency = 0.2;		 //��ͨ���Ƶ��(��Χ0-1)
	HTuple hv_GaussBandMinSigma = 3.0;	//��˹��ͨ��СSigma
	HTuple hv_GaussBandMaxSigma = 10.0;	//��˹��ͨ���Sigma
};

// ���в����ṹ��
struct RunParamsStruct_ImgClassifyDetect
{
	HTuple hv_ClassMethod = "MLP";	//MLP����SVM
	vector<HTuple> hv_ImgFilePaths;	//ѵ��ͼƬ�ļ�������
	vector<HTuple> hv_ClassNames;	//ѵ����������
	HTuple hv_ClassifyHandle;	//ѵ�����
	HTuple hv_ErrorData;	//ѵ�����
};
// ��������ṹ��
struct ResultParamsStruct_ImgClassifyDetect
{
	HTuple hv_RetName;	//ʶ��������
	HTuple hv_Confidence;	//ʶ�������Ŷ�
};

//ͼ��任���ͽṹ��
struct TypeStruct_ImgConvertDetect
{
	HTuple RGB_ToGray = "��ɫת�Ҷ�ͼ";
	HTuple Img_ToRGB = "��ɫתRGB";
	HTuple RGB_ToHSV = "��ɫתHSV";
	HTuple RGB_ToYUV = "��ɫתYUV";
	HTuple RGB_ToHSI = "��ɫתHSI";
};
//ͼ��任���в����ṹ��
struct RunParamsStruct_ImgConvertDetect
{
	HTuple hv_ImgConvertDetectType;				//ͼ��任����
};
//ͼ��任��������ṹ��
struct ResultParamsStruct_ImgConvertDetect
{
	HObject ho_GrayImage;  //�Ҷ�ͼ��
	//RGBͨ��ͼ��
	HObject ho_RgbR;
	HObject ho_RgbG;
	HObject ho_RgbB;
	//HSVͨ��ͼ��
	HObject ho_HsvH;
	HObject ho_HsvS;
	HObject ho_HsvV;
	//HSIͨ��ͼ��
	HObject ho_HsiH;
	HObject ho_HsiS;
	HObject ho_HsiI;
	//YUVͨ��ͼ��
	HObject ho_YuvY;
	HObject ho_YuvU;
	HObject ho_YuvV;
};

//ͼ����ǿ���ͽṹ��
struct TypeStruct_ImgProcessOneDetect
{
	HTuple Gauss_Filter = "��˹�˲�";
	HTuple Bilateral_Filter = "˫���˲�";
	HTuple Mean_Image = "��ֵ�˲�";
	HTuple Median_Rect = "��ֵ�˲�";
	HTuple Shock_Filter = "������˲�";
	HTuple Equ_Histo_Image = "ֱ��ͼ����";
	HTuple Emphasize = "�Աȶ���ǿ";
	HTuple Scale_Image = "�Ҷ�����";
	HTuple Scale_ImageMax = "�Ҷ�������0-255";
	HTuple Invert_Image = "�Ҷȷ�ת";
	HTuple Log_Image = "�����任";
	HTuple Exp_Image = "ָ���任";
	HTuple GrayErosion_Rect = "�Ҷȸ�ʴ";
	HTuple GrayDilation_Rect = "�Ҷ�����";
	HTuple GrayOpening = "�Ҷȿ�����";
	HTuple GrayClosing = "�Ҷȱ�����";
};
//ͼ����ǿ���в����ṹ��
struct RunParamsStruct_ImgProcessOneDetect
{
	HTuple hv_ProcessType;               //���飬ͼ����ǿ����
	HTuple hv_CoreWidth;			//���飬�˲��˿��
	HTuple hv_CoreHeight;			//���飬�˲��˸߶�
	HTuple hv_Sigma;				//���飬ƽ����ֵ
	HTuple hv_BilateralFilterThreshold; //���飬˫���˲�����ֵ(���ǻҶ�ֵ֮��С�ڴ�ֵ�����أ����ᱻƽ��)
	HTuple hv_Scale_Image_MultValue;	//���飬�Ҷ������������
	HTuple hv_Scale_Image_AddValue;	//���飬�Ҷ������������

	HObject ho_SearchRegion;			 //��������
};

//ͼ����ǿ���ͽṹ��
struct TypeStruct_ImgProcessTwoDetect
{
	HTuple Add_Img = "ͼ�����";
	HTuple Sub_Img = "ͼ�����";
	HTuple Mult_Img = "ͼ�����";
	HTuple Div_Img = "ͼ�����";
	HTuple Min_Img = "��ͼ�Ҷ���С";
	HTuple Max_Img = "��ͼ�Ҷ����";
};
//ͼ����ǿ���в����ṹ��
struct RunParamsStruct_ImgProcessTwoDetect
{
	HTuple hv_ProcessType = "null";       //ͼ��������

	HTuple hv_MultValue = 1;	    //��������
	HTuple hv_AddValue = 0;		   //��������
};

// ���ͽṹ��
struct TypeStruct_ImgRotateDetect
{
	HTuple hv_Mirror = "����";		//����
	HTuple hv_MirrorRow = "row";
	HTuple hv_MirrorColumn = "column";

	HTuple hv_Rotate = "��ת";		//��ת
};
// ���в����ṹ��
struct RunParamsStruct_ImgRotateDetect
{
	HTuple hv_Type = "��ת";
	HTuple hv_Angle = 0;			//��ת�Ƕȣ��ǽǶȣ����ǻ��ȣ�
	HTuple hv_MirrorDirection = "row";//������
};

//���ͽṹ��
struct TypeStruct_ImgSaveDetect
{
	string hv_SaveOk = "SaveOk";
	string hv_SaveNg = "SaveNg";
	string hv_SaveOkAndNg = "SaveOkAndNg";
	string hv_SaveNone = "SaveNone";
	string hv_FileName_Id_Time = "Id_Time";
};
// ���в����ṹ��
struct RunParamsStruct_ImgSaveDetect
{
	HTuple IsOk = 0;			//�Ƿ�OK,0Ϊok,��0ΪNG
	string hv_SavePath = "C:\\ImgSave";//��ͼ·��
	string hv_SaveType = "bmp";	//��ͼ����bmp, jpeg, png, tiff
	string hv_SaveMethod = "SaveOkAndNg";//��ͼ����(Ĭ��OK��NG������)
	string hv_FileName = "Id_Time";//�ļ�����
};

// ���в����ṹ��
struct RunParamsStruct_ImgShowDetect
{
	vector<HObject> vec_InputRegion;		//������������
	vector<HObject> vec_InputXld;		//����XLD����
	vector<HTuple> vec_Ret;		//���������飬1��OK����1��NG
	HTuple hv_Ret = 0;		//1��OK,��1��NG
	HTuple hv_LineWidth = 2;	//����������߿�
	HTuple hv_Word_X = 10;		//�������ֵ�X����
	HTuple hv_Word_Y = 10;		//�������ֵ�Y����
	HTuple hv_Word_Size = 10;	//���ֵ��ֺ�
};
// ��������ṹ��
struct ResultParamsStruct_ImgShowDetect
{
	HObject ho_DestImg;		//���ͼƬ
};

//�������ͽṹ��
struct TypeStruct_IntersectionDetect
{
	HTuple hv_LineLine = "ֱ��ֱ�߽���";
	HTuple hv_SegmentLine = "�߶�ֱ�߽���";
	HTuple hv_SegmentSegment = "�߶��߶ν���";
	HTuple hv_LineCircle = "ֱ��Բ����";
	HTuple hv_SegmentCircle = "�߶�Բ����";
	HTuple hv_CircleCircle = "ԲԲ����";
	HTuple hv_LineEllipse = "ֱ����Բ����";
	HTuple hv_SegmentEllipse = "�߶���Բ����";
	HTuple hv_NULL = "null";
};
//�������в����ṹ��
struct RunParamsStruct_IntersectionDetect
{
	HTuple hv_IntersectType = "null";
	ResultParamsStruct_Line LineA;
	ResultParamsStruct_Line LineB;
	ResultParamsStruct_Circle CircleA;
	ResultParamsStruct_Circle CircleB;
	ResultParamsStruct_Ellipse EllipseA;
};
//�������н���ṹ��
struct ResultParamsStruct_IntersectionDetect
{
	//��������
	HTuple hv_PointNum = 0;
	//��������
	HTuple hv_Row1_Result;
	HTuple hv_Column1_Result;
	HTuple hv_Row2_Result;
	HTuple hv_Column2_Result;

};

//���в����ṹ��
struct RunParamsStruct_Judgement
{
	vector<string> hv_GlobalName;
	string hv_AndOr = "and";
	vector<HTuple> hv_CurrValue;
	vector<double> hv_MinValue;
	vector<double> hv_MaxValue;
	vector<HTuple> hv_LabelStr;
};

//�������ͽṹ��
struct TypeStruct_MeasurementDetect
{
	HTuple hv_DistanceSegmentLine = "�߶�ֱ�߾���";
	HTuple hv_DistanceSegmentSegment = "�߶��߶ξ���";
	HTuple hv_DistanceSegmentCircle = "�߶�Բ����";
	HTuple hv_DistanceLineCircle = "ֱ��Բ����";
	HTuple hv_DistanceCircleCircle = "ԲԲ����";
	HTuple hv_DistancePointPoint = "������";
	HTuple hv_DistancePointLine = "��ֱ�߾���";
	HTuple hv_DistancePointSegment = "���߶ξ���";
	HTuple hv_AngleLineLine = "ֱ��ֱ�߽Ƕ�";
	HTuple hv_AngleLineHorizon = "ֱ��ˮƽ�߽Ƕ�";

	HTuple hv_DistanceContourContour = "������������";
	HTuple hv_DistanceLineContour = "ֱ����������";
	HTuple hv_DistanceSegmentContour = "�߶���������";
	HTuple hv_DistancePointContour = "����������";

	HTuple hv_DistanceRegionRegion = "�����������";
	HTuple hv_DistanceLineRegion = "ֱ���������";
	HTuple hv_DistanceSegmentRegion = "�߶��������";
	HTuple hv_DistancePointRegion = "���������";
	HTuple hv_NULL = "null";
};
//�������в����ṹ��
struct RunParamsStruct_MeasurementDetect
{
	HTuple hv_MeasureType = "null";			//��������
	HTuple hv_PixelMm = 1;					//���ص���

	ResultParamsStruct_Line LineA;			//����ֱ��1
	ResultParamsStruct_Line LineB;			//����ֱ��2
	ResultParamsStruct_Circle CircleA;		//����Բ1
	ResultParamsStruct_Circle CircleB;		//����Բ2
	HTuple hv_Row_PointA;					//�����1������
	HTuple hv_Col_PointA;					//�����1������
	HTuple hv_Row_PointB;					//�����1������
	HTuple hv_Col_PointB;				    //�����1������

	HObject hv_Contour1;					//��������1
	HObject hv_Contour2;					//��������2
	HObject hv_Region1;					    //��������1
	HObject hv_Region2;						//��������2
};
//�������н���ṹ��
struct ResultParamsStruct_MeasurementDetect
{
	HTuple hv_DistanceMin;
	HTuple hv_DistanceMax;
	HTuple hv_Angle;
	HObject ho_OutObj;
};

//������������ͽṹ��
struct TypeStruct_PhotoStereo
{
	HTuple GaussCurvature = "��˹����";
	HTuple MeanCurvature = "ƽ������";
	HTuple Curl = "��������";
	HTuple Divergence = "����ɢ��";
};
//���в����ṹ��
struct RunParamsStruct_PhotoStereo
{
	HTuple hv_PhotoStereoType = "��˹����";		//�������
	HTuple hv_Sigma = 1.0;			//ƽ����ֵ
	//��Դ�����������������߼н�
	HTuple hv_Slants1 = 45;
	HTuple hv_Slants2 = 45;
	HTuple hv_Slants3 = 45;
	HTuple hv_Slants4 = 45;
	//�������������Ʒ�н�(����ͼ�����ͼ���Ҳ��������ǶȾ���0�㣬�������������Ƕ���90�㣬�������������Ƕ���180�㣬�±ߴ������270��)
	HTuple hv_Tilts1 = 0;
	HTuple hv_Tilts2 = 90;
	HTuple hv_Tilts3 = 180;
	HTuple hv_Tilts4 = 270;
};

//253010 �Ӿ�ģ����£���֮ǰPIN��ͻ�������Ӹ�_New
//Pin�������Ͳ����ṹ��
struct TypeParamsStruct_Pin_New
{
	//Pin������Ͳ���
	HTuple DetectType_Blob = "Blob";	//blob
	HTuple DetectType_Template = "ģ��ƥ��";			//ģ��ƥ��
	HTuple BaseType_Line = "ֱ��";		//ֱ��
	HTuple BaseType_Circle = "Բ";	//Բ
	//pin������������
	HTuple SortType_Row = "��������";//��������
	HTuple SortType_Column = "��������";//��������
	//��ֵ�����Ͳ���
	HTuple Fixed_Threshold = "�̶���ֵ";
	HTuple Auto_Threshold = "����Ӧ��ֵ";
	HTuple Dyn_Threshold = "��̬��ֵ";
	HTuple DynPolarity_Light = "light";
	HTuple DynPolarity_Dark = "dark";
	HTuple DynPolarity_Equal = "equal";
	HTuple DynPolarity_NotEqual = "not_equal";

	//��̬ѧ��ʽ
	HTuple Opening_Rec = "���ο�����";
	HTuple Closing_Rec = "���α�����";
	HTuple Opening_Cir = "Բ�ο�����";
	HTuple Closing_Cir = "Բ�α�����";
	HTuple Erosion_Rec = "���θ�ʴ";
	HTuple Dilation_Rec = "��������";
	HTuple Erosion_Cir = "Բ�θ�ʴ";
	HTuple Dilation_Cir = "Բ������";

	//ģ��ƥ������
	HTuple ShapeModel = "������״";
	HTuple NccModel = "���ڻ����";

	HTuple hv_Null = "null";
};
//Pin�������в����ṹ��
struct RunParamsStruct_Pin_New
{
	//����CSV·��
	string FilePath_Csv = "D:/PinData/";
	int ID_Csv = 0;
	//���Ʋ���
	HTuple hv_Tolerance_X = 20;		//X���������ƫ�Χ
	HTuple hv_Tolerance_Y = 20;		//Y���������ƫ�Χ
	HTuple hv_MmPixel = 0.01;		//���ص���
	HTuple hv_StandardX_Array;	//X�����׼����(����)
	HTuple hv_StandardY_Array;	//Y�����׼����(����)

	//Pin����󶨵Ļ�׼���
	HTuple hv_X_BindingNum;	//����
	HTuple hv_Y_BindingNum;	//����

	//��׼����
	HTuple hv_BaseType;				//���飬��׼����(Ĭ����ֱ�ߣ�������ѡ��ṹ��RunParamsStruct_Pin�������)
	HTuple hv_Row1_Base;	//����(ֱ�����row������Բ��row)
	HTuple hv_Column1_Base;	//����(ֱ�����column������Բ��column)
	HTuple hv_Row2_Base;	//����(ֱ���յ�row)
	HTuple hv_Column2_Base;	//����(ֱ���յ�column)
	HTuple hv_Radius_Base;	//����(Բ�뾶)
	//�ұ���Բģ����Ҫ���㷨����
	HTuple hv_MeasureLength1;		//����,���߰볤,
	HTuple hv_MeasureLength2;		//����,���߰��
	HTuple hv_MeasureSigma;		//����,ƽ��ֵ 
	HTuple hv_MeasureThreshold;	//����,��Ե��ֵ
	HTuple hv_MeasureTransition;	//����,���߼���
	HTuple hv_MeasureSelect;		//����,��Եѡ��
	HTuple hv_MeasureNum;		    //����,���߸���
	HTuple hv_MeasureMinScore;     //����,��С�÷�
	HTuple hv_DistanceThreshold;   //����,������ֵ

	//��ֵ������
	HTuple hv_DetectType;			//���飬�������(Ĭ��"��ֵ��"������ѡ��ṹ��RunParamsStruct_Pin�������)
	HTuple hv_SortType;				//����,pin��������ʽ
	HObject ho_SearchRegion;		//����,��������
	HTuple hv_ThresholdType;		//����,ͼ���ֵ������
	HTuple hv_LowThreshold;				//����,����ֵ(�̶���ֵ)
	HTuple hv_HighThreshold;				//����,����ֵ(�̶���ֵ)
	HTuple hv_Sigma;						//����,ƽ����ֵ
	HTuple hv_CoreWidth;					//����,��̬��ֵ(�˲��˿��)
	HTuple hv_CoreHeight;					//����,��̬��ֵ(�˲��˸߶�)
	HTuple hv_DynThresholdContrast;		//����,��̬��ֵ�ָ�ͼ��Աȶ�
	HTuple hv_DynThresholdPolarity;	//����,��̬��ֵ����ѡ��(ѡ����ȡ���������������)��light,dark

	//ɸѡBlob����
	HTuple hv_BlobCount;//����,Blob����
	HTuple hv_AreaFilter_Min;	//����
	HTuple hv_RecLen1Filter_Min;//����
	HTuple hv_RecLen2Filter_Min;//����
	HTuple hv_RowFilter_Min;//����
	HTuple hv_ColumnFilter_Min;//����
	HTuple hv_CircularityFilter_Min;//����
	HTuple hv_RectangularityFilter_Min;//����
	HTuple hv_WidthFilter_Min;//����
	HTuple hv_HeightFilter_Min;//����
	HTuple hv_AreaFilter_Max;//����
	HTuple hv_RecLen1Filter_Max;//����
	HTuple hv_RecLen2Filter_Max;//����
	HTuple hv_RowFilter_Max;//����
	HTuple hv_ColumnFilter_Max;//����
	HTuple hv_CircularityFilter_Max;//����
	HTuple hv_RectangularityFilter_Max;//����
	HTuple hv_WidthFilter_Max;//����
	HTuple hv_HeightFilter_Max;//����
	HTuple hv_SelectAreaMax;	//ѡ��������������ֵ������
	//��̬ѧ����
	HTuple hv_OperatorType;//����
	HTuple hv_OperaRec_Width;//����
	HTuple hv_OperaRec_Height;//����
	HTuple hv_OperaCir_Radius;//����
	//�׶�������
	HTuple hv_FillUpShape_Min;//����
	HTuple hv_FillUpShape_Max;//����

	//ģ��ƥ�����
	HTuple hv_MatchMethod;      //ģ������:"������״"��"���ڻ����	
	HTuple hv_AngleStart;            //��ʼ�Ƕ�
	HTuple hv_AngleExtent;                //�Ƕȷ�Χ
	HTuple hv_ScaleRMin;                 //��С������
	HTuple hv_ScaleRMax;                 //���������
	HTuple hv_ScaleCMin;                 //��С������
	HTuple hv_ScaleCMax;				   //���������
	HTuple hv_MinScore;               //��ͷ���
	HTuple hv_NumMatches;               //ƥ�������0���Զ�ѡ��100�����ƥ��100��
	HObject ho_TrainRegion;				//ѵ������
	HTuple hv_ModelID;                     //ģ����

	//λ����������
	HObject ho_SearchRegion_PositionCorrection;	//��������
	HObject ho_TrainRegion_PositionCorrection;	//ѵ������
	HTuple hv_Row_PositionCorrection = 0;	//λ������ģ��row
	HTuple hv_Column_PositionCorrection = 0;//λ������ģ��column
	HTuple hv_Angle_PositionCorrection = 0;//λ������ģ��angle
	HTuple hv_MatchMethod_PositionCorrection = "������״";      //ģ������:"������״"��"���ڻ����	
	HTuple hv_AngleStart_PositionCorrection = -180;            //��ʼ�Ƕ�
	HTuple hv_AngleExtent_PositionCorrection = 180;                //��ֹ�Ƕ�
	HTuple hv_ScaleRMin_PositionCorrection = 1;                 //��С������
	HTuple hv_ScaleRMax_PositionCorrection = 1;                 //���������
	HTuple hv_ScaleCMin_PositionCorrection = 1;                 //��С������
	HTuple hv_ScaleCMax_PositionCorrection = 1;				   //���������
	HTuple hv_MinScore_PositionCorrection = 0.5;               //��ͷ���
	HTuple hv_NumMatches_PositionCorrection = 1;               //ƥ�������0���Զ�ѡ��100�����ƥ��100��
	HTuple hv_ModelID_PositionCorrection = NULL;                     //ģ����
	HTuple hv_Check_PositionCorrection = 0;	//�Ƿ�����λ������(0�����ã� 1����)
	HTuple hv_SortType_PositionCorrection;	//����ʽ
};
//Pin�������ṹ��
struct ResultParamsStruct_Pin_New
{
	HTuple hv_Ret = 1;	//1��OK,0��NG
	HTuple hv_RetNum = 0;		//Pin������
	HObject ho_Contour_Base;			     //��׼����
	HObject ho_ModelXLD;//λ��������ģ������
	//��׼�߽������
	HTuple hv_Row1_BaseLine_Ret;
	HTuple hv_Column1_BaseLine_Ret;
	HTuple hv_Row2_BaseLine_Ret;
	HTuple hv_Column2_BaseLine_Ret;
	//��׼Բ������
	HTuple hv_Row_BaseCircle_Ret;
	HTuple hv_Column_BaseCircle_Ret;
	HTuple hv_Radius_BaseCircle_Ret;

	HObject ho_Region_AllPin;				 //����pin������
	HObject ho_Region_OKPin;				//OK��pin������
	HObject ho_Region_NGPin;				//NG��pin������
	HTuple hv_Row_Array;					   //pin����������������
	HTuple hv_Column_Array;					   //pin����������������
	HTuple hv_DisX_Pixel_Array;                  //pin������������Ի�׼��X�������,����
	HTuple hv_DisY_Pixel_Array;				  //pin������������Ի�׼��Y�������,����
	HTuple hv_OffsetX_Pixel_Array;              //pin������������Ի�׼��X����ƫ��ֵ,����
	HTuple hv_OffsetY_Pixel_Array;			 //pin������������Ի�׼��Y����ƫ��ֵ,����
	HTuple hv_DisX_MM_Array;                  //pin������������Ի�׼��X�������,����
	HTuple hv_DisY_MM_Array;				  //pin������������Ի�׼��Y�������,����
	HTuple hv_OffsetX_MM_Array;              //pin������������Ի�׼��X����ƫ��ֵ,����
	HTuple hv_OffsetY_MM_Array;			 //pin������������Ի�׼��Y����ƫ��ֵ,����
};

//Pcb������Ͳ����ṹ��
struct TypeParamsStruct_Pcb
{
	//Pin������Ͳ���
	HTuple DetectType_Blob = "Blob";	//blob
	HTuple DetectType_Template = "ģ��ƥ��";			//ģ��ƥ��
	HTuple BaseType_Line = "ֱ��";		//ֱ��
	HTuple BaseType_Circle = "Բ";	//Բ
	//pin������������
	HTuple SortType_Row = "��������";//��������
	HTuple SortType_Column = "��������";//��������
	//��ֵ�����Ͳ���
	HTuple Fixed_Threshold = "�̶���ֵ";
	HTuple Auto_Threshold = "����Ӧ��ֵ";
	HTuple Dyn_Threshold = "��̬��ֵ";
	HTuple DynPolarity_Light = "light";
	HTuple DynPolarity_Dark = "dark";
	HTuple DynPolarity_Equal = "equal";
	HTuple DynPolarity_NotEqual = "not_equal";

	//��̬ѧ��ʽ
	HTuple Opening_Rec = "���ο�����";
	HTuple Closing_Rec = "���α�����";
	HTuple Opening_Cir = "Բ�ο�����";
	HTuple Closing_Cir = "Բ�α�����";
	HTuple Erosion_Rec = "���θ�ʴ";
	HTuple Dilation_Rec = "��������";
	HTuple Erosion_Cir = "Բ�θ�ʴ";
	HTuple Dilation_Cir = "Բ������";

	//ģ��ƥ������
	HTuple ShapeModel = "������״";
	HTuple NccModel = "���ڻ����";

	HTuple hv_Null = "null";
};
//Pcb������в����ṹ��
struct RunParamsStruct_Pcb
{
	//���Ʋ���
	HTuple hv_Tolerance_X = 20;		//X���������ƫ�Χ
	HTuple hv_Tolerance_Y = 20;		//Y���������ƫ�Χ
	HTuple hv_Tolerance_A = 360;	//�Ƕ������ƫ�Χ
	HTuple hv_Tolerance_Area = 999999999;	//��������ƫ�Χ
	HTuple hv_MmPixel = 0.01;		//���ص���
	HTuple hv_StandardX_Array;	//X�����׼����(����)
	HTuple hv_StandardY_Array;	//Y�����׼����(����)
	HTuple hv_StandardA_Array;	//�ǶȻ�׼����(����)
	HTuple hv_StandardArea_Array;	//�����׼����(����)

	//Pin����󶨵Ļ�׼���
	HTuple hv_X_BindingNum;	//����
	HTuple hv_Y_BindingNum;	//����

	//��׼����
	HTuple hv_BaseType;				//���飬��׼����(Ĭ����ֱ�ߣ�������ѡ��ṹ��RunParamsStruct_Pin�������)
	HTuple hv_Row1_Base;	//����(ֱ�����row������Բ��row)
	HTuple hv_Column1_Base;	//����(ֱ�����column������Բ��column)
	HTuple hv_Row2_Base;	//����(ֱ���յ�row)
	HTuple hv_Column2_Base;	//����(ֱ���յ�column)
	HTuple hv_Radius_Base;	//����(Բ�뾶)
	//�ұ���Բģ����Ҫ���㷨����
	HTuple hv_MeasureLength1;		//����,���߰볤,
	HTuple hv_MeasureLength2;		//����,���߰��
	HTuple hv_MeasureSigma;		//����,ƽ��ֵ 
	HTuple hv_MeasureThreshold;	//����,��Ե��ֵ
	HTuple hv_MeasureTransition;	//����,���߼���
	HTuple hv_MeasureSelect;		//����,��Եѡ��
	HTuple hv_MeasureNum;		    //����,���߸���
	HTuple hv_MeasureMinScore;     //����,��С�÷�
	HTuple hv_DistanceThreshold;   //����,������ֵ

	//��ֵ������
	HTuple hv_DetectType;			//���飬�������(Ĭ��"��ֵ��"������ѡ��ṹ��RunParamsStruct_Pin�������)
	HTuple hv_SortType;				//����,pin��������ʽ
	HObject ho_SearchRegion;		//����,��������
	HTuple hv_ThresholdType;		//����,ͼ���ֵ������
	HTuple hv_LowThreshold;				//����,����ֵ(�̶���ֵ)
	HTuple hv_HighThreshold;				//����,����ֵ(�̶���ֵ)
	HTuple hv_Sigma;						//����,ƽ����ֵ
	HTuple hv_CoreWidth;					//����,��̬��ֵ(�˲��˿��)
	HTuple hv_CoreHeight;					//����,��̬��ֵ(�˲��˸߶�)
	HTuple hv_DynThresholdContrast;		//����,��̬��ֵ�ָ�ͼ��Աȶ�
	HTuple hv_DynThresholdPolarity;	//����,��̬��ֵ����ѡ��(ѡ����ȡ���������������)��light,dark

	//ɸѡBlob����
	HTuple hv_BlobCount;//����,Blob����
	HTuple hv_AreaFilter_Min;	//����
	HTuple hv_RecLen1Filter_Min;//����
	HTuple hv_RecLen2Filter_Min;//����
	HTuple hv_RowFilter_Min;//����
	HTuple hv_ColumnFilter_Min;//����
	HTuple hv_CircularityFilter_Min;//����
	HTuple hv_RectangularityFilter_Min;//����
	HTuple hv_WidthFilter_Min;//����
	HTuple hv_HeightFilter_Min;//����
	HTuple hv_AreaFilter_Max;//����
	HTuple hv_RecLen1Filter_Max;//����
	HTuple hv_RecLen2Filter_Max;//����
	HTuple hv_RowFilter_Max;//����
	HTuple hv_ColumnFilter_Max;//����
	HTuple hv_CircularityFilter_Max;//����
	HTuple hv_RectangularityFilter_Max;//����
	HTuple hv_WidthFilter_Max;//����
	HTuple hv_HeightFilter_Max;//����
	HTuple hv_SelectAreaMax;	//ѡ��������������ֵ������
	//��̬ѧ����
	HTuple hv_OperatorType;//����
	HTuple hv_OperaRec_Width;//����
	HTuple hv_OperaRec_Height;//����
	HTuple hv_OperaCir_Radius;//����
	//�׶�������
	HTuple hv_FillUpShape_Min;//����
	HTuple hv_FillUpShape_Max;//����

	//ģ��ƥ�����
	HTuple hv_MatchMethod;      //ģ������:"������״"��"���ڻ����	
	HTuple hv_AngleStart;            //��ʼ�Ƕ�
	HTuple hv_AngleExtent;                //�Ƕȷ�Χ
	HTuple hv_ScaleRMin;                 //��С������
	HTuple hv_ScaleRMax;                 //���������
	HTuple hv_ScaleCMin;                 //��С������
	HTuple hv_ScaleCMax;				   //���������
	HTuple hv_MinScore;               //��ͷ���
	HTuple hv_NumMatches;               //ƥ�������0���Զ�ѡ��100�����ƥ��100��
	HObject ho_TrainRegion;				//ѵ������
	HTuple hv_ModelID;                     //ģ����

	//λ����������
	HObject ho_SearchRegion_PositionCorrection;	//��������
	HObject ho_TrainRegion_PositionCorrection;	//ѵ������
	HTuple hv_Row_PositionCorrection = 0;	//λ������ģ��row
	HTuple hv_Column_PositionCorrection = 0;//λ������ģ��column
	HTuple hv_Angle_PositionCorrection = 0;//λ������ģ��angle
	HTuple hv_MatchMethod_PositionCorrection = "������״";      //ģ������:"������״"��"���ڻ����	
	HTuple hv_AngleStart_PositionCorrection = -180;            //��ʼ�Ƕ�
	HTuple hv_AngleExtent_PositionCorrection = 180;                //��ֹ�Ƕ�
	HTuple hv_ScaleRMin_PositionCorrection = 1;                 //��С������
	HTuple hv_ScaleRMax_PositionCorrection = 1;                 //���������
	HTuple hv_ScaleCMin_PositionCorrection = 1;                 //��С������
	HTuple hv_ScaleCMax_PositionCorrection = 1;				   //���������
	HTuple hv_MinScore_PositionCorrection = 0.5;               //��ͷ���
	HTuple hv_NumMatches_PositionCorrection = 1;               //ƥ�������0���Զ�ѡ��100�����ƥ��100��
	HTuple hv_ModelID_PositionCorrection = NULL;                     //ģ����
	HTuple hv_Check_PositionCorrection = 0;	//�Ƿ�����λ������(0�����ã� 1����)
	HTuple hv_SortType_PositionCorrection;	//����ʽ
};
//Pcb������ṹ��
struct ResultParamsStruct_Pcb
{
	HTuple hv_Ret = 1;	//1��OK,0��NG
	HTuple hv_RetNum = 0;		//Pin������
	HObject ho_Contour_Base;			     //��׼����
	HObject ho_ModelXLD;//λ��������ģ������
	//��׼�߽������
	HTuple hv_Row1_BaseLine_Ret;
	HTuple hv_Column1_BaseLine_Ret;
	HTuple hv_Row2_BaseLine_Ret;
	HTuple hv_Column2_BaseLine_Ret;
	//��׼Բ������
	HTuple hv_Row_BaseCircle_Ret;
	HTuple hv_Column_BaseCircle_Ret;
	HTuple hv_Radius_BaseCircle_Ret;

	HObject ho_Region_AllPcb;				 //����pin������
	HObject ho_Region_OKPcb;				//OK��pin������
	HObject ho_Region_NGPcb;				//NG��pin������
	HTuple hv_Row_Array;					   //pin����������������
	HTuple hv_Column_Array;					   //pin����������������
	HTuple hv_Angle_Array;					   //pin������Ƕ�
	HTuple hv_Area_Array;					   //pin���������
	HTuple hv_DisX_Pixel_Array;                  //pin������������Ի�׼��X�������,����
	HTuple hv_DisY_Pixel_Array;				  //pin������������Ի�׼��Y�������,����
	HTuple hv_OffsetX_Pixel_Array;              //pin������������Ի�׼��X����ƫ��ֵ,����
	HTuple hv_OffsetY_Pixel_Array;			 //pin������������Ի�׼��Y����ƫ��ֵ,����
	HTuple hv_DisX_MM_Array;                  //pin������������Ի�׼��X�������,����
	HTuple hv_DisY_MM_Array;				  //pin������������Ի�׼��Y�������,����
	HTuple hv_OffsetX_MM_Array;              //pin������������Ի�׼��X����ƫ��ֵ,����
	HTuple hv_OffsetY_MM_Array;			 //pin������������Ի�׼��Y����ƫ��ֵ,����
	HTuple hv_OffsetA_Array;//���飬�Ƕ�ƫ��
	HTuple hv_OffsetArea_Array;//���飬���ƫ��
};


//������任���в����ṹ��
struct RunParamsStruct_PolarTransDetect
{
	//����
	HTuple hv_InnerRadius = 50;		//Բ����Բ�뾶
	HTuple hv_OuterRadius = 150;	//Բ����Բ�뾶
	HTuple hv_Row = 100;			//Բ��������
	HTuple hv_Column = 100;			//Բ��������

	//ԭʼͼ��Ŀ��
	HTuple hv_OriginalWidth;
	HTuple hv_OriginalHeight;

};

//���в����ṹ��
struct RunParamsStruct_PrintDetection
{
	//��������
	HObject ho_SearchRegion;
	//ѵ������
	HObject ho_TrainRegion;
	//����ģ��ģ�;��
	HTuple hv_Deformable_ID;
	//����ģ��ģ�;��
	HTuple hv_Variation_ID;

	string toolName;			//ֱ������
	string jobName;                      //��������

};
//��������ṹ��
struct ResultParamsStruct_PrintDetection
{
	//����ģ��ƥ������
	HObject ho_Deformed_Contours;
	//ȱ������
	HObject ho_DirtyRegion;
	//ȱ�����
	HTuple hv_DirtyArea;
	//���ν������ͼ��
	HObject ho_Rectified_Img;
	//OK����
	HTuple hv_RetNum = 0;

};

//������任���в����ṹ��
struct RunParamsStruct_ProjectionTrans
{
	//����
	HTuple hv_Row_LeftTop;		//ͶӰ�任�ı������Ͻ�������			
	HTuple hv_Column_LeftTop;		//ͶӰ�任���Ͻ�������		
	HTuple hv_Row_RightTop;		//ͶӰ�任�ı������Ͻ�������			
	HTuple hv_Column_RightTop;	//ͶӰ�任���Ͻ�������	
	HTuple hv_Row_RightBottom;	//ͶӰ�任�ı������½�������			
	HTuple hv_Column_RightBottom;	//ͶӰ�任���½�������	
	HTuple hv_Row_LeftBottom;		//ͶӰ�任�ı������½�������			
	HTuple hv_Column_LeftBottom;	//ͶӰ�任���½�������	

	string toolName;			//��������
	string jobName;                      //��������
};

//��ά�����ͽṹ��
struct CodeTypeStruct_Code2d
{
	HTuple DM = "Data Matrix ECC 200";
	HTuple QR = "QR Code";
	HTuple Aztec = "Aztec Code";
	HTuple GS1Aztec = "GS1 Aztec Code";
	HTuple GS1DM = "GS1 DataMatrix";
	HTuple GS1QR = "GS1 QR Code";
	HTuple MicroQR = "Micro QR Code";
	HTuple PDF417 = "PDF417";
};
//��ά�������в����ṹ��
struct RunParamsStruct_Code2d
{
	HObject ho_SearchRegion;					//��������
	HTuple hv_CodeType_Run = "QR Code";			//��������(Ĭ��"Data Matrix ECC 200")
	HTuple hv_CodeNum = 1;					//ʶ������(����Ϊ0����ɨ����������)
	HTuple hv_Tolerance = "high";               //ʶ���ݲ�(Ĭ�ϸ߼���Ĭ��high������������"low")
	HTuple hv_HeadChar = "null";					//�ض��ַ���ͷ
	HTuple hv_EndChar = "null";						//�ض��ַ���β
	HTuple hv_ContainChar = "null";					//�����ض��ַ�
	HTuple hv_NotContainChar = "null";				//�������ض��ַ�
	HTuple hv_CodeLength_Run = 2;			    //������С����
	HTuple hv_TimeOut = 50;					//��ʱʱ��(��λms)
	HTuple hv_HomMat2D = NULL;            //����任�����������
	bool isFollow = false;                //�Ƿ����
};
//��ά��ʶ�����ṹ��
struct ResultParamsStruct_Code2d
{
	//����������
	HTuple hv_CodeType_Result;         //��ά������
	HTuple hv_CodeContent;			  //��ά������
	HTuple hv_CodeLength_Result;	  //��ά�볤��
	HObject ho_ContourCode2d;		  //��ά������
	HObject ho_RegionCode2d;		  //��ά������
	HTuple hv_Row;                    //��ά������������
	HTuple hv_Column;                 //��ά������������
	HTuple hv_RetNum = 0;			 //�ҵ��Ķ�ά����(Ĭ����0��ʶ�𵽶�ά������0)
	HTuple hv_QualityGrade;		    //���������ȼ�0-4(iso15416)
	~ResultParamsStruct_Code2d()
	{
		ho_ContourCode2d.Clear();
		ho_RegionCode2d.Clear();
	}

};

//���½ṹ��� RobotCalibDetect ��Ľṹ���ͻ����������� 20250507
////�ŵ�궨���ͽṹ��
//struct RunParamsStruct_RobotCalib
//{
//	//�����˱궨����
//	HTuple hv_NPointCamera_X;	  //�ŵ�궨��������X
//	HTuple hv_NPointCamera_Y;    //�ŵ�궨��������Y
//	HTuple hv_NPointRobot_X;	 //�ŵ�궨��������X
//	HTuple hv_NPointRobot_Y;	 //�ŵ�궨��������Y
//	HTuple hv_HomMat2D = NULL;	 //�ŵ�궨����
//	HTuple hv_CenterCamera_X;	 //��ת���ı궨��������X
//	HTuple hv_CenterCamera_Y;    //��ת���ı궨��������Y
//	HTuple hv_CenterRobot_X;	//��ת���ı궨��������X
//	HTuple hv_CenterRobot_Y;    //��ת���ı궨��������Y
//	HTuple hv_CenterRobot_A;    //��ת���ı궨��������A
//	HTuple hv_Center_X;			//����������ת����X
//	HTuple hv_Center_Y;			//����������ת����Y
//	HTuple hv_Center_Radius;	//����������ת�뾶
//	HObject ho_CenterCircle;	//��ת���ĵ�Բ
//
//
//	//�Ƿ����ƫ�������ǻ����˾�������
//	bool hv_IsAbsCoordinate = false;
//	//�Ź�����������֮�����(��λmm)
//	HTuple hv_DistancePP_Robot = 10.0;
//	//��ת���ı궨ʱ��ÿ����ת�ĽǶ�
//	HTuple hv_AngleRotate_Robot = 10;
//	//��ת���ı궨ʱ�������˾�������
//	HTuple hv_CenterPosition_RobotX = 0.000;
//	HTuple hv_CenterPosition_RobotY = 0.000;
//	HTuple hv_CenterPosition_RobotA = 0.000;
//	//��ģλ�����˾�������
//	HTuple hv_ModelPosition_RobotX = 0.000;
//	HTuple hv_ModelPosition_RobotY = 0.000;
//	HTuple hv_ModelPosition_RobotA = 0.000;
//	//��ģλ��������Mark������
//	HTuple hv_ModelX = 0.000;	//ģ��λ��X
//	HTuple hv_ModelY = 0.000;	//ģ��λ��Y
//	HTuple hv_ModelA = 0.000;	//ģ��λ��A
//	//����ƫ��������Mark�㵱ǰ����
//	HTuple hv_NowX = 0.000;		//��ǰλ��X
//	HTuple hv_NowY = 0.000;		//��ǰλ��Y
//	HTuple hv_NowA = 0.000;		//��ǰλ��A
//	bool hv_IsCamFirst = true;  //���ĺ�ץ������ץ����
//	//�����Ƕ��Ƿ�ȡ��(��ͼ���нǶ�������������˵���ת�Ƕ��������෴ʱ����Ҫ�Ѳ����Ƕ�ȡ��),true��ȡ����false�ǲ�ȡ��
//	bool hv_IsReverseAngle = false;
//	//�Ƿ�������ת���� true������, false������
//	bool hv_IsUseCenterCalib = true;
//
//	//������������������
//	HTuple hv_AbsInputX = 0.000;	//��������ؾ�������X
//	HTuple hv_AbsInputY = 0.000;	//��������ؾ�������Y
//	HTuple hv_AbsInputA = 0.000;	//��������ؾ�������A
//
//};
////����ƫ�������߾����������ṹ��
//struct ResultParamsStruct_RobotCalib
//{
//	//ƫ�����������
//	HTuple hv_OffsetX = -1;	//ƫ����X
//	HTuple hv_OffsetY = -1;	//ƫ����Y
//	HTuple hv_OffsetA = -1;	//ƫ����A
//
//	//��������������
//	HTuple hv_AbsOutputX = -1;	//�����˾�������X
//	HTuple hv_AbsOutputY = -1;	//�����˾�������Y
//	HTuple hv_AbsOutputA = -1;	//�����˾�������A
//
//};

//����任���ͽṹ��
struct TypeStruct_ShapeTransDetect
{
	HTuple hv_Null = "null";
	HTuple hv_Convex = "convex"; //"͹����";
	HTuple hv_Ellipse = "ellipse"; //"��С�����Բ"
	HTuple hv_Inner_Circle = "inner_circle"; // "����ڽ�Բ"
	HTuple hv_Outer_Circle = "outer_circle"; // "��С���Բ"
	HTuple hv_Inner_Rectangle1 = "inner_rectangle1"; // "����ڽӾ���"
	HTuple hv_Rectangle1 = "rectangle1"; // "��С��Ӿ���"
	HTuple hv_Rectangle2 = "rectangle2"; // "��С�����ת����"
};
// ���в����ṹ��
struct RunParamsStruct_ShapeTransDetect
{
	HTuple hv_TypeTrans = "convex";		//ת������
};
//��������ṹ��
struct ResultParamsStruct_ShapeTransDetect
{
	HObject hv_DestRegion;					 //�������

	HTuple hv_Row1_Rectangle1;			      //���ε����Ͻǵ�������
	HTuple hv_Column1_Rectangle1;			  //���ε����Ͻǵ�������
	HTuple hv_Row2_Rectangle1;			      //���ε����½ǵ�������
	HTuple hv_Column2_Rectangle1;			  //���ε����½ǵ�������

	HTuple hv_Row_Circle;					 //Բ�ε����ĵ�������
	HTuple hv_Column_Circle;				 //Բ�ε����ĵ�������
	HTuple hv_Radius_Circle;				 //Բ�εİ뾶

	HTuple hv_Row_Rectangle2;			      //��ת���ε����ĵ�������
	HTuple hv_Column_Rectangle2;			  //��ת���ε����ĵ�������
	HTuple hv_Length1_Rectangle2;			  //��ת���εİ볤
	HTuple hv_Length2_Rectangle2;			  //��ת���εİ��
	HTuple hv_Phi_Rectangle2;			      //��ת���εĽǶ�

	HTuple hv_Row_Ellipse;					 //��Բ�����ĵ�������
	HTuple hv_Column_Ellipse;				 //��Բ�����ĵ�������
	HTuple hv_Radius1_Ellipse;				 //��Բ�ĳ�����
	HTuple hv_Radius2_Ellipse;				 //��Բ�Ķ̰���
	HTuple hv_Phi_Ellipse;					 //��Բ�ĽǶ�

	HTuple hv_Row_Convex;					 //͹�������ĵ�������
	HTuple hv_Column_Convex;				 //͹�������ĵ�������
};

//ģ��ƥ�����ͽṹ��
struct ModelTypeStruct_Template
{
	//ģ������:"none"������,"scaled"ͬ������,"aniso"�첽����,"ncc"���ڻ����
	HTuple None = "none";
	HTuple Scaled = "scaled";
	HTuple Aniso = "aniso";
	HTuple Ncc = "ncc";
	//ģ���������
	HTuple Auto = "auto";
};
// ģ��ƥ������ṹ��
struct RunParamsStruct_Template
{
	HTuple hv_MatchMethod = "scaled";      //ģ������
	HTuple hv_NumLevels = "auto";              //����������
	HTuple hv_AngleStart = -30;                //��ʼ�Ƕ�
	HTuple hv_AngleExtent = 30;                //�Ƕȷ�Χ
	HTuple hv_AngleStep = "auto";              //�ǶȲ���
	HTuple hv_ScaleRMin = 0.9;                 //��С������(ScaleModelģʽ��ʱ��hv_ScaleRMin_Train����X��Y������С����ϵ��)
	HTuple hv_ScaleRMax = 1.1;                 //���������(ScaleModelģʽ��ʱ��hv_ScaleRMax_Train����X��Y�����������ϵ��)
	HTuple hv_ScaleRStep = "auto";             //�з������Ų���
	HTuple hv_ScaleCMin = 0.9;                 //��С������
	HTuple hv_ScaleCMax = 1.1;				   //���������
	HTuple hv_ScaleCStep = "auto";             //�з������Ų���
	HTuple hv_Optimization = "auto";           //�Ż��㷨
	HTuple hv_Metric = "use_polarity";         //����/����
	HTuple hv_Contrast = "auto";               //�Աȶ�
	HTuple hv_MinContrast = "auto";            //��С�Աȶ�

	HTuple hv_Angle_Original = 0;			 //����ģ��ʱ��ԭʼģ��Ƕ�
	HTuple hv_CenterRow_Original = 0;		//����ģ��ʱ��ԭʼģ��Y
	HTuple hv_CenterCol_Original = 0;		//����ģ��ʱ��ԭʼģ��X

	HTuple hv_MinScore = 0.5;               //��ͷ���
	HTuple hv_NumMatches = 1;               //ƥ�������0���Զ�ѡ��100�����ƥ��100��
	HTuple hv_MaxOverlap = 0.5;             //Ҫ�ҵ�ģ��ʵ��������ص�
	HTuple hv_SubPixel = "least_squares";   //�����ؾ���
	HTuple hv_Greediness = 0.9;             //̰��ϵ��

	HTuple hv_ModelID;                     //ģ����
	HObject ho_TrainRegion;				   //ģ��ѵ������
	HObject ho_CropModelImg;			   //ģ��ѵ��Сͼ
	HObject ho_SearchRegion;			  //��������
	HTuple hv_TimeOut = 1000;			//��ʱʱ��(��λms)
};

// ����ģ�����ṹ��
struct ResultParamsStruct_Template
{
	//��ģʱ������ͽǶ�
	HTuple hv_Angle_Original = 0;			 //����ģ��ʱ��ԭʼģ��Ƕ�
	HTuple hv_CenterRow_Original = 0;		//����ģ��ʱ��ԭʼģ��Y
	HTuple hv_CenterCol_Original = 0;		//����ģ��ʱ��ԭʼģ��X
	//����ʱ�ҵ���ģ����
	HTuple hv_Row;                    //���ĵ㣨X��
	HTuple hv_Column;                 //���ĵ㣨Y��
	HTuple hv_Angle;                  //�Ƕ�
	HTuple hv_ScaleR;                 //������(ScaleModelģʽ��ʱ��hv_ScaleR����X��Y��������ϵ��)
	HTuple hv_ScaleC;                 //������
	HTuple hv_Score;                  //ƥ�����
	HTuple hv_RetNum = 0;             //�ҵ���ģ�����(Ĭ��0���ҵ�ģ�������0)

	HObject ho_Contour_Template;      // ģ������
	HObject ho_Region_Template;      // ģ������
	//����任����(����ץ�߸���)
	HTuple hv_ROIHomMat2D;

	void clear()
	{
		ho_Contour_Template.Clear();
		ho_Region_Template.Clear();
	};

	~ResultParamsStruct_Template()
	{
		clear();
	}

};



//����㷨���ͽṹ��
struct ModelTypeStruct_ImgDifference
{
	//ģ������:"none"������,"scaled"ͬ������,"aniso"�첽����,"ncc"���ڻ����
	HTuple None = "none";
	HTuple Scaled = "scaled";
	HTuple Aniso = "aniso";
	HTuple Ncc = "ncc";
	//ģ���������
	HTuple Auto = "auto";
};
// ����㷨�����ṹ��
struct RunParamsStruct_ImgDifference
{
	//ģ��ƥ�����
	HTuple hv_IsUsSearchRoi = 0;//0 ��������������1������������
	HTuple hv_MatchMethod = "scaled";      //ģ������
	HTuple hv_NumLevels = "auto";              //����������
	HTuple hv_AngleStart = -90;                //��ʼ�Ƕ�
	HTuple hv_AngleExtent = 90;                //�Ƕȷ�Χ
	HTuple hv_AngleStep = "auto";              //�ǶȲ���
	HTuple hv_ScaleRMin = 0.9;                 //��С������(ScaleModelģʽ��ʱ��hv_ScaleRMin_Train����X��Y������С����ϵ��)
	HTuple hv_ScaleRMax = 1.1;                 //���������(ScaleModelģʽ��ʱ��hv_ScaleRMax_Train����X��Y�����������ϵ��)
	HTuple hv_ScaleRStep = "auto";             //�з������Ų���
	HTuple hv_ScaleCMin = 0.9;                 //��С������
	HTuple hv_ScaleCMax = 1.1;				   //���������
	HTuple hv_ScaleCStep = "auto";             //�з������Ų���
	HTuple hv_Optimization = "auto";           //�Ż��㷨
	HTuple hv_Metric = "use_polarity";         //����/����
	HTuple hv_Contrast = "auto";               //�Աȶ�
	HTuple hv_MinContrast = "auto";            //��С�Աȶ�

	HTuple hv_Angle_Original = 0;			 //����ģ��ʱ��ԭʼģ��Ƕ�
	HTuple hv_CenterRow_Original = 0;		//����ģ��ʱ��ԭʼģ��Y
	HTuple hv_CenterCol_Original = 0;		//����ģ��ʱ��ԭʼģ��X

	HTuple hv_MinScore = 0.5;               //��ͷ���
	HTuple hv_NumMatches = 1;               //ƥ�������0���Զ�ѡ��100�����ƥ��100��
	HTuple hv_MaxOverlap = 0.5;             //Ҫ�ҵ�ģ��ʵ��������ص�
	HTuple hv_SubPixel = "none";   //�����ؾ���
	HTuple hv_Greediness = 0.5;             //̰��ϵ��

	HTuple hv_ModelID = NULL;                     //ģ����
	HObject ho_TrainRegion;				   //ģ��ѵ������
	HObject ho_CropModelImg;			   //ģ��ѵ��Сͼ
	HObject ho_ModelImg;			       //ģ��ѵ����ͼ
	HObject ho_SearchRegion;			  //��������
	HTuple hv_TimeOut = 1000;			//��ʱʱ��(��λms)
	//ȱ�ݹ��˲���
	HTuple hv_IsUseGauss = 0;//�Ƿ����ø�˹�˲���1 ʹ�ã�0 ��ʹ��
	HTuple hv_Sigma = 3;
	HTuple hv_Mult = 1;
	HTuple hv_Add = 0;
	HTuple hv_FilterGrayMin = 0;
	HTuple hv_FilterGrayMax = 128;
	HTuple hv_FilterAreaMin = 100;
	HTuple hv_FilterAreaMax = 999999999;
	HTuple hv_FilterWidthMin = 1;
	HTuple hv_FilterWidthMax = 999999999;
	HTuple hv_FilterHeightMin = 1;
	HTuple hv_FilterHeightMax = 999999999;
};

// ����㷨����ṹ��
struct ResultParamsStruct_ImgDifference
{
	//����ʱ�ҵ���ģ����
	HTuple hv_Row;                    //���ĵ㣨X��
	HTuple hv_Column;                 //���ĵ㣨Y��
	HTuple hv_Angle;                  //�Ƕ�
	HTuple hv_Score;                  //ƥ�����
	HTuple hv_RetNum = 0;             //�ҵ���ģ�����(Ĭ��0���ҵ�ģ�������0)

	HObject ho_Contour_Template;      // ģ������
	HObject ho_Region_Template;      // ģ������
	HObject ho_ErrorRegion;	//ȱ������
	HTuple hv_ErrorNum = 0; //ȱ������
};


//���в����ṹ��
struct RunParamsStruct_CharacterCheckDetect
{
	HTuple hv_InputValue1;//����ֵ1
	HTuple hv_InputValue2;//����ֵ2
};
//���в����ṹ��
struct ResultParamsStruct_CharacterCheckDetect
{
	HTuple hv_Ret;//����ֵ1
};

//�ַ�������в����ṹ��
struct RunParamsStruct_CharacterDetect
{
	//��������
	HObject ho_SearchRoi;
	HObject ho_ModelImg;
	vector<HObject> ModelCutImg;
	vector<string> ModelCutImgPath;
	vector<HObject> TrainRegions;
	vector<HObject> VariationModelROIs;
	vector<HTuple> Ori_RowRefs;
	vector<HTuple> Ori_ColRefs;
	vector<HTuple> ShapeModel_ID;
	vector<HTuple> Ori_Widths_Rec;
	vector<HTuple> Ori_Heights_Rec;
	vector<HTuple> Ori_Row1s_Rec;
	vector<HTuple> Ori_Col1s_Rec;
	vector<HTuple> Ori_Row2s_Rec;
	vector<HTuple> Ori_Col2s_Rec;
	vector<HTuple> VarModel_ID;
	HObject ModelRegions;

	//��״ģ�����
	int hv_NumLevels = 1;
	double hv_AngleStart = -10;
	double hv_AngleExtent = 20;
	double hv_ScaleRMin = 1;
	double hv_ScaleRMax = 1;
	double hv_ScaleCMin = 1;
	double hv_ScaleCMax = 1;
	string hv_Metric = "use_polarity";
	int hv_Contrast = 30;
	int hv_ContrastMin = 10;
	int hv_SizeMin = 10;
	double hv_ScoreMin = 0.5;
	int hv_NumMatches = 1;
	double hv_MaxOverlap = 0.9;
	string hv_SubPixel = "least_squares";
	double hv_Greediness = 0.5;
	//����ģ�Ͳ���
	string hv_ErrorType = "light";//"light","dark","light_dark"
	int hv_LightDirtContrast = 30;
	double hv_LightVarRadio = 2;
	int hv_DarkDirtContrast = 30;
	double hv_DarkVarRadio = 2;
	double hv_MinArea = 10;
	double hv_MaxArea = 10000000;
	double hv_EdgeTolerance = 0;
	string OkImgFilePath = "";
	//ģ������
	int hv_ModelNum = 0;
	//ͼ�������
	string hv_FilterType = "gauss_filter";//���Ͱ���"gauss_filter"��"bilateral_filter","mean_filter","median_filter"
	double hv_Sigma = 3;
	int hv_BilateralContrast = 20;
	int hv_MaskWidth = 3;
	int MaskHeight = 3;
	int hv_IsUseFilter = 0;
	//�Ҷ�ֵ��һ������
	int hv_IsUseNormalize = 0;
	vector<HTuple> hv_ForwardGrayValue;
	vector<HTuple> hv_BackGroundGrayValue;
};
//�ַ�����������ṹ��
struct ResultParamsStruct_CharacterDetect
{
	HTuple b_IsOk = 1;//���(1 ��OK,0 ��NG)
	HObject ShapeModelContours;	//ģ������
	HObject ErrorRegions;		//NG����
	HTuple ErrorNum;					//NG����
	HTuple hv_Area;
	HTuple hv_Row;
	HTuple hv_Column;
};

//�Ҷ�ֵ������в����ṹ��
struct RunParamsStruct_GrayValueDetect
{
	HTuple hv_HomMat2D = NULL;            //����任����ROI����
	bool isFollow = false;                //�Ƿ����
	//�������򣬿���������
	HObject ho_SearchRoi;
	//OK�Ҷ���ֵ
	HTuple hv_MeanGrayValue_Standard;
	//�Ҷ���ֵ������
	HTuple hv_GrayValueOffset_Min;
	HTuple hv_GrayValueOffset_Max;
};
//�Ҷ�ֵ����������ṹ��
struct ResultParamsStruct_GrayValueDetect
{
	//�������򣬿���������
	HObject ho_AllRegion;
	HObject ho_OKRegion;
	HObject ho_NGRegion;
	//����ĻҶȾ�ֵ������������
	HTuple hv_MeanGrayValue_Ret;
	//�����0��ok����0��ng
	HTuple hv_Ret;
	HTuple hv_RetArray;//������飬����������һһ��Ӧ
};

//ͼ�������в����ṹ��
struct RunParamsStruct_ImgSplitDetect
{
	//�趨����
	HTuple hv_CutRows_Ori = 5;//ԭʼ��
	HTuple hv_CutCols_Ori = 5;//ԭʼ��
	HTuple hv_OverlapRate = 0.2;//�ص���
	//�������
	HTuple hv_CutRows_Dest = 5;//�����
	HTuple hv_CutCols_Dest = 5;//�����
	HTuple hv_WidthImg = 4000;//��ͼ��ͼ���
	HTuple hv_HeightImg = 3000;//��ͼ��ͼ���
	HTuple hv_CutWidth = 100;//Сͼ��
	HTuple hv_CutHeight = 100;//Сͼ��
	//�������
	HObject ho_Image;//�����ͼ
	HObject Vec_Image;//��ƴ��ͼ������
	//�жϲ���
	HTuple hv_IsSplit = 0;//0��ͼ���֣�1��ͼ��ƴ��
	//��ͼ����
	HTuple hv_CutNum = 0;

};
//ͼ���ֽ�������ṹ��
struct ResultParamsStruct_ImgSplitDetect
{
	//ƴͼ�������
	HObject ho_Img_Dest;//ƴ�ӺõĴ�ͼ
	//��ֽ������
	HObject Vec_Img_Dest;//��ֺõ�ͼ������
	//���Сͼ��������
	HTuple hv_CutRow1_Array = 0;
	HTuple hv_CutColumn1_Array = 0;
	HTuple hv_CutRow2_Array = 0;
	HTuple hv_CutColumn2_Array = 0;
};

//���в����ṹ��
struct RunParamsStruct_JointCalibrationDetect
{
	//�궨�������
	HTuple hv_StartCamPar;//�궨��Ҫ�������
	HTuple hv_CaltabDescrPath;//�궨�������ļ�·��(Halcon��ʽ�궨�壬��".cpd"��ʽ��)
	HTuple hv_CaltabThickness = 0.001;//�궨����(��λm,��)
	//�궨���
	HObject ho_CalibContours;// �궨���·����
	HTuple hv_CamParam;//�궨�������ڲ�
	HTuple hv_CamPose;//�궨���������
	HTuple hv_Error;//�궨���(�Ż���ͶӰ�ľ�������RMSE������������Ϊ��λ������)
	HTuple hv_Resolution;//���ص���
	//�����ת�����궨������ϵ����������
	HTuple hv_InputX1 = 0.000;		//��ת������������X1
	HTuple hv_InputY1 = 0.000;		//��ת������������Y1

	HTuple hv_InputX2 = 0.000;		//��ת������������X2
	HTuple hv_InputY2 = 0.000;		//��ת������������Y2

	HTuple hv_InputX3 = 0.000;		//��ת������������X3
	HTuple hv_InputY3 = 0.000;		//��ת������������Y3

	HTuple hv_InputX4 = 0.000;		//��ת������������X4
	HTuple hv_InputY4 = 0.000;		//��ת������������Y4

	HTuple hv_InputX5 = 0.000;		//��ת������������X5
	HTuple hv_InputY5 = 0.000;		//��ת������������Y5

	HTuple hv_InputX6 = 0.000;		//��ת������������X6
	HTuple hv_InputY6 = 0.000;		//��ת������������Y6

	HTuple hv_InputX7 = 0.000;		//��ת������������X7
	HTuple hv_InputY7 = 0.000;		//��ת������������Y7

	HTuple hv_InputX8 = 0.000;		//��ת������������X8
	HTuple hv_InputY8 = 0.000;		//��ת������������Y8

	HTuple hv_InputX9 = 0.000;		//��ת������������X9
	HTuple hv_InputY9 = 0.000;		//��ת������������Y9

	HTuple hv_InputX10 = 0.000;		//��ת������������X10
	HTuple hv_InputY10 = 0.000;		//��ת������������Y10
};
//��������ṹ��
struct ResultParamsStruct_JointCalibrationDetect
{
	HTuple hv_OutputX1 = 0.000;	//ת���������X1
	HTuple hv_OutputY1 = 0.000;	//ת���������Y1

	HTuple hv_OutputX2 = 0.000;	//ת���������X2
	HTuple hv_OutputY2 = 0.000;	//ת���������Y2

	HTuple hv_OutputX3 = 0.000;	//ת���������X3
	HTuple hv_OutputY3 = 0.000;	//ת���������Y3

	HTuple hv_OutputX4 = 0.000;	//ת���������X4
	HTuple hv_OutputY4 = 0.000;	//ת���������Y4

	HTuple hv_OutputX5 = 0.000;	//ת���������X5
	HTuple hv_OutputY5 = 0.000;	//ת���������Y5

	HTuple hv_OutputX6 = 0.000;	//ת���������X6
	HTuple hv_OutputY6 = 0.000;	//ת���������Y6

	HTuple hv_OutputX7 = 0.000;	//ת���������X7
	HTuple hv_OutputY7 = 0.000;	//ת���������Y7

	HTuple hv_OutputX8 = 0.000;	//ת���������X8
	HTuple hv_OutputY8 = 0.000;	//ת���������Y8

	HTuple hv_OutputX9 = 0.000;	//ת���������X9
	HTuple hv_OutputY9 = 0.000;	//ת���������Y9

	HTuple hv_OutputX10 = 0.000;	//ת���������X10
	HTuple hv_OutputY10 = 0.000;	//ת���������Y10

};

struct TypeParamsStruct_MachineLearning
{
	//ģ��ƥ������
	HTuple ShapeModel = "������״";
	HTuple NccModel = "���ڻ����";
	//��������
	HTuple SortType_Row = "��������";//��������
	HTuple SortType_Column = "��������";//��������
	//ROI����
	HTuple DetectROIType_Draw = "�ֻ�ROI";	//�ֻ�ROI
	HTuple DetectROIType_Blob = "Blob�Զ�ROI";	//Blob�Զ�ROI
		//��ֵ�����Ͳ���
	HTuple Fixed_Threshold = "�̶���ֵ";
	HTuple Auto_Threshold = "����Ӧ��ֵ";
	HTuple Dyn_Threshold = "��̬��ֵ";
	HTuple DynPolarity_Light = "light";
	HTuple DynPolarity_Dark = "dark";
	HTuple DynPolarity_Equal = "equal";
	HTuple DynPolarity_NotEqual = "not_equal";

	//��̬ѧ��ʽ
	HTuple Opening_Rec = "���ο�����";
	HTuple Closing_Rec = "���α�����";
	HTuple Opening_Cir = "Բ�ο�����";
	HTuple Closing_Cir = "Բ�α�����";
	HTuple Erosion_Rec = "���θ�ʴ";
	HTuple Dilation_Rec = "��������";
	HTuple Erosion_Cir = "Բ�θ�ʴ";
	HTuple Dilation_Cir = "Բ������";

	HTuple hv_Null = "null";
};
// ���в����ṹ��
struct RunParamsStruct_MachineLearning
{
	//WeightToleranceָ��ÿ�ε�������Ȩ�ص���ֵ����������ε���֮���Ȩ�ر仯�ľ���ֵ��͡�
	//��ˣ���ֵȡ����Ȩ�ص������Լ�Ȩ�صĴ�С����Ȩ�صĴ�С��ȡ����ѵ�����ݵ����š�ͨ����Ӧʹ��0.00001��1֮���ֵ��
	//ErrorToleranceָ��ÿ�ε����Ĵ���ֵ���ĵ���ֵ����ֵȡ����ѵ�������������Լ�MLP�����������������ͬ�������ͨ��Ӧʹ��0.00001��1֮���ֵ��
	//���Ȩ�ر仯С��WeightTolerance�������ֵ�ı仯С��ErrorTolerance������ֹ�Ż������κ�����£��Ż������MaxIterations��������ֹ��
	//NumHidden��С�ᵼ����ʧ�����ﲻ��ѵ��Ҫ��NumHidden������ᵼ�¹���ϣ����Ա�����δ��������������

	//��Դ����
	HTuple hv_ImgFilePathArray;	//ѵ��ͼƬ�ļ�������
	vector<HObject> ho_DetectROIArray;	//���е�Сͼ���roi
	HTuple hv_ClassNameArray;	//ѵ����������
	HTuple hv_ClassIDArray;	//ѵ��ID����
	HTuple hv_NGID = -1;//ָ��NG��IDֵ(-1����ָ��)
	HTuple hv_IsUnionDetectROI = 0;//�ϲ����еļ������(0 ���ϲ���1 �ϲ�),����ɫѡ����Ŀ����Ӧ����ͼ������е�Ϻ�ţ��޳�NG����û��ָ��ROIλ�ü��
	HTuple hv_IsAddNG = 0;//0�ǲ�����β��NG���࣬1������β��NG����(����β��NG����󣬲����������Ӽ���β��NG���಻����ROI���ã�ֻ���ж�NG��id��)
	HTuple hv_IsZoomSize = 0;//0�ǲ����ţ�1������
	HTuple hv_Zoom_Width = 640;//���ſ�
	HTuple hv_Zoom_Height = 640;//���Ÿ�
	//MLP����
	HTuple hv_Iterations = 200;//��������
	HTuple hv_NumHidden = 15;//���ز�����
	HTuple hv_WeightTolerance = 1;//Ȩ��
	HTuple hv_ErrorTolerance = 0.0001;//��С���
	HTuple hv_Rejection = "no_rejection_class";//���þܾ���
	HTuple hv_ClassifyHandle = NULL;	//ѵ�����
	HTuple hv_ErrorData = 1;	//ѵ�����
	HTuple hv_ErrorLog;//ѵ�������������

	//λ����������
	HObject ho_SearchRegion_PositionCorrection;	//��������
	HObject ho_TrainRegion_PositionCorrection;	//ѵ������
	HTuple hv_Row_PositionCorrection = 0;	//λ������ģ��row
	HTuple hv_Column_PositionCorrection = 0;//λ������ģ��column
	HTuple hv_Angle_PositionCorrection = 0;//λ������ģ��angle
	HTuple hv_MatchMethod_PositionCorrection = "������״";      //ģ������:"������״"��"���ڻ����	
	HTuple hv_AngleStart_PositionCorrection = -180;            //��ʼ�Ƕ�
	HTuple hv_AngleExtent_PositionCorrection = 180;                //��ֹ�Ƕ�
	HTuple hv_ScaleRMin_PositionCorrection = 1;                 //��С������
	HTuple hv_ScaleRMax_PositionCorrection = 1;                 //���������
	HTuple hv_ScaleCMin_PositionCorrection = 1;                 //��С������
	HTuple hv_ScaleCMax_PositionCorrection = 1;				   //���������
	HTuple hv_MinScore_PositionCorrection = 0.5;               //��ͷ���
	HTuple hv_NumMatches_PositionCorrection = 1;               //ƥ�������0���Զ�ѡ��100�����ƥ��100��
	HTuple hv_ModelID_PositionCorrection = NULL;                     //ģ����
	HTuple hv_Check_PositionCorrection = 0;	//�Ƿ�����λ������(0�����ã� 1����)
	HTuple hv_SortType_PositionCorrection;	//����ʽ

	//ROI����
	HTuple ho_DetectROIType = "�ֻ�ROI";	//���roi������
	//��ֵ������
	HObject ho_SearchRegion;	//�������������
	HTuple hv_SortType;				//����,pin��������ʽ
	HTuple hv_ThresholdType;		//����,ͼ���ֵ������
	HTuple hv_LowThreshold;				//����,����ֵ(�̶���ֵ)
	HTuple hv_HighThreshold;				//����,����ֵ(�̶���ֵ)
	HTuple hv_Sigma;						//����,ƽ����ֵ
	HTuple hv_CoreWidth;					//����,��̬��ֵ(�˲��˿��)
	HTuple hv_CoreHeight;					//����,��̬��ֵ(�˲��˸߶�)
	HTuple hv_DynThresholdContrast;		//����,��̬��ֵ�ָ�ͼ��Աȶ�
	HTuple hv_DynThresholdPolarity;	//����,��̬��ֵ����ѡ��(ѡ����ȡ���������������)��light,dark

	//ɸѡBlob����
	HTuple hv_AreaFilter_Min;	//����
	HTuple hv_RecLen1Filter_Min;//����
	HTuple hv_RecLen2Filter_Min;//����
	HTuple hv_RowFilter_Min;//����
	HTuple hv_ColumnFilter_Min;//����
	HTuple hv_CircularityFilter_Min;//����
	HTuple hv_RectangularityFilter_Min;//����
	HTuple hv_WidthFilter_Min;//����
	HTuple hv_HeightFilter_Min;//����
	HTuple hv_AreaFilter_Max;//����
	HTuple hv_RecLen1Filter_Max;//����
	HTuple hv_RecLen2Filter_Max;//����
	HTuple hv_RowFilter_Max;//����
	HTuple hv_ColumnFilter_Max;//����
	HTuple hv_CircularityFilter_Max;//����
	HTuple hv_RectangularityFilter_Max;//����
	HTuple hv_WidthFilter_Max;//����
	HTuple hv_HeightFilter_Max;//����
	HTuple hv_SelectAreaMax;	//ѡ��������������ֵ������(0���ã�1������)
	//��̬ѧ����
	HTuple hv_OperatorType;//����
	HTuple hv_OperaRec_Width;//����
	HTuple hv_OperaRec_Height;//����
	HTuple hv_OperaCir_Radius;//����
	//�׶�������
	HTuple hv_FillUpShape_Min;//����
	HTuple hv_FillUpShape_Max;//����
};
// ��������ṹ��
struct ResultParamsStruct_MachineLearning
{
	HTuple hv_Ret = 1;           //���(1 OK, 0 NG)
	HTuple hv_RetNameArray;	//ʶ��������
	HTuple hv_RetIDArray;	//ʶ��ID
	HTuple hv_IsOKArray;	//�Ƿ�OK(OK, NG)
	HTuple hv_ConfidenceArray;	//ʶ�������Ŷ�
	HObject ho_AllROIArray;//ʶ������
	HObject ho_OKROIArray;//OK����
	HObject ho_NGROIArray;//NG����
	HObject ho_ModelXLD;//λ��������ģ������

};

// ���в����ṹ��
struct RunParamsStruct_MultCamCalib
{
	HTuple hv_NPointCamera_X;		 //�ŵ�궨��������X
	HTuple hv_NPointCamera_Y;		 //�ŵ�궨��������Y
	HTuple hv_NPointBoard_X;		 //�ŵ�궨�궨������X
	HTuple hv_NPointBoard_Y;		 //�ŵ�궨�궨������Y
	HTuple hv_HomMat2D = NULL;		 //�ŵ�궨����

	HTuple hv_InputX1 = 0.000;		//��ת������������X1
	HTuple hv_InputY1 = 0.000;		//��ת������������Y1

	HTuple hv_InputX2 = 0.000;		//��ת������������X2
	HTuple hv_InputY2 = 0.000;		//��ת������������Y2

	HTuple hv_InputX3 = 0.000;		//��ת������������X3
	HTuple hv_InputY3 = 0.000;		//��ת������������Y3

	HTuple hv_InputX4 = 0.000;		//��ת������������X4
	HTuple hv_InputY4 = 0.000;		//��ת������������Y4
};

//����ṹ��
struct ResultParamsStruct_MultCamCalib
{
	HTuple hv_OutputX1 = 0.000;	//ת���������X1
	HTuple hv_OutputY1 = 0.000;	//ת���������Y1

	HTuple hv_OutputX2 = 0.000;	//ת���������X2
	HTuple hv_OutputY2 = 0.000;	//ת���������Y2

	HTuple hv_OutputX3 = 0.000;	//ת���������X3
	HTuple hv_OutputY3 = 0.000;	//ת���������Y3

	HTuple hv_OutputX4 = 0.000;	//ת���������X4
	HTuple hv_OutputY4 = 0.000;	//ת���������Y4
};

//ColorSortDetect������Ͳ����ṹ��
struct TypeParamsStruct_ColorSort
{
	//��ֵ�����Ͳ���
	HTuple Fixed_Threshold = "�̶���ֵ";
	HTuple Auto_Threshold = "����Ӧ��ֵ";
	HTuple Dyn_Threshold = "��̬��ֵ";
	HTuple DynPolarity_Light = "light";
	HTuple DynPolarity_Dark = "dark";
	HTuple DynPolarity_Equal = "equal";
	HTuple DynPolarity_NotEqual = "not_equal";

	//��̬ѧ��ʽ
	HTuple Opening_Rec = "���ο�����";
	HTuple Closing_Rec = "���α�����";
	HTuple Opening_Cir = "Բ�ο�����";
	HTuple Closing_Cir = "Բ�α�����";
	HTuple Erosion_Rec = "���θ�ʴ";
	HTuple Dilation_Rec = "��������";
	HTuple Erosion_Cir = "Բ�θ�ʴ";
	HTuple Dilation_Cir = "Բ������";

	//ɸѡ����
	HTuple hv_Area_Filter = "area";//area
	HTuple hv_Column_Filter = "column";//������(X)column
	HTuple hv_Row_Filter = "row";//�����꣨Y��row
	HTuple hv_Circularity_Filter = "circularity";//circularity
	HTuple hv_Compactness_Filter = "compactness";//compactness
	HTuple hv_Convexity_Filter = "convexity";//convexity
	HTuple hv_Rectangularity_Filter = "rectangularity";//rectangularity
	HTuple hv_Height_Filter = "height";//height
	HTuple hv_Width_Filter = "width";//width
	HTuple hv_OuterRadius_Filter = "outer_radius";//outer_radius
	HTuple hv_InnerRadius_Filter = "inner_radius";//inner_radius
	HTuple hv_Rect2Len1_Filter = "rect2_len1";//rect2_len1
	HTuple hv_Rect2Len2_Filter = "rect2_len2";//rect2_len2
	HTuple hv_Rect2Angle_Filter = "rect2_phi";//rect2_phi
	HTuple hv_Ra_Filter = "ra";//ra
	HTuple hv_Rb_Filter = "rb";//rb
	HTuple hv_Phi_Filter = "phi";//phi

	HTuple hv_Null = "null";
};
//ColorSortDetect������в����ṹ��
struct RunParamsStruct_ColorSort
{
	HObject ho_SearchRegion;		//����,��������
	//��ֵ������
	HTuple hv_Channel_Product = "b";//ʹ���ĸ�ͨ�����в�Ʒ��ȡ(r,g,b)
	HTuple hv_ThresholdType = "�̶���ֵ";		//����,ͼ���ֵ������
	HTuple hv_LowThreshold = 50;				//����,����ֵ(�̶���ֵ)
	HTuple hv_HighThreshold = 128;				//����,����ֵ(�̶���ֵ)
	HTuple hv_Sigma = 1;						//����,ƽ����ֵ
	HTuple hv_CoreWidth = 5;					//����,��̬��ֵ(�˲��˿��)
	HTuple hv_CoreHeight = 5;					//����,��̬��ֵ(�˲��˸߶�)
	HTuple hv_DynThresholdContrast = 15;		//����,��̬��ֵ�ָ�ͼ��Աȶ�
	HTuple hv_DynThresholdPolarity = "dark";	//����,��̬��ֵ����ѡ��(ѡ����ȡ���������������)��light,dark

	//��̬ѧ����
	HTuple hv_OperatorType = "null";//����
	HTuple hv_OperaRec_Width = 5;//����
	HTuple hv_OperaRec_Height = 5;//����
	HTuple hv_OperaCir_Radius = 5;//����
	//�׶�������
	HTuple hv_FillUpShape_Min = 1;//����
	HTuple hv_FillUpShape_Max = 1000;//����
	//����ɸѡ����
	HTuple hv_FeaturesFilter_Type;				 //���飬����ɸѡ��������(��Ҫʹ�ýṹ��TypeStruct_FeaturesFilter_MorphologyDetect����Ĳ���)
	HTuple hv_And_Or_Filter = "and";			//����ɸѡ�������֮��ѡ��"and"����"or",Ĭ��"and"
	HTuple hv_MinValue_Filter;					//���飬����ɸѡ������Сֵ
	HTuple hv_MaxValue_Filter;					//���飬����ɸѡ�������ֵ

	HTuple hv_UseBlob = 0;//1����Blob��ȡ��Ʒ��0������
	//��ɫ����
	HTuple hv_ColorValue1_Min = 50;//ͨ��1��ɫ��ֵmin
	HTuple hv_ColorValue1_Max = 128;//ͨ��1��ɫ��ֵmax
	HTuple hv_ColorValue2_Min = 50;//ͨ��2��ɫ��ֵmin
	HTuple hv_ColorValue2_Max = 128;//ͨ��2��ɫ��ֵmax
	HTuple hv_ColorValue3_Min = 50;//ͨ��3��ɫ��ֵmin
	HTuple hv_ColorValue3_Max = 128;//ͨ��3��ɫ��ֵmax
	HTuple hv_ColorArea_Min = 1;//��ɫɸѡ���min
	HTuple hv_ColorArea_Max = 999999999;//��ɫɸѡ���max
	HTuple hv_Channel_ColorSort = "hsv";//ʹ���ĸ�ͨ������ɫѡ(rgb,hsv,hsi)
	HTuple hv_UseUnionSortRegion = 0;	//������ȡ��Ʒ���Ƿ�ϲ���Ʒ����������ɫѡ����(1����,0������)

};
//ColorSortDetect������ṹ��
struct ResultParamsStruct_ColorSort
{
	HTuple hv_AreaRetArray;//�������
	HTuple hv_RowRetArray;	//����õ�������ɫ������С��Ӿ���������row
	HTuple hv_ColumnRetArray;	//����õ�������ɫ������С��Ӿ���������column
	HObject ho_RegionRetArray;//����õ�������ɫ����
};

//ColorExtractDetect������Ͳ����ṹ��
struct TypeParamsStruct_ColorExtract
{
	//ģ��ƥ������
	HTuple ShapeModel = "������״";
	HTuple NccModel = "���ڻ����";

	HTuple hv_Null = "null";
};
//ColorExtractDetect������в����ṹ��
struct RunParamsStruct_ColorExtract
{
	HObject ho_SearchRegion;		//����,��������

	//��ɫ��ȡ����
	HTuple hv_ColorValue1_Min = 50;//ͨ��1��ɫ��ֵmin
	HTuple hv_ColorValue1_Max = 128;//ͨ��1��ɫ��ֵmax
	HTuple hv_ColorValue2_Min = 50;//ͨ��2��ɫ��ֵmin
	HTuple hv_ColorValue2_Max = 128;//ͨ��2��ɫ��ֵmax
	HTuple hv_ColorValue3_Min = 50;//ͨ��3��ɫ��ֵmin
	HTuple hv_ColorValue3_Max = 128;//ͨ��3��ɫ��ֵmax
	HTuple hv_ColorArea_Min = 1;//��ɫɸѡ���min
	HTuple hv_ColorArea_Max = 999999999;//��ɫɸѡ���max
	HTuple hv_Channel_ColorExtract = "hsv";//ʹ���ĸ�ͨ������ɫѡ(rgb,hsv,hsi)

	//λ����������
	HObject ho_SearchRegion_PositionCorrection;	//��������
	HObject ho_TrainRegion_PositionCorrection;	//ѵ������
	HTuple hv_Row_PositionCorrection = 0;	//λ������ģ��row
	HTuple hv_Column_PositionCorrection = 0;//λ������ģ��column
	HTuple hv_Angle_PositionCorrection = 0;//λ������ģ��angle
	HTuple hv_MatchMethod_PositionCorrection = "������״";      //ģ������:"������״"��"���ڻ����	
	HTuple hv_AngleStart_PositionCorrection = -180;            //��ʼ�Ƕ�
	HTuple hv_AngleExtent_PositionCorrection = 180;                //��ֹ�Ƕ�
	HTuple hv_ScaleRMin_PositionCorrection = 1;                 //��С������
	HTuple hv_ScaleRMax_PositionCorrection = 1;                 //���������
	HTuple hv_ScaleCMin_PositionCorrection = 1;                 //��С������
	HTuple hv_ScaleCMax_PositionCorrection = 1;				   //���������
	HTuple hv_MinScore_PositionCorrection = 0.5;               //��ͷ���
	HTuple hv_NumMatches_PositionCorrection = 1;               //ƥ�������0���Զ�ѡ��100�����ƥ��100��
	HTuple hv_ModelID_PositionCorrection = NULL;                     //ģ����
	HTuple hv_Check_PositionCorrection = 0;	//�Ƿ�����λ������(0�����ã� 1����)
	HTuple hv_Channel_PositionCorrection = "gray";//ʹ���ĸ�ͨ����ģ��("r","g","b","gray")
};
//ColorExtractDetect������ṹ��
struct ResultParamsStruct_ColorExtract
{
	HObject ho_ModelXLD;//λ��������ģ������
	HObject ho_AffineTransSearchRegion;//����任�����������

	HTuple hv_AreaRetArray;//�������
	HTuple hv_RowRetArray;	//����õ�������ɫ������С��Ӿ���������row
	HTuple hv_ColumnRetArray;	//����õ�������ɫ������С��Ӿ���������column
	HObject ho_RegionExtract; //��ȡ����������ɫ����

	HTuple hv_Ret = 0;//0��NG��1��OK
};

struct RunParamsStruct_OCRDetect
{
	HTuple hv_Train_Type = 0;//0 ��ѵ��ģʽ��1ѵ��ģʽ
	HObject ho_SearchRegion;//ʶ������
	//��ѵ��ģʽ����
	HTuple hv_OCR_Type = "MLP";//ʶ�����ͣ�MLP����CNN
	HTuple hv_OCR_ModelName = "Document_0-9A-Z_Rej.omc";//ģ������
	HTuple hv_IsDotPrint = 0; //�Ƿ�Ϊ�����ӡ����,0����,1��
	HTuple hv_NotDetectPunctuation = 1;//���α�����,0�����Σ�1����
	HTuple hv_NotDetectSeparators = 1;//���ηָ���,0�����Σ�1����
	HTuple hv_UseCorrectAngle = 0;//���ýǶȽ���
	HTuple hv_Height_WordLine = 0;//�ǶȽ����ı��и߶�
	//ѵ��ģʽ����
	HTuple hv_BinarizationType = 0;//0�ǹ̶���ֵ��1���Զ���ֵ
	HTuple hv_FixThresholdMin = 0;
	HTuple hv_FixThresholdMax = 75;
	HTuple hv_PolarityChar = 0;//0�Ǻ��ַ��ױ�����1�ǰ��ַ��ڱ���
	HTuple hv_WidthChar_Min = 50;//�ַ���С���
	HTuple hv_WidthChar_Max = 50;//�ַ������
	HTuple hv_HeightChar_Min = 50;//�ַ���С�߶�
	HTuple hv_HeightChar_Max = 50;//�ַ����߶�
	HTuple hv_Distance_Char = 10;//�ַ�֮��������
	HTuple hv_BlockWidth_Min = 10;//�ʻ���С���
	HTuple hv_BlockWidth_Max = 10;//�ʻ������
	HTuple hv_BlockHeight_Min = 10;//�ʻ���С�߶�
	HTuple hv_BlockHeight_Max = 10;//�ʻ����߶�
	HTuple hv_BlockArea_Min = 10;//�ʻ���С���
	HTuple hv_BlockArea_Max = 1000;//�ʻ�������
	HTuple hv_Distance_Block = 10;//�ʻ�֮�������
	HTuple hv_WidthType = 0;//�ַ��������(0�ɱ�,1�̶����,2��̬���)
	HTuple hv_FixWidthType_Width = 50;//�̶����ģʽ���ַ��Ŀ�
	HTuple hv_FixWidthType_Height = 50;//�̶����ģʽ���ַ��ĸ�
	HTuple hv_DynamicWidthType_Width = 50;//��̬���ģʽ���ַ���
	HTuple hv_DynamicWidthType_Scale = 20;//��̬���ģʽ�����շָ�λ�����ʼ�ָ�λ�õ����ƫ��Ϊ����*�ٷֱ�*0.01��
	//ѵ�����ַ��⼯��
	HTuple hv_OcrTrainfile = NULL;
};
struct ResultParamsStruct_OCRDetect
{
	//�ں����
	HTuple hv_Confidence;//ʶ�����
	HTuple hv_OCR_Content;//ʶ�������
	HTuple hv_OCR_Row;//ʶ���λ��������
	HTuple hv_OCR_Column;//ʶ���λ��������
	HObject ho_OCR_Region;//ʶ�������
	//���ƺ�Ľ��ͼ
	Mat ho_DestImg;
};