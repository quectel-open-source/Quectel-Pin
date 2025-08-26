#include "pinLabel.h"
#include <QPainter>
#include <QPaintEvent>
#include <QDebug>
#include "QC_Log.h"
#include "rs.h"
#include "rs_motion.h"
using namespace cv;

//���嵥���Ŵ���
#define ZOOMRATIO 2.0

pinLabel::pinLabel( double in_Width, double in_Height)
    : m_hHalconID(0), m_width(in_Width), m_height(in_Height)
{
    // ȷ��Halcon�����ѳ�ʼ��
    initializeHalconWindow();
}

pinLabel::~pinLabel()
{
    //if (m_hHalconID.Length() == 0) {
    //    return;
    //}
    //if (m_hHalconID.Length() == 1 && m_hHalconID[0].I() == 0) {
    //    return;
    //}
    //m_hHalconID.
    //CloseWindow(m_hHalconID);  // �ر�Halcon����

    if(m_img.IsInitialized())
        m_img.Clear();
    if (ho_showImg.IsInitialized())
        ho_showImg.Clear();
    m_message_2D.clear();
}

//����allPinͼ��
void pinLabel::allPinImage(const HObject& region)
{
    try {
        m_message_2D.ho_Region_AllPin = region;
    }
    catch (const HalconCpp::HException& e) {
        qWarning() << "allPinImage Error displaying Halcon image or region:" << e.ErrorMessage().Text();
    }
}

void pinLabel::setImage(HObject img)
{
    m_img = img;
    //update();  // ������ͼ������paintEvent���»���
}

void pinLabel::initializeHalconWindow()
{
    // �����û�д���Halcon���ڣ��򴴽���
    if (m_hHalconID == NULL) {
        //int width = this->width();
        //int height = this->height();

        //m_hLabelID = (Hlong)this->winId();  // ��ȡQLabel���ھ��
        m_hLabelID = 0;

        // ����Halcon���ڲ��󶨵�QLabel�ؼ�
        OpenWindow(0, 0, m_width, m_height, m_hLabelID, "buffer", "", &m_hHalconID);
    }
}

//void pinLabel::paintEvent(QPaintEvent* event)
//{
//    QPainter painter(this);
//}


//2D����NG����������
void pinLabel::DrawTextNearRegion(HTuple& hHalconID,HObject ho_Region_NGPin, int index)
{
    try {
        // ȷ����������Ч��
        HTuple hv_NumRegions;
        CountObj(ho_Region_NGPin, &hv_NumRegions);
        if (index >= 1 && index <= hv_NumRegions)
        {
            HObject ho_SelectedRegion;

            // ʹ�� SelectObj ��������ȡָ��������
            SelectObj(ho_Region_NGPin, &ho_SelectedRegion, index);

            // ��ȡ�������������������
            HTuple hv_Row, hv_Column, hv_Area;
            AreaCenter(ho_SelectedRegion, &hv_Area, &hv_Row, &hv_Column);

            // ʹ�� DispText ��ʾ����
            HTuple xPos = m_message_2D.hv_OffsetX_Pixel_Array[index];
            HTuple yPos = m_message_2D.hv_OffsetY_Pixel_Array[index];
            HTuple hv_String = "X : " + xPos + " Y : " + yPos;
            //HTuple hv_String = "NG Pin Detected";
            HTuple  hv_Font = "-Arial-50-*-*-*-0-";  // ���������Arial�����ء���ͨ��ʽ
            // ��������
            SetFont(hHalconID, hv_Font);
            double a = hv_Row[0].D();
            double b = hv_Column[0].D();

            // ��ʾ����
            HTuple  hv_TextRow = hv_Row + 20;
            HTuple  hv_TextCol = hv_Column;
            // ����������鿴�ı���ʾ���к���
            std::cout << "TextRow: " << hv_TextRow[0].D() << " TextCol: " << hv_TextCol[0].D() << std::endl;

            if (hHalconID != NULL)
            {
                SetColor(hHalconID, "red");
                SetTposition(hHalconID, hv_TextRow, hv_TextCol);
                WriteString(hHalconID, hv_String);
                DispObj(ho_SelectedRegion, hHalconID);
            }
        }
    }
    catch (const HalconCpp::HException& e) {
        machineLog->write("Error DrawTextNearRegion Halcon image or region:" + QString(e.ErrorMessage().Text()), Normal);
    }
}

//ԭͼ�ü���ֻѡȡpin������
void pinLabel::CropImageAroundRegion(HObject ho_Region_AllPin, HObject& ho_CroppedImage)
{
    try {
        // ��������pin�������Ӿ���
        HTuple hv_Row1, hv_Column1, hv_Row2, hv_Column2;
        // �ϲ�����pin����
        HObject ho_UnionRegion;
        Union1(ho_Region_AllPin, &ho_UnionRegion);
        SmallestRectangle1(ho_UnionRegion, &hv_Row1, &hv_Column1, &hv_Row2, &hv_Column2);

        std::cout << "Bounding Box: "
            << "Row1: " << hv_Row1[0].D() << ", "
            << "Column1: " << hv_Column1[0].D() << ", "
            << "Row2: " << hv_Row2[0].D() << ", "
            << "Column2: " << hv_Column2[0].D() << std::endl;

        // ��չ��Ӿ��Σ�����200����
        HTuple hv_Extension = 200;
        hv_Row1 = hv_Row1 - hv_Extension;     // ������չ
        hv_Column1 = hv_Column1 - hv_Extension; // ������չ
        hv_Row2 = hv_Row2 + hv_Extension;     // ������չ
        hv_Column2 = hv_Column2 + hv_Extension; // ������չ

        // ����������鿴��չ��ľ���
        std::cout << "Extended Bounding Box: "
            << "Row1: " << hv_Row1[0].D() << ", "
            << "Column1: " << hv_Column1[0].D() << ", "
            << "Row2: " << hv_Row2[0].D() << ", "
            << "Column2: " << hv_Column2[0].D() << std::endl;
        // ��ȡͼ��ߴ�
        HTuple hv_Width, hv_Height;
        GetImageSize(m_img, &hv_Width, &hv_Height);

        // ��չ���Σ���ȷ�����겻Խ��
        hv_Row1 = (hv_Row1 < 0) ? HTuple(0) : hv_Row1;
        hv_Column1 = (hv_Column1 < 0) ? HTuple(0) : hv_Column1;

        // ʹ��ͼ��ߴ����ƾ�������
        hv_Row2 = (hv_Row2 > hv_Height) ? HTuple(hv_Height) : hv_Row2;
        hv_Column2 = (hv_Column2 > hv_Width) ? HTuple(hv_Width) : hv_Column2;

        // ������չ�����Ӿ��βü�ԭͼ��
        CropRectangle1(m_img, &ho_CroppedImage, hv_Row1, hv_Column1, hv_Row2, hv_Column2);
    }
    catch (const HalconCpp::HException& e) {
        machineLog->write("Error in CropImageAroundRegion: " + QString(e.ErrorMessage().Text()), Normal);
    }
}

void pinLabel::showImage()
{
    if (!m_img.IsInitialized()) {
        qWarning() << "Halcon image is not initialized.";
        return;
    }

    try {
        // ȷ��Halcon�����ѳ�ʼ��
        initializeHalconWindow();

        ClearWindow(m_hHalconID);

        generateCompositeImage();

    }
    catch (const HalconCpp::HException& e) {
        machineLog->write("Error displaying Halcon image or region:" + QString(e.ErrorMessage().Text()), Normal);
    }
}

void pinLabel::generateCompositeImage()
{
    try {
        HTuple hv_WindowID;
        // ����һ����ʱ�Ĵ���ID��ץȡ��ǰͼ��
        HTuple hv_width, hv_height;
        GetImageSize(m_img, &hv_width, &hv_height);
        OpenWindow(0, 0, hv_width, hv_height, m_hLabelID, "buffer", "",&hv_WindowID);
        SetPart(hv_WindowID, 0, 0, hv_height - 1, hv_width - 1);

        CopyObj(m_img, &ho_showImg, 1, 1);
        DispObj(ho_showImg, hv_WindowID);

        // ��ʾ NG ���� OK
        if (m_message_3D.PinFlag.size() != 0)
        {
            std::vector<int> ngIdx3D;
            for (int i = 0; i < m_message_3D.PinFlag.size(); i++)
            {
                //��¼ 3D ng�������
                if (m_message_3D.PinFlag[i] == false)
                {
                    ngIdx3D.emplace_back(i + 1);
                }
            }
            //2d��3d��ng����������
            std::unordered_set<int> allNgIdx;
            for (auto val : ngIdx3D)
            {
                allNgIdx.insert(val);
            }
            for (auto val : m_message_2D.ngIdx)
            {
                allNgIdx.insert(val);
            }
            

            if (allNgIdx.size() > 0)
            {
                //NG
                SetColor(hv_WindowID, "red");
                HTuple  hv_Font = "-Arial-500-*-*-*-0-";
                SetFont(hv_WindowID, hv_Font);
                // �����Ͻ���ʾ "NG"
                WriteString(hv_WindowID, "NG");
            }
            else
            {
                //OK
                SetColor(hv_WindowID, "green");
                HTuple  hv_Font = "-Arial-500-*-*-*-0-";
                // ��������
                SetFont(hv_WindowID, hv_Font);
                WriteString(hv_WindowID, "OK");
            }

            // �����OK���󣬻�������
            if (m_message_2D.ho_Region_OKPin.IsInitialized()) {
                SetColor(hv_WindowID, "green");
                SetDraw(hv_WindowID, "margin");
                SetLineWidth(hv_WindowID, 5);
                DispObj(m_message_2D.ho_Region_OKPin, hv_WindowID);
                //DrawTextNearRegion(hv_WindowID, m_message_2D.ho_Region_OKPin, 3);
            }

            for (auto idx : allNgIdx)
            {
                machineLog->write("NG idx :" + QString::number(idx), Normal);
                if (idx >= m_message_2D.hv_RetNum || idx >= m_message_3D.PinFlag.size())
                {
                    continue;
                }
                ////2D��3D�����ʾ��NG����
                //DrawTextNearRegion_2Dand3D(hv_WindowID, m_message_2D.ho_Region_AllPin, idx);

                //250424 ����ֱ����ʾ�����½ǣ�����ʾ��pin�븽����ֻ��ʾNG������
                HObject ho_SelectedRegion;
                SetColor(hv_WindowID, "red");
                SetDraw(hv_WindowID, "margin");
                SetLineWidth(hv_WindowID, 5);
                // ʹ�� SelectObj ��������ȡָ��������
                SelectObj(m_message_2D.ho_Region_AllPin, &ho_SelectedRegion, idx);
                DispObj(ho_SelectedRegion, hv_WindowID);
            }

            //��ע�������XY��heightƫ��
            SetColor(hv_WindowID, "blue");
            DrawTextAllNearRegion_2D3D(hv_WindowID, m_message_2D.ho_Region_AllPin, m_message_3D.PinHeight, allNgIdx);

        }
        else
        {
            std::unordered_set<int> allNgIdx;
            for (auto val : m_message_2D.ngIdx)
            {
                allNgIdx.insert(val);
            }
            //��2Dʶ����ʾ
            if (m_message_2D.hv_Ret == 1)
            {
                SetColor(hv_WindowID, "green");
                HTuple  hv_Font = "-Arial-500-*-*-*-0-";  // ���������Arial�����ء���ͨ��ʽ

                // ��������
                SetFont(hv_WindowID, hv_Font);
                WriteString(hv_WindowID, "OK");

            }
            else
            {
                SetColor(hv_WindowID, "red");
                HTuple  hv_Font = "-Arial-500-*-*-*-0-";
                SetFont(hv_WindowID, hv_Font);
                // �����Ͻ���ʾ "NG"
                WriteString(hv_WindowID, "NG");

                // �����NG���󣬻�������
                if (m_message_2D.ho_Region_NGPin.IsInitialized() && m_message_2D.ngIdx.size() > 0) {
                    SetDraw(hv_WindowID, "margin");
                    SetLineWidth(hv_WindowID, 5);
                    SetColor(hv_WindowID, "red");
                    DispObj(m_message_2D.ho_Region_NGPin, hv_WindowID);
                    //machineLog->write("8888888", Normal);
   /*                 for (auto idx : m_message_2D.ngIdx)
                    {
                        machineLog->write("NG idx :" + QString::number(idx), Normal);
                        DrawTextNearRegion(hv_WindowID, m_message_2D.ho_Region_AllPin, idx);
                    }*/
                }
            }

            // �����OK���󣬻�������
            if (m_message_2D.ho_Region_OKPin.IsInitialized()) {
                SetDraw(hv_WindowID, "margin");
                SetColor(hv_WindowID, "green");
                SetLineWidth(hv_WindowID, 5);
                DispObj(m_message_2D.ho_Region_OKPin, hv_WindowID);
                //DrawTextNearRegion(hv_WindowID, m_message_2D.ho_Region_OKPin, 3);
            }
            //��ע�������XYƫ��
            SetColor(hv_WindowID, "blue");
            DrawTextAllNearRegion(hv_WindowID, m_message_2D.ho_Region_AllPin,allNgIdx);
        }



        // ����ǰ�������ݱ��浽һ���µ�ͼ�����
        DumpWindowImage(&ho_showImg, hv_WindowID);

        // �ر���ʱ����
        CloseWindow(hv_WindowID);
        //������ͼ��ϲ������ˣ�������Ϊ��ʾģ�� Erik 2025/3/21
        //DispObj(ho_showImg, m_hHalconID);
    }
    catch (const HalconCpp::HException& e) {
        qDebug() << "Error saving composite image:" + QString(e.ErrorMessage().Text());
    }
}


void pinLabel::convertHalconToOpenCV(HObject& ho_image, cv::Mat& opencvImage)
{
    HTuple hv_AcquisitionResult, hv_Width, hv_Height;
    HTuple hv_Pointer, hv_Type;

    try {
        // ��ȡ Halcon ͼ��Ŀ�Ⱥ͸߶�
        GetImageSize(ho_image, &hv_Width, &hv_Height);

        // ��ȡ Halcon ͼ������ָ��
        GetImagePointer1(ho_image, &hv_Pointer, &hv_Type, &hv_Width, &hv_Height);

        // �ж�ͼ�����ͣ��������Ϊ 8 λ�Ҷ�ͼ�񣨿��Ը�����Ҫ������
        if (hv_Type == "byte") {
            // ʹ�� OpenCV �� Mat ��������ͼ��
            opencvImage = cv::Mat(hv_Height, hv_Width, CV_8UC1, (uchar*)hv_Pointer[0].L());

        }
        else if (hv_Type == "int2") {
            opencvImage = cv::Mat(hv_Height, hv_Width, CV_16SC1, (short*)hv_Pointer[0].L());
        }
        else if (hv_Type == "real") {
            opencvImage = cv::Mat(hv_Height, hv_Width, CV_32FC1, (float*)hv_Pointer[0].L());
        }
        else {
            throw HalconCpp::HException("Unsupported image type");
        }
    }
    catch (const HalconCpp::HException& e) {
        std::cerr << "Error converting Halcon image to OpenCV Mat: " << e.ErrorMessage().Text() << std::endl;
    }
}

//HobjectתMat�ĸ�ʽ
bool pinLabel::HObject2MatImg(HalconCpp::HObject& Hobj, cv::Mat& matImg)
{
    try {
        HalconCpp::HTuple htCh;
        HalconCpp::HString cType;

        HalconCpp::ConvertImageType(Hobj, &Hobj, "byte");
        HalconCpp::CountChannels(Hobj, &htCh);
        Hlong wid = 0;
        Hlong hgt = 0;
        if (htCh[0].I() == 1)
        {
            HalconCpp::HImage hImg(Hobj);
            void* ptr = hImg.GetImagePointer1(&cType, &wid, &hgt);
            int W = wid;
            int H = hgt;
            matImg = cv::Mat::zeros(H, W, CV_8UC1);
            unsigned char* pdata = static_cast<unsigned char*>(ptr);
            memcpy(matImg.data, pdata, W * H);
        }
        else if (htCh[0].I() == 3)
        {
            void* Rptr;
            void* Gptr;
            void* Bptr;
            HalconCpp::HImage hImg(Hobj);
            hImg.GetImagePointer3(&Rptr, &Gptr, &Bptr, &cType, &wid, &hgt);
            int W = wid;
            int H = hgt;
            matImg = cv::Mat::zeros(H, W, CV_8UC3);
            std::vector<cv::Mat> VecM(3);
            VecM[0].create(H, W, CV_8UC1);
            VecM[1].create(H, W, CV_8UC1);
            VecM[2].create(H, W, CV_8UC1);
            unsigned char* R = (unsigned char*)Rptr;
            unsigned char* G = (unsigned char*)Gptr;
            unsigned char* B = (unsigned char*)Bptr;
            memcpy(VecM[2].data, R, W * H);
            memcpy(VecM[1].data, G, W * H);
            memcpy(VecM[0].data, B, W * H);
            cv::merge(VecM, matImg);
        }
    }
    catch (const HalconCpp::HException& e) {
        std::cerr << "HObject2MatImg: " << e.ErrorMessage().Text() << std::endl;
    }

    return true;
}


//2D,3D����NG����������
void pinLabel::DrawTextNearRegion_2Dand3D(HTuple& hHalconID, HObject ho_Region_AllPin, int index)
{
    try {
        // ȷ����������Ч��
        HTuple hv_NumRegions;
        CountObj(ho_Region_AllPin, &hv_NumRegions);
        if (index >= 1 && index <= hv_NumRegions)
        {
            HObject ho_SelectedRegion;

            // ʹ�� SelectObj ��������ȡָ��������
            SelectObj(ho_Region_AllPin, &ho_SelectedRegion, index);

            // ��ȡ�������������������
            HTuple hv_Row, hv_Column, hv_Area;
            AreaCenter(ho_SelectedRegion, &hv_Area, &hv_Row, &hv_Column);

            // ʹ�� DispText ��ʾ����
            HTuple xPos = m_message_2D.hv_OffsetX_Pixel_Array[index];
            HTuple yPos = m_message_2D.hv_OffsetY_Pixel_Array[index];
            HTuple hPos = m_message_3D.PinHeight[index];
            HTuple hv_String = "X : " + xPos + " Y : " + yPos + "Height : " + hPos;
            //HTuple hv_String = "NG Pin Detected";
            HTuple  hv_Font = "-Arial-50-*-*-*-0-";  // ���������Arial�����ء���ͨ��ʽ
            // ��������
            SetFont(hHalconID, hv_Font);
            double a = hv_Row[0].D();
            double b = hv_Column[0].D();

            // ��ʾ����
            HTuple  hv_TextRow = hv_Row + 20;
            HTuple  hv_TextCol = hv_Column;
            // ����������鿴�ı���ʾ���к���
            std::cout << "TextRow: " << hv_TextRow[0].D() << " TextCol: " << hv_TextCol[0].D() << std::endl;

            if (hHalconID != NULL)
            {
                SetColor(hHalconID, "red");
                SetTposition(hHalconID, hv_TextRow, hv_TextCol);
                WriteString(hHalconID, hv_String);
                DispObj(ho_SelectedRegion, hHalconID);
            }
        }
    }
    catch (const HalconCpp::HException& e) {
        machineLog->write("Error DrawTextNearRegion Halcon image or region:" + QString(e.ErrorMessage().Text()), Normal);
    }
}

//MatתHObject
HObject pinLabel::HMatToHObject(cv::Mat image)
{
    //HObject ho_obj = HObject();
    //if (image.empty())
    //    return ho_obj;
    ////3ͨ��
    //if (3 == image.channels())
    //{
    //    int w = image.cols;
    //    int h = image.rows;
    //    //byteλͼ
    //    if (image.depth() == CV_8U)
    //    {
    //        vector<cv::Mat> ichannels;
    //        cv::Mat imagB;
    //        cv::Mat imagG;
    //        cv::Mat imagR;
    //        cv::split(image, ichannels);
    //        imagB = ichannels.at(0);
    //        imagG = ichannels.at(1);
    //        imagR = ichannels.at(2);
    //        uchar* dataRed = new uchar[w * h];
    //        uchar* dataGreen = new uchar[w * h];
    //        uchar* dataBlue = new uchar[w * h];
    //        for (int i = 0; i < h; i++)
    //        {
    //            memcpy(dataRed + w * i, imagR.ptr() + imagR.step * i, w);
    //            memcpy(dataGreen + w * i, imagG.ptr() + imagG.step * i, w);
    //            memcpy(dataBlue + w * i, imagB.ptr() + imagB.step * i, w);
    //        }
    //        GenImage3(&ho_obj, "byte", w, h, (Hlong)(dataRed), (Hlong)(dataGreen), (Hlong)(dataBlue));
    //        delete[] dataRed;
    //        delete[] dataGreen;
    //        delete[] dataBlue;
    //    }
    //    //���ͼreal
    //    else if (image.depth() == CV_32F)
    //    {
    //        HObject ho_X, ho_Y, ho_Z, ho_Rect;
    //        HTuple hv_Rows, hv_Cols, hv_Gray;
    //        GenImageConst(&ho_X, "real", w, h);
    //        GenImageConst(&ho_Y, "real", w, h);
    //        GenImageConst(&ho_Z, "real", w, h);
    //        Compose3(ho_X, ho_Y, ho_Z, &ho_obj);
    //        int* rows = new int[w * h];
    //        int* cols = new int[w * h];
    //        double* dGray = new double[3 * w * h];
    //        int k = 0;
    //        for (int y = 0; y < h; y++)
    //        {
    //            Vec3f* ptr = image.ptr<Vec3f>(y);
    //            for (int x = 0; x < w; x++)
    //            {
    //                float xx = ptr[x][0];
    //                float yy = ptr[x][1];
    //                float zz = ptr[x][2];
    //                if (xx == 0.0 && yy == 0.0 && zz == 0.0)
    //                    continue;
    //                dGray[k * 3] = xx;
    //                dGray[k * 3 + 1] = yy;
    //                dGray[k * 3 + 2] = zz;
    //                rows[k] = y;
    //                cols[k] = x;
    //                k++;
    //            }
    //        }
    //        hv_Rows = HTuple(rows, k);
    //        hv_Cols = HTuple(cols, k);
    //        hv_Gray = HTuple(dGray, 3 * k);
    //        SetGrayval(ho_obj, hv_Rows, hv_Cols, hv_Gray);
    //        GenRegionPoints(&ho_Rect, hv_Rows, hv_Cols);
    //        ReduceDomain(ho_obj, ho_Rect, &ho_obj);
    //        delete[] rows;
    //        delete[] cols;
    //        delete[] dGray;
    //    }
    //}
    ////byteλͼ
    //else if (1 == image.channels())
    //{
    //    int w = image.cols;
    //    int h = image.rows;
    //    //λͼ
    //    if (image.depth() == CV_8U)
    //    {
    //        uchar* dataGray = new uchar[w * h];
    //        for (int i = 0; i < h; i++)
    //            memcpy(dataGray + w * i, image.ptr() + image.step * i, w);
    //        GenImage1(&ho_obj, "byte", w, h, (Hlong)(dataGray));
    //        delete[] dataGray;
    //    }
    //    //���ͼreal
    //    else if (image.depth() == CV_32F)
    //    {
    //        HObject ho_Rect;
    //        HTuple hv_Rows, hv_Cols, hv_Gray;
    //        GenImageConst(&ho_obj, "real", w, h);
    //        int* rows = new int[w * h];
    //        int* cols = new int[w * h];
    //        double* dGray = new double[w * h];
    //        int k = 0;
    //        for (int y = 0; y < h; y++)
    //        {
    //            float* ptr = image.ptr<float>(y);
    //            for (int x = 0; x < w; x++)
    //            {
    //                float gg = ptr[x];
    //                if (gg == 0.0)
    //                    continue;
    //                dGray[k] = gg;
    //                rows[k] = y;
    //                cols[k] = x;
    //                k++;
    //            }
    //        }
    //        hv_Rows = HTuple(rows, k);
    //        hv_Cols = HTuple(cols, k);
    //        hv_Gray = HTuple(dGray, k);
    //        SetGrayval(ho_obj, hv_Rows, hv_Cols, hv_Gray);
    //        GenRegionPoints(&ho_Rect, hv_Rows, hv_Cols);
    //        ReduceDomain(ho_obj, ho_Rect, &ho_obj);
    //        delete[] rows;
    //        delete[] cols;
    //        delete[] dGray;
    //    }
    //}
    //return ho_obj;


    HObject Hobj = HObject();
    int hgt = image.rows;
    int wid = image.cols;
    int i;
    //  CV_8UC3  
    if (image.type() == CV_8UC3)
    {
        std::vector<cv::Mat> imgchannel;
        split(image, imgchannel);
        cv::Mat imgB = imgchannel[0];
        cv::Mat imgG = imgchannel[1];
        cv::Mat imgR = imgchannel[2];
        uchar* dataR = new uchar[hgt * wid];
        uchar* dataG = new uchar[hgt * wid];
        uchar* dataB = new uchar[hgt * wid];
        for (i = 0; i < hgt; i++)
        {
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
    //  CV_8UCU1  
    else if (image.type() == CV_8UC1)
    {
        uchar* data = new uchar[hgt * wid];
        for (i = 0; i < hgt; i++)
            memcpy(data + wid * i, image.data + image.step * i, wid);
        GenImage1(&Hobj, "byte", wid, hgt, (Hlong)data);
        delete[] data;
        data = NULL;
    }
    return Hobj;
}

// չʾ3DͼƬ
void pinLabel::show3DImage()
{
    if (!m_img.IsInitialized()) {
        qWarning() << "3D Halcon image is not initialized.";
        return;
    }

    try {
        // ȷ��Halcon�����ѳ�ʼ��
        initializeHalconWindow();

        ClearWindow(m_hHalconID);

        HTuple hv_WindowID;
        // ����һ����ʱ�Ĵ���ID��ץȡ��ǰͼ��
        HTuple hv_width, hv_height;
        GetImageSize(m_img, &hv_width, &hv_height);
        OpenWindow(0, 0, hv_width, hv_height, m_hLabelID, "buffer", "", &hv_WindowID);
        SetPart(hv_WindowID, 0, 0, hv_height - 1, hv_width - 1);

        CopyObj(m_img, &ho_showImg, 1, 1);
        DispObj(ho_showImg, hv_WindowID);
        //��
        //bool isNG = false;
        //if(m_message_3D.PinFlag.empty())
        //    isNG = true;
        //for (int i = 0; i < m_message_3D.PinFlag.size(); i++)
        //{
        //    machineLog->write("m_message_3D.PinFlag[ " + QString::number(i)+" ] = " + QString::number(m_message_3D.PinFlag[i]), Normal);
        //    // ����3D�Ƿ�NG
        //    if (m_message_3D.PinFlag[i] != 1 || m_message_3D.PinHeight[i] == -2)
        //    {
        //        isNG = true;
        //        break;
        //    }
        //}

        //��
        bool isOK = m_message_3D.isOK;
        //isOK = true;
        if (!isOK)
        {
            SetColor(hv_WindowID, "red");
            HTuple  hv_Font = "-Arial-500-*-*-*-0-";
            SetFont(hv_WindowID, hv_Font);
            // �����Ͻ���ʾ "NG"
            WriteString(hv_WindowID, "NG");
        }
        else
        {
            SetColor(hv_WindowID, "green");
            HTuple  hv_Font = "-Arial-500-*-*-*-0-";  // ���������Arial�����ء���ͨ��ʽ

            // ��������
            SetFont(hv_WindowID, hv_Font);
            WriteString(hv_WindowID, "OK");
        }

        // ����ǰ�������ݱ��浽һ���µ�ͼ�����
        DumpWindowImage(&ho_showImg, hv_WindowID);

        // �ر���ʱ����
        CloseWindow(hv_WindowID);
        //������ͼ��ϲ������ˣ�������Ϊ��ʾģ�� Erik 2025/3/21
        //DispObj(ho_showImg, m_hHalconID);
    }
    catch (const HalconCpp::HException& e) {
        machineLog->write("3D Error displaying Halcon image or region:" + QString(e.ErrorMessage().Text()), Normal);
    }
}

void pinLabel::DrawTextAllNearRegion(HTuple& hHalconID, HObject ho_Region_AllPin, std::unordered_set<int> allNgIdx)
{
    try {
        // ��ȡ��������
        HTuple hv_NumRegions;
        CountObj(ho_Region_AllPin, &hv_NumRegions);

        // ��ȡͼ��ߴ�
        HTuple hv_Width, hv_Height;
        GetImageSize(ho_showImg, &hv_Width, &hv_Height);

        // ����������ʽ
        HTuple hv_Font = "-Arial-50-*-*-*-0-";
        SetFont(hHalconID, hv_Font);

        // ��һ���֣���ÿ������������ʾ�������֣����ֲ��䣩
        for (HTuple hv_Index = 1; hv_Index <= hv_NumRegions; hv_Index += 1) {
            if (allNgIdx.count(hv_Index.I()))
            {
                //NG�ú�ɫ����
                SetColor(hHalconID, "red");
            }
            else
            {
                SetColor(hHalconID, "blue");
            }
            HObject ho_SingleRegion;
            SelectObj(ho_Region_AllPin, &ho_SingleRegion, hv_Index);

            // ��ȡ������������
            HTuple hv_Row, hv_Column, hv_Area;
            AreaCenter(ho_SingleRegion, &hv_Area, &hv_Row, &hv_Column);

            // ���������ı���������1��ʼ��ʾ��
            HTuple hv_Text = (hv_Index).TupleString("d");

            // �����ı�λ��Ϊ�������ģ������ƫ���������ڵ���
            SetTposition(hHalconID, hv_Row + 15, hv_Column - 15);
            WriteString(hHalconID, hv_Text);
        }

        // �ڶ����֣���̬������ʾ������Ϣ
        HTuple hv_StartRow = hv_Height - 1000;  // ��ʼ�У���ײ�1000���أ�
        HTuple hv_BaseCol = hv_Width - 1500;    // ��׼�У����Ҳ�1500���أ�
        HTuple hv_LineHeight = 70;             // �и�
        HTuple hv_ColOffset = 1500;             // �м�ƫ����

        for (HTuple hv_Idx = 1; hv_Idx <= hv_NumRegions; hv_Idx += 1) {
            // ��ȡ����ֵ
            HTuple xPos = m_message_2D.hv_OffsetX_MM_Array[hv_Idx - 1];
            HTuple yPos = m_message_2D.hv_OffsetY_MM_Array[hv_Idx - 1];
            // ��ʽ���ı�
            HTuple hv_CoordStr = hv_Idx.TupleString("d") + ": X offset: " +
                xPos.TupleString(".2f") + ", Y offset:" +
                yPos.TupleString(".2f") + ", Height: 0.0";


            // ��̬������λ��
            HTuple hv_CurrentCol, hv_CurrentRow;
            HTuple hv_ColWidth = 1000;  // ���磺ÿ�п��300����
            HTuple hv_MaxPerColumn = 15;  // ÿ�������Ŀ��

            if (hv_NumRegions.I() > 20)
            {
                //����20����
                HTuple hv_StartRow = hv_Height - 1200;  // ��ʼ�У���ײ�1000���أ�
                hv_BaseCol = hv_Width - 1000;
                // �����кź��к�
                HTuple hv_Column = (hv_Idx - 1) / hv_MaxPerColumn;         // ��������0,1,2...��
                HTuple hv_RowInColumn = (hv_Idx - 1) % hv_MaxPerColumn;    // ��ǰ���ڵ��кţ�0~19��

                // ����λ��
                hv_CurrentCol = hv_BaseCol - (hv_Column * hv_ColWidth);     // �������и�Ϊ�ӷ��������ü���
                hv_CurrentRow = hv_StartRow + (hv_RowInColumn + 1) * hv_LineHeight; // +1�������д�0��ʼ

            }
            else
            {
                if (hv_Idx <= 10) {
                    // ǰ10����ʾ������
                    hv_CurrentCol = hv_BaseCol;
                    hv_CurrentRow = hv_StartRow + (hv_Idx * hv_LineHeight);
                }
                else {
                    // ����10����ʾ�����У�����ƫ�ƣ�
                    hv_CurrentCol = hv_BaseCol - hv_ColOffset;
                    // ���������е��кţ��ӵ�11�����¼�����
                    HTuple hv_LeftRowIndex = hv_Idx - 10;
                    hv_CurrentRow = hv_StartRow + (hv_LeftRowIndex * hv_LineHeight);
                }
            }

            if (allNgIdx.count(hv_Idx.I()))
            {
                //NG�ú�ɫ����
                SetColor(hHalconID, "red");
            }
            else
            {
                SetColor(hHalconID, "blue");
            }
            // �����ı�λ�ò����
            SetTposition(hHalconID, hv_CurrentRow, hv_CurrentCol);
            if (hv_NumRegions.I() > 20)
            {
                // ����������ʽ
                HTuple hv_Font = "-Arial-35-*-*-*-0-";
                SetFont(hHalconID, hv_Font);
            }
            else
            {
                // ����������ʽ
                HTuple hv_Font = "-Arial-50-*-*-*-0-";
                SetFont(hHalconID, hv_Font);
            }
            WriteString(hHalconID, hv_CoordStr);
        }
    }
    catch (const HalconCpp::HException& e) {
        machineLog->write("Error drawing annotations: " + QString(e.ErrorMessage().Text()), Normal);
    }
}

void pinLabel::DrawTextAllNearRegion_2D3D(HTuple& hHalconID, HObject ho_Region_AllPin, std::vector<double> Height_3D,std::unordered_set<int> allNgIdx)
{
    try {
        // ��ȡ��������
        HTuple hv_NumRegions;
        CountObj(ho_Region_AllPin, &hv_NumRegions);

        // ��ȡͼ��ߴ�
        HTuple hv_Width, hv_Height;
        GetImageSize(ho_showImg, &hv_Width, &hv_Height);

        // ����������ʽ
        HTuple hv_Font = "-Arial-50-*-*-*-0-";
        SetFont(hHalconID, hv_Font);

        // ��һ���֣���ÿ������������ʾ�������֣����ֲ��䣩
        for (HTuple hv_Index = 1; hv_Index <= hv_NumRegions; hv_Index += 1) {
            if (allNgIdx.count(hv_Index.I()))
            {
                //NG�ú�ɫ����
                SetColor(hHalconID, "red");
            }
            else
            {
                SetColor(hHalconID, "blue");
            }
            HObject ho_SingleRegion;
            SelectObj(ho_Region_AllPin, &ho_SingleRegion, hv_Index);

            // ��ȡ������������
            HTuple hv_Row, hv_Column, hv_Area;
            AreaCenter(ho_SingleRegion, &hv_Area, &hv_Row, &hv_Column);

            // ���������ı���������1��ʼ��ʾ��
            HTuple hv_Text = (hv_Index).TupleString("d");

            // �����ı�λ��Ϊ�������ģ������ƫ���������ڵ���
            SetTposition(hHalconID, hv_Row + 15, hv_Column - 15);
            WriteString(hHalconID, hv_Text);
        }

        // �ڶ����֣���̬������ʾ������Ϣ
        HTuple hv_StartRow = hv_Height - 1000;  // ��ʼ�У���ײ�1000���أ�
        HTuple hv_BaseCol = hv_Width - 1500;    // ��׼�У����Ҳ�1000���أ�
        HTuple hv_LineHeight = 70;             // �и�
        HTuple hv_ColOffset = 1500;             // �м�ƫ����

        for (HTuple hv_Idx = 1; hv_Idx <= hv_NumRegions; hv_Idx += 1) {
            // ��ȡ����ֵ
            HTuple xPos = m_message_2D.hv_OffsetX_MM_Array[hv_Idx - 1];
            HTuple yPos = m_message_2D.hv_OffsetY_MM_Array[hv_Idx - 1];
            HTuple hPos = 0;
            if (hv_Idx.I() - 1 < Height_3D.size())
            {
                hPos = m_message_3D.PinHeight[hv_Idx.I() - 1];
            }
            // ��ʽ���ı�
            HTuple hv_CoordStr = hv_Idx.TupleString("d") + ": X offset: " +
                xPos.TupleString(".2f") + ", Y offset:" +
                yPos.TupleString(".2f") + ", Height:" + hPos.TupleString(".2f");


            // ��̬������λ��
            HTuple hv_CurrentCol, hv_CurrentRow;
            HTuple hv_ColWidth = 1000;  // ���磺ÿ�п��300����
            HTuple hv_MaxPerColumn = 15;  // ÿ�������Ŀ��

            if (hv_NumRegions.I() > 20)
            {
                HTuple hv_StartRow = hv_Height - 1200;  // ��ʼ�У���ײ�1000���أ�
                hv_BaseCol = hv_Width - 1000;
                // �����кź��к�
                HTuple hv_Column = (hv_Idx - 1) / hv_MaxPerColumn;         // ��������0,1,2...��
                HTuple hv_RowInColumn = (hv_Idx - 1) % hv_MaxPerColumn;    // ��ǰ���ڵ��кţ�0~19��

                // ����λ��
                hv_CurrentCol = hv_BaseCol - (hv_Column * hv_ColWidth);     // �������и�Ϊ�ӷ��������ü���
                hv_CurrentRow = hv_StartRow + (hv_RowInColumn + 1) * hv_LineHeight; // +1�������д�0��ʼ

            }
            else
            {
                if (hv_Idx <= 10) {
                    // ǰ10����ʾ������
                    hv_CurrentCol = hv_BaseCol;
                    hv_CurrentRow = hv_StartRow + (hv_Idx * hv_LineHeight);
                }
                else {
                    // ����10����ʾ�����У�����ƫ�ƣ�
                    hv_CurrentCol = hv_BaseCol - hv_ColOffset;
                    // ���������е��кţ��ӵ�11�����¼�����
                    HTuple hv_LeftRowIndex = hv_Idx - 10;
                    hv_CurrentRow = hv_StartRow + (hv_LeftRowIndex * hv_LineHeight);
                }
            }
           

            // �����ı�λ�ò����
            SetTposition(hHalconID, hv_CurrentRow, hv_CurrentCol);
            if (allNgIdx.count(hv_Idx.I()))
            {
                //NG�ú�ɫ����
                SetColor(hHalconID, "red");
            }
            else
            {
                SetColor(hHalconID, "blue");
            }

            if (hv_NumRegions.I() > 20)
            {
                // ����������ʽ
                HTuple hv_Font = "-Arial-35-*-*-*-0-";
                SetFont(hHalconID, hv_Font);
            }
            else
            {
                // ����������ʽ
                HTuple hv_Font = "-Arial-50-*-*-*-0-";
                SetFont(hHalconID, hv_Font);
            }
            WriteString(hHalconID, hv_CoordStr);
        }
    }
    catch (const HalconCpp::HException& e) {
        machineLog->write("Error drawing annotations: " + QString(e.ErrorMessage().Text()), Normal);
    }
}

//չʾ�Ӿ������ͼƬ
void pinLabel::showVisionImage(bool isNG)
{
    if (!m_img.IsInitialized()) {
        qWarning() << "Vision Halcon image is not initialized.";
        return;
    }

    try {
        // ȷ��Halcon�����ѳ�ʼ��
        initializeHalconWindow();

        ClearWindow(m_hHalconID);

        HTuple hv_WindowID;
        // ����һ����ʱ�Ĵ���ID��ץȡ��ǰͼ��
        HTuple hv_width, hv_height;
        GetImageSize(m_img, &hv_width, &hv_height);
        OpenWindow(0, 0, hv_width, hv_height, m_hLabelID, "buffer", "", &hv_WindowID);
        SetPart(hv_WindowID, 0, 0, hv_height - 1, hv_width - 1);

        CopyObj(m_img, &ho_showImg, 1, 1);
        DispObj(ho_showImg, hv_WindowID);

        if (isNG)
        {
            SetColor(hv_WindowID, "red");
            HTuple  hv_Font = "-Arial-500-*-*-*-0-";
            SetFont(hv_WindowID, hv_Font);
            // �����Ͻ���ʾ "NG"
            WriteString(hv_WindowID, "NG");
        }
        else
        {
            SetColor(hv_WindowID, "green");
            HTuple  hv_Font = "-Arial-500-*-*-*-0-";  // ���������Arial�����ء���ͨ��ʽ

            // ��������
            SetFont(hv_WindowID, hv_Font);
            WriteString(hv_WindowID, "OK");
        }

        // ����ǰ�������ݱ��浽һ���µ�ͼ�����
        DumpWindowImage(&ho_showImg, hv_WindowID);

        // �ر���ʱ����
        CloseWindow(hv_WindowID);
    }
    catch (const HalconCpp::HException& e) {
        machineLog->write("Vision Error displaying Halcon image or region:" + QString(e.ErrorMessage().Text()), Normal);
    }
}

//չʾ�Ӿ������ͼƬ
void pinLabel::showVisionImage(Vision_Message message)
{
    if (!m_img.IsInitialized()) {
        qWarning() << "Vision Halcon image is not initialized.";
        return;
    }

    try {
        // ȷ��Halcon�����ѳ�ʼ��
        initializeHalconWindow();

        ClearWindow(m_hHalconID);

        HTuple hv_WindowID;
        // ����һ����ʱ�Ĵ���ID��ץȡ��ǰͼ��
        HTuple hv_width, hv_height;
        GetImageSize(m_img, &hv_width, &hv_height);
        OpenWindow(0, 0, hv_width, hv_height, m_hLabelID, "buffer", "", &hv_WindowID);
        SetPart(hv_WindowID, 0, 0, hv_height - 1, hv_width - 1);

        CopyObj(m_img, &ho_showImg, 1, 1);
        DispObj(ho_showImg, hv_WindowID);

        vector<QString> allNGmes;
        //�������������Ϣ
        if (message.BarMes.size() > 0)
        {
            //��һά����Ϣ
            for (int i  = 0;i < message.BarMes.size();i ++)
            {
                auto mes = message.BarMes[i];
                HTuple color;
                QString ngtext;
                if (!mes.first.isNG)
                {
                    //OK
                    // ����������ɫ  
                    SetDraw(hv_WindowID, "margin");
                    SetColor(hv_WindowID, "green");
                    SetLineWidth(hv_WindowID, 5);
                    if(mes.second.ho_RegionBarCode.IsInitialized())
                        DispObj(mes.second.ho_RegionBarCode, hv_WindowID);
                    ngtext = " :OK";

                    if (mes.second.ho_RegionBarCode.IsInitialized())
                    {
                        //���������д�ϱ�ע
                        QString tip = mes.first.Tip + ngtext;
                        // ��ȡ������С��Ӿ�������
                        HTuple hv_Row1, hv_Column1, hv_Row2, hv_Column2;
                        SmallestRectangle1(mes.second.ho_RegionBarCode, &hv_Row1, &hv_Column1, &hv_Row2, &hv_Column2);

                        // �����ı�λ�����������Ϸ�
                        HTuple  hv_Font = "-Arial-50-*-*-*-0-";
                        SetFont(hv_WindowID, hv_Font);
                        SetTposition(hv_WindowID, hv_Row1 - 50, hv_Column2 + 20); // ƫ������
                        WriteString(hv_WindowID, tip.toStdString().c_str());
                    }
                }
                else
                {
                    //NG
                    SetDraw(hv_WindowID, "margin");
                    SetColor(hv_WindowID, "red");
                    SetLineWidth(hv_WindowID, 5);
                    if (mes.second.ho_RegionBarCode.IsInitialized())
                        DispObj(mes.second.ho_RegionBarCode, hv_WindowID);
                    // ����������ɫ  
                    ngtext = " :NG"; 
                    allNGmes.push_back(mes.first.Tip + ngtext);
                }



            }
        }
        if (message.QRMes.size() > 0)
        {
            //�ж�ά����Ϣ
            for (int i = 0; i < message.QRMes.size(); i++)
            {
                auto mes = message.QRMes[i];
                HTuple color;
                QString ngtext;
                if (!mes.first.isNG)
                {
                    //OK
                    // ����������ɫ  
                    SetDraw(hv_WindowID, "margin");
                    SetColor(hv_WindowID, "green");
                    SetLineWidth(hv_WindowID, 5);
                    if (mes.second.ho_RegionCode2d.IsInitialized())
                    {
                        DispObj(mes.second.ho_RegionCode2d, hv_WindowID);
                    }
                    ngtext = " :OK";

                    if (mes.second.ho_RegionCode2d.IsInitialized())
                    {
                        //���������д�ϱ�ע
                        QString tip = mes.first.Tip + ngtext;
                        // ��ȡ������С��Ӿ�������
                        HTuple hv_Row1, hv_Column1, hv_Row2, hv_Column2;
                        SmallestRectangle1(mes.second.ho_RegionCode2d, &hv_Row1, &hv_Column1, &hv_Row2, &hv_Column2);

                        // �����ı�λ�����������Ϸ�
                        HTuple  hv_Font = "-Arial-50-*-*-*-0-";
                        SetFont(hv_WindowID, hv_Font);
                        SetTposition(hv_WindowID, hv_Row1 - 50, hv_Column2 + 20); // ƫ������
                        WriteString(hv_WindowID, tip.toStdString().c_str());
                    }
                }
                else
                {
                    //NG
                    SetDraw(hv_WindowID, "margin");
                    SetColor(hv_WindowID, "red");
                    SetLineWidth(hv_WindowID, 5);
                    if (mes.second.ho_RegionCode2d.IsInitialized())
                    {
                        DispObj(mes.second.ho_RegionCode2d, hv_WindowID);
                    }
                    // ����������ɫ  
                    ngtext = " :NG";
                    allNGmes.push_back(mes.first.Tip + ngtext);
                }


            }
        }
        if (message.TempMes.size() > 0)
        {
            //��ģ��ƥ��ά����Ϣ
            for (int i = 0; i < message.TempMes.size(); i++)
            {
                auto mes = message.TempMes[i];
                HTuple color;
                QString ngtext;
                if (!mes.first.isNG)
                {
                    //OK
                    // ����������ɫ  
                    SetDraw(hv_WindowID, "margin");
                    SetColor(hv_WindowID, "green");
                    SetLineWidth(hv_WindowID, 5);
                    if (mes.second.ho_Region_Template.IsInitialized())
                    {
                        DispObj(mes.second.ho_Region_Template, hv_WindowID);
                    }
                    ngtext = " :OK";

                    if (mes.second.ho_Region_Template.IsInitialized())
                    {
                        //���������д�ϱ�ע
                        QString tip = mes.first.Tip + ngtext;
                        // ��ȡ������С��Ӿ�������
                        HTuple hv_Row1, hv_Column1, hv_Row2, hv_Column2;
                        SmallestRectangle1(mes.second.ho_Region_Template, &hv_Row1, &hv_Column1, &hv_Row2, &hv_Column2);

                        // �����ı�λ�����������Ϸ�
                        HTuple  hv_Font = "-Arial-50-*-*-*-0-";
                        SetFont(hv_WindowID, hv_Font);
                        SetTposition(hv_WindowID, hv_Row1 - 50, hv_Column2 + 20); // ƫ������
                        WriteString(hv_WindowID, tip.toStdString().c_str());
                    }
                }
                else
                {
                    //NG
                    SetDraw(hv_WindowID, "margin");
                    SetColor(hv_WindowID, "red");
                    SetLineWidth(hv_WindowID, 5);
                    //if (mes.second.ho_Region_Template.IsInitialized())
                    //{
                    //    DispObj(mes.second.ho_Region_Template, hv_WindowID);
                    //}
                    // ����������ɫ  
                    ngtext = " :NG";
                    allNGmes.push_back(mes.first.Tip + ngtext);
                }


            }
        }
        if (message.BlobMes.size() > 0)
        {
            //��Blob��Ϣ
            for (int i = 0; i < message.BlobMes.size(); i++)
            {
                auto mes = message.BlobMes[i];
                HTuple color;
                QString ngtext;
                if (!mes.first.isNG)
                {
                    //OK
                    // ����������ɫ  
                    SetDraw(hv_WindowID, "margin");
                    SetColor(hv_WindowID, "green");
                    SetLineWidth(hv_WindowID, 5);
                    if (mes.second.ho_DestRegions.IsInitialized())
                    {
                        DispObj(mes.second.ho_DestRegions, hv_WindowID);
                    }
                    ngtext = " :OK";

                    if (mes.second.ho_DestRegions.IsInitialized())
                    {
                        //���������д�ϱ�ע
                        QString tip = mes.first.Tip + ngtext;
                        // ��ȡ������С��Ӿ�������
                        HTuple hv_Row1, hv_Column1, hv_Row2, hv_Column2;
                        SmallestRectangle1(mes.second.ho_DestRegions, &hv_Row1, &hv_Column1, &hv_Row2, &hv_Column2);

                        // �����ı�λ�����������Ϸ�
                        HTuple  hv_Font = "-Arial-50-*-*-*-0-";
                        SetFont(hv_WindowID, hv_Font);
                        SetTposition(hv_WindowID, hv_Row1 - 50, hv_Column2 + 20); // ƫ������
                        WriteString(hv_WindowID, tip.toStdString().c_str());
                    }
                }
                else
                {
                    //NG
                    SetDraw(hv_WindowID, "margin");
                    SetColor(hv_WindowID, "red");
                    SetLineWidth(hv_WindowID, 5);
                    if (mes.second.ho_DestRegions.IsInitialized())
                    {
                        DispObj(mes.second.ho_DestRegions, hv_WindowID);
                    }
                    // ����������ɫ  
                    ngtext = " :NG";
                    allNGmes.push_back(mes.first.Tip + ngtext);
                }


            }
        }

        // �����Ͻ���ʾ����NG��Ϣ
        if (allNGmes.size() > 0)
        {
            //��NG����
            SetColor(hv_WindowID, "red");
            SetTposition(hv_WindowID, 0, 0); // ��ȷ���������Ͻ�
            HTuple  hv_Font = "-Arial-500-*-*-*-0-";
            SetFont(hv_WindowID, hv_Font);
            // �����Ͻ���ʾ "NG"
            WriteString(hv_WindowID, "NG");

            // 1. ��ȡͼ��ߴ�
            HTuple hv_ImageWidth, hv_ImageHeight;
            GetImageSize(ho_showImg, &hv_ImageWidth, &hv_ImageHeight);

            // 2. ���ù̶�ƫ��ֵ
            HTuple hv_RightMargin = 800;   // �Ҳ�̶�ƫ�������ɵ�����
            HTuple hv_TopStart = 50;        // ������ʼλ�ã��ɵ�����
            HTuple hv_LineSpacing = 90;     // �м�ࣨ�ɵ�����

            // 3. �����������
            hv_Font = "-Arial-80-*-*-*-0-";
            SetFont(hv_WindowID, hv_Font);
            SetColor(hv_WindowID, "red");   // ʹ�ú�ɫͻ����ʾ

            // 4. ������ʼ��λ�ã�ȷ���������߽磩
            HTuple hv_StartCol = hv_ImageWidth - hv_RightMargin;
            if (hv_StartCol < 0) hv_StartCol = 0;

            // 5. ��ʾ����NG��Ϣ
            HTuple hv_CurrentRow = hv_TopStart;
            for (int i = 0; i < allNGmes.size(); i++)
            {
                // �����ı�λ�ã����Ͻǹ̶�ƫ�ƣ�
                SetTposition(hv_WindowID, hv_CurrentRow, hv_StartCol);

                // д���ı�
                WriteString(hv_WindowID, allNGmes[i].toStdString().c_str());

                // �ƶ�����һ��λ��
                hv_CurrentRow += hv_LineSpacing;

                // ����Ƿ񳬳�ͼ��ײ�
                if (hv_CurrentRow > (hv_ImageHeight - 50))
                    break;
            }
        }
        else
        {
            SetColor(hv_WindowID, "green");
            HTuple  hv_Font = "-Arial-500-*-*-*-0-";  // ���������Arial�����ء���ͨ��ʽ
            SetTposition(hv_WindowID, 0, 0); // ��ȷ���������Ͻ�
            // ��������
            SetFont(hv_WindowID, hv_Font);
            WriteString(hv_WindowID, "OK");
        }

        // ����ǰ�������ݱ��浽һ���µ�ͼ�����
        DumpWindowImage(&ho_showImg, hv_WindowID);

        // �ر���ʱ����
        CloseWindow(hv_WindowID);
    }
    catch (const HalconCpp::HException& e) {
        machineLog->write("Vision Error displaying Halcon image or region:" + QString(e.ErrorMessage().Text()), Normal);
    }
    //message.clear();
}

// ��ʾԲ����ͼƬ
void pinLabel::showCircleMeasure(ResultParamsStruct_Circle cir1, ResultParamsStruct_Circle cir2, PointItemConfig config, double dis, bool isNG)
{
    if (!m_img.IsInitialized()) {
        qWarning() << "CircleMeasure Halcon image is not initialized.";
        return;
    }

    try {
        // ȷ��Halcon�����ѳ�ʼ��
        initializeHalconWindow();

        ClearWindow(m_hHalconID);

        HTuple hv_WindowID;
        // ����һ����ʱ�Ĵ���ID��ץȡ��ǰͼ��
        HTuple hv_width, hv_height;
        GetImageSize(m_img, &hv_width, &hv_height);
        OpenWindow(0, 0, hv_width, hv_height, m_hLabelID, "buffer", "", &hv_WindowID);
        SetPart(hv_WindowID, 0, 0, hv_height - 1, hv_width - 1);

        CopyObj(m_img, &ho_showImg, 1, 1);
        DispObj(ho_showImg, hv_WindowID);

        if (isNG)
        {
            SetColor(hv_WindowID, "red");
            HTuple  hv_Font = "-Arial-500-*-*-*-0-";
            SetFont(hv_WindowID, hv_Font);
            // �����Ͻ���ʾ "NG"
            WriteString(hv_WindowID, "Բ���NG");
        }
        else
        {
            SetColor(hv_WindowID, "green");
            HTuple  hv_Font = "-Arial-500-*-*-*-0-";  // ���������Arial�����ء���ͨ��ʽ

            // ��������
            SetFont(hv_WindowID, hv_Font);
            WriteString(hv_WindowID, "Բ���OK"); 
        }
        //������Բ����
        if (cir1.ho_Contour_Circle.IsInitialized())
        {
            SetDraw(hv_WindowID, "margin");
            //SetColor(hv_WindowID, "green");
            SetLineWidth(hv_WindowID, 5);
            DispObj(cir1.ho_Contour_Circle, hv_WindowID);
        }
        if (cir2.ho_Contour_Circle.IsInitialized())
        {
            SetDraw(hv_WindowID, "margin");
            //SetColor(hv_WindowID, "green");
            SetLineWidth(hv_WindowID, 5);
            DispObj(cir2.ho_Contour_Circle, hv_WindowID);
        }

        if (cir1.ho_Contour_Circle.IsInitialized() && cir2.ho_Contour_Circle.IsInitialized())
        {
            //�����ֱ��
            //��һ����
            HTuple cir1RowCenter = cir1.hv_CircleRowCenter;
            HTuple cir1ColumnCenter = cir1.hv_CircleColumnCenter;
            //�ڶ�����
            HTuple cir2RowCenter = cir2.hv_CircleRowCenter;
            HTuple cir2ColumnCenter = cir2.hv_CircleColumnCenter;
            // ����ֱ���е�����
            HTuple hv_MidRow = (cir1RowCenter + cir2RowCenter) / 2.0;
            HTuple hv_MidCol = (cir1ColumnCenter + cir2ColumnCenter) / 2.0;
            // �����ı�λ�ã��е��Ϸ���
            HTuple hv_TextRow = hv_MidRow - 150;
            HTuple hv_TextCol = hv_MidCol + 20;
            // ���ò�೤���ı�λ�����������Ϸ�
            QString tip = "Բ��೤�� �� " + QString::number(dis);
            HTuple  hv_Font = "-Arial-60-*-*-*-0-";
            SetFont(hv_WindowID, hv_Font);
            SetTposition(hv_WindowID, hv_TextRow, hv_TextCol); // ƫ������
            WriteString(hv_WindowID, tip.toStdString().c_str());

            // ����ֱ�߶���
            HObject ho_LineContour;
            GenContourPolygonXld(
                &ho_LineContour,
                HTuple().Append(cir1RowCenter).Append(cir2RowCenter),
                HTuple().Append(cir1ColumnCenter).Append(cir2ColumnCenter)
            );

            // ���û�ͼ��ʽ
            SetDraw(hv_WindowID, "margin");
            //SetColor(hv_WindowID, "green");
            SetLineWidth(hv_WindowID, 3);
            DispObj(ho_LineContour, hv_WindowID);
        }
        // ����ǰ�������ݱ��浽һ���µ�ͼ�����
        DumpWindowImage(&ho_showImg, hv_WindowID);

        // �ر���ʱ����
        CloseWindow(hv_WindowID);
    }
    catch (const HalconCpp::HException& e) {
        machineLog->write("CircleMeasure Error displaying Halcon image or region:" + QString(e.ErrorMessage().Text()), Normal);
    }
}

//ֱ����ʾ����NG��ͼƬ
void pinLabel::showNGimage(HObject photo, PointItemConfig config, bool isNG)
{
    if (!photo.IsInitialized()) {
        qWarning() << "showNGimage Halcon image is not initialized.";
        return;
    }

    try {
        // ȷ��Halcon�����ѳ�ʼ��
        initializeHalconWindow();

        ClearWindow(m_hHalconID);

        HTuple hv_WindowID;
        // ����һ����ʱ�Ĵ���ID��ץȡ��ǰͼ��
        HTuple hv_width, hv_height;
        GetImageSize(photo, &hv_width, &hv_height);
        OpenWindow(0, 0, hv_width, hv_height, m_hLabelID, "buffer", "", &hv_WindowID);
        SetPart(hv_WindowID, 0, 0, hv_height - 1, hv_width - 1);

        CopyObj(photo, &ho_showImg, 1, 1);
        DispObj(ho_showImg, hv_WindowID);

        if (isNG)
        {
            SetColor(hv_WindowID, "red");
            HTuple  hv_Font = "-Arial-500-*-*-*-0-";
            SetFont(hv_WindowID, hv_Font);
            // �����Ͻ���ʾ "NG"
            QString text = config.Tip + " NG";
            WriteString(hv_WindowID, text.toStdString().c_str());
        }
        else
        {
            SetColor(hv_WindowID, "green");
            HTuple  hv_Font = "-Arial-500-*-*-*-0-";  // ���������Arial�����ء���ͨ��ʽ

            // ��������
            SetFont(hv_WindowID, hv_Font);
            // �����Ͻ���ʾ "NG"
            QString text = config.Tip + " OK";
            WriteString(hv_WindowID, text.toStdString().c_str());
        }
        // ����ǰ�������ݱ��浽һ���µ�ͼ�����
        DumpWindowImage(&ho_showImg, hv_WindowID);

        // �ر���ʱ����
        CloseWindow(hv_WindowID);
    }
    catch (const HalconCpp::HException& e) {
        machineLog->write("showNGimage Error displaying Halcon image or region:" + QString(e.ErrorMessage().Text()), Normal);
    }
}