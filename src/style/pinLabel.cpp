#include "pinLabel.h"
#include <QPainter>
#include <QPaintEvent>
#include <QDebug>
#include "QC_Log.h"
#include "rs.h"
#include "rs_motion.h"
using namespace cv;

//定义单步放大倍率
#define ZOOMRATIO 2.0

pinLabel::pinLabel( double in_Width, double in_Height)
    : m_hHalconID(0), m_width(in_Width), m_height(in_Height)
{
    // 确保Halcon窗口已初始化
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
    //CloseWindow(m_hHalconID);  // 关闭Halcon窗口

    if(m_img.IsInitialized())
        m_img.Clear();
    if (ho_showImg.IsInitialized())
        ho_showImg.Clear();
    m_message_2D.clear();
}

//设置allPin图像
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
    //update();  // 更新视图，触发paintEvent重新绘制
}

void pinLabel::initializeHalconWindow()
{
    // 如果还没有创建Halcon窗口，则创建它
    if (m_hHalconID == NULL) {
        //int width = this->width();
        //int height = this->height();

        //m_hLabelID = (Hlong)this->winId();  // 获取QLabel窗口句柄
        m_hLabelID = 0;

        // 创建Halcon窗口并绑定到QLabel控件
        OpenWindow(0, 0, m_width, m_height, m_hLabelID, "buffer", "", &m_hHalconID);
    }
}

//void pinLabel::paintEvent(QPaintEvent* event)
//{
//    QPainter painter(this);
//}


//2D画出NG区域的坐标等
void pinLabel::DrawTextNearRegion(HTuple& hHalconID,HObject ho_Region_NGPin, int index)
{
    try {
        // 确保索引是有效的
        HTuple hv_NumRegions;
        CountObj(ho_Region_NGPin, &hv_NumRegions);
        if (index >= 1 && index <= hv_NumRegions)
        {
            HObject ho_SelectedRegion;

            // 使用 SelectObj 按索引获取指定的区域
            SelectObj(ho_Region_NGPin, &ho_SelectedRegion, index);

            // 获取该区域的重心坐标和面积
            HTuple hv_Row, hv_Column, hv_Area;
            AreaCenter(ho_SelectedRegion, &hv_Area, &hv_Row, &hv_Column);

            // 使用 DispText 显示文字
            HTuple xPos = m_message_2D.hv_OffsetX_Pixel_Array[index];
            HTuple yPos = m_message_2D.hv_OffsetY_Pixel_Array[index];
            HTuple hv_String = "X : " + xPos + " Y : " + yPos;
            //HTuple hv_String = "NG Pin Detected";
            HTuple  hv_Font = "-Arial-50-*-*-*-0-";  // 字体参数：Arial、像素、普通样式
            // 设置字体
            SetFont(hHalconID, hv_Font);
            double a = hv_Row[0].D();
            double b = hv_Column[0].D();

            // 显示文字
            HTuple  hv_TextRow = hv_Row + 20;
            HTuple  hv_TextCol = hv_Column;
            // 调试输出，查看文本显示的行和列
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

//原图裁剪（只选取pin附近）
void pinLabel::CropImageAroundRegion(HObject ho_Region_AllPin, HObject& ho_CroppedImage)
{
    try {
        // 计算所有pin区域的外接矩形
        HTuple hv_Row1, hv_Column1, hv_Row2, hv_Column2;
        // 合并所有pin区域
        HObject ho_UnionRegion;
        Union1(ho_Region_AllPin, &ho_UnionRegion);
        SmallestRectangle1(ho_UnionRegion, &hv_Row1, &hv_Column1, &hv_Row2, &hv_Column2);

        std::cout << "Bounding Box: "
            << "Row1: " << hv_Row1[0].D() << ", "
            << "Column1: " << hv_Column1[0].D() << ", "
            << "Row2: " << hv_Row2[0].D() << ", "
            << "Column2: " << hv_Column2[0].D() << std::endl;

        // 扩展外接矩形，增加200像素
        HTuple hv_Extension = 200;
        hv_Row1 = hv_Row1 - hv_Extension;     // 向上扩展
        hv_Column1 = hv_Column1 - hv_Extension; // 向左扩展
        hv_Row2 = hv_Row2 + hv_Extension;     // 向下扩展
        hv_Column2 = hv_Column2 + hv_Extension; // 向右扩展

        // 调试输出，查看扩展后的矩形
        std::cout << "Extended Bounding Box: "
            << "Row1: " << hv_Row1[0].D() << ", "
            << "Column1: " << hv_Column1[0].D() << ", "
            << "Row2: " << hv_Row2[0].D() << ", "
            << "Column2: " << hv_Column2[0].D() << std::endl;
        // 获取图像尺寸
        HTuple hv_Width, hv_Height;
        GetImageSize(m_img, &hv_Width, &hv_Height);

        // 扩展矩形，并确保坐标不越界
        hv_Row1 = (hv_Row1 < 0) ? HTuple(0) : hv_Row1;
        hv_Column1 = (hv_Column1 < 0) ? HTuple(0) : hv_Column1;

        // 使用图像尺寸限制矩形坐标
        hv_Row2 = (hv_Row2 > hv_Height) ? HTuple(hv_Height) : hv_Row2;
        hv_Column2 = (hv_Column2 > hv_Width) ? HTuple(hv_Width) : hv_Column2;

        // 根据扩展后的外接矩形裁剪原图像
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
        // 确保Halcon窗口已初始化
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
        // 创建一个临时的窗口ID来抓取当前图像
        HTuple hv_width, hv_height;
        GetImageSize(m_img, &hv_width, &hv_height);
        OpenWindow(0, 0, hv_width, hv_height, m_hLabelID, "buffer", "",&hv_WindowID);
        SetPart(hv_WindowID, 0, 0, hv_height - 1, hv_width - 1);

        CopyObj(m_img, &ho_showImg, 1, 1);
        DispObj(ho_showImg, hv_WindowID);

        // 显示 NG 还是 OK
        if (m_message_3D.PinFlag.size() != 0)
        {
            std::vector<int> ngIdx3D;
            for (int i = 0; i < m_message_3D.PinFlag.size(); i++)
            {
                //记录 3D ng点的索引
                if (m_message_3D.PinFlag[i] == false)
                {
                    ngIdx3D.emplace_back(i + 1);
                }
            }
            //2d和3d的ng的所有索引
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
                // 在左上角显示 "NG"
                WriteString(hv_WindowID, "NG");
            }
            else
            {
                //OK
                SetColor(hv_WindowID, "green");
                HTuple  hv_Font = "-Arial-500-*-*-*-0-";
                // 设置字体
                SetFont(hv_WindowID, hv_Font);
                WriteString(hv_WindowID, "OK");
            }

            // 如果有OK对象，绘制区域
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
                ////2D和3D结合显示的NG区域
                //DrawTextNearRegion_2Dand3D(hv_WindowID, m_message_2D.ho_Region_AllPin, idx);

                //250424 坐标直接显示到右下角，不显示在pin针附近，只显示NG针区域
                HObject ho_SelectedRegion;
                SetColor(hv_WindowID, "red");
                SetDraw(hv_WindowID, "margin");
                SetLineWidth(hv_WindowID, 5);
                // 使用 SelectObj 按索引获取指定的区域
                SelectObj(m_message_2D.ho_Region_AllPin, &ho_SelectedRegion, idx);
                DispObj(ho_SelectedRegion, hv_WindowID);
            }

            //标注所有针的XY和height偏移
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
            //纯2D识别显示
            if (m_message_2D.hv_Ret == 1)
            {
                SetColor(hv_WindowID, "green");
                HTuple  hv_Font = "-Arial-500-*-*-*-0-";  // 字体参数：Arial、像素、普通样式

                // 设置字体
                SetFont(hv_WindowID, hv_Font);
                WriteString(hv_WindowID, "OK");

            }
            else
            {
                SetColor(hv_WindowID, "red");
                HTuple  hv_Font = "-Arial-500-*-*-*-0-";
                SetFont(hv_WindowID, hv_Font);
                // 在左上角显示 "NG"
                WriteString(hv_WindowID, "NG");

                // 如果有NG对象，绘制区域
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

            // 如果有OK对象，绘制区域
            if (m_message_2D.ho_Region_OKPin.IsInitialized()) {
                SetDraw(hv_WindowID, "margin");
                SetColor(hv_WindowID, "green");
                SetLineWidth(hv_WindowID, 5);
                DispObj(m_message_2D.ho_Region_OKPin, hv_WindowID);
                //DrawTextNearRegion(hv_WindowID, m_message_2D.ho_Region_OKPin, 3);
            }
            //标注所有针的XY偏移
            SetColor(hv_WindowID, "blue");
            DrawTextAllNearRegion(hv_WindowID, m_message_2D.ho_Region_AllPin,allNgIdx);
        }



        // 将当前窗口内容保存到一个新的图像变量
        DumpWindowImage(&ho_showImg, hv_WindowID);

        // 关闭临时窗口
        CloseWindow(hv_WindowID);
        //仅用作图像合并处理了，不再作为显示模块 Erik 2025/3/21
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
        // 获取 Halcon 图像的宽度和高度
        GetImageSize(ho_image, &hv_Width, &hv_Height);

        // 获取 Halcon 图像数据指针
        GetImagePointer1(ho_image, &hv_Pointer, &hv_Type, &hv_Width, &hv_Height);

        // 判断图像类型，这里假设为 8 位灰度图像（可以根据需要调整）
        if (hv_Type == "byte") {
            // 使用 OpenCV 的 Mat 类来创建图像
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

//Hobject转Mat的格式
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


//2D,3D画出NG区域的坐标等
void pinLabel::DrawTextNearRegion_2Dand3D(HTuple& hHalconID, HObject ho_Region_AllPin, int index)
{
    try {
        // 确保索引是有效的
        HTuple hv_NumRegions;
        CountObj(ho_Region_AllPin, &hv_NumRegions);
        if (index >= 1 && index <= hv_NumRegions)
        {
            HObject ho_SelectedRegion;

            // 使用 SelectObj 按索引获取指定的区域
            SelectObj(ho_Region_AllPin, &ho_SelectedRegion, index);

            // 获取该区域的重心坐标和面积
            HTuple hv_Row, hv_Column, hv_Area;
            AreaCenter(ho_SelectedRegion, &hv_Area, &hv_Row, &hv_Column);

            // 使用 DispText 显示文字
            HTuple xPos = m_message_2D.hv_OffsetX_Pixel_Array[index];
            HTuple yPos = m_message_2D.hv_OffsetY_Pixel_Array[index];
            HTuple hPos = m_message_3D.PinHeight[index];
            HTuple hv_String = "X : " + xPos + " Y : " + yPos + "Height : " + hPos;
            //HTuple hv_String = "NG Pin Detected";
            HTuple  hv_Font = "-Arial-50-*-*-*-0-";  // 字体参数：Arial、像素、普通样式
            // 设置字体
            SetFont(hHalconID, hv_Font);
            double a = hv_Row[0].D();
            double b = hv_Column[0].D();

            // 显示文字
            HTuple  hv_TextRow = hv_Row + 20;
            HTuple  hv_TextCol = hv_Column;
            // 调试输出，查看文本显示的行和列
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

//Mat转HObject
HObject pinLabel::HMatToHObject(cv::Mat image)
{
    //HObject ho_obj = HObject();
    //if (image.empty())
    //    return ho_obj;
    ////3通道
    //if (3 == image.channels())
    //{
    //    int w = image.cols;
    //    int h = image.rows;
    //    //byte位图
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
    //    //深度图real
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
    ////byte位图
    //else if (1 == image.channels())
    //{
    //    int w = image.cols;
    //    int h = image.rows;
    //    //位图
    //    if (image.depth() == CV_8U)
    //    {
    //        uchar* dataGray = new uchar[w * h];
    //        for (int i = 0; i < h; i++)
    //            memcpy(dataGray + w * i, image.ptr() + image.step * i, w);
    //        GenImage1(&ho_obj, "byte", w, h, (Hlong)(dataGray));
    //        delete[] dataGray;
    //    }
    //    //深度图real
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

// 展示3D图片
void pinLabel::show3DImage()
{
    if (!m_img.IsInitialized()) {
        qWarning() << "3D Halcon image is not initialized.";
        return;
    }

    try {
        // 确保Halcon窗口已初始化
        initializeHalconWindow();

        ClearWindow(m_hHalconID);

        HTuple hv_WindowID;
        // 创建一个临时的窗口ID来抓取当前图像
        HTuple hv_width, hv_height;
        GetImageSize(m_img, &hv_width, &hv_height);
        OpenWindow(0, 0, hv_width, hv_height, m_hLabelID, "buffer", "", &hv_WindowID);
        SetPart(hv_WindowID, 0, 0, hv_height - 1, hv_width - 1);

        CopyObj(m_img, &ho_showImg, 1, 1);
        DispObj(ho_showImg, hv_WindowID);
        //旧
        //bool isNG = false;
        //if(m_message_3D.PinFlag.empty())
        //    isNG = true;
        //for (int i = 0; i < m_message_3D.PinFlag.size(); i++)
        //{
        //    machineLog->write("m_message_3D.PinFlag[ " + QString::number(i)+" ] = " + QString::number(m_message_3D.PinFlag[i]), Normal);
        //    // 查找3D是否NG
        //    if (m_message_3D.PinFlag[i] != 1 || m_message_3D.PinHeight[i] == -2)
        //    {
        //        isNG = true;
        //        break;
        //    }
        //}

        //新
        bool isOK = m_message_3D.isOK;
        //isOK = true;
        if (!isOK)
        {
            SetColor(hv_WindowID, "red");
            HTuple  hv_Font = "-Arial-500-*-*-*-0-";
            SetFont(hv_WindowID, hv_Font);
            // 在左上角显示 "NG"
            WriteString(hv_WindowID, "NG");
        }
        else
        {
            SetColor(hv_WindowID, "green");
            HTuple  hv_Font = "-Arial-500-*-*-*-0-";  // 字体参数：Arial、像素、普通样式

            // 设置字体
            SetFont(hv_WindowID, hv_Font);
            WriteString(hv_WindowID, "OK");
        }

        // 将当前窗口内容保存到一个新的图像变量
        DumpWindowImage(&ho_showImg, hv_WindowID);

        // 关闭临时窗口
        CloseWindow(hv_WindowID);
        //仅用作图像合并处理了，不再作为显示模块 Erik 2025/3/21
        //DispObj(ho_showImg, m_hHalconID);
    }
    catch (const HalconCpp::HException& e) {
        machineLog->write("3D Error displaying Halcon image or region:" + QString(e.ErrorMessage().Text()), Normal);
    }
}

void pinLabel::DrawTextAllNearRegion(HTuple& hHalconID, HObject ho_Region_AllPin, std::unordered_set<int> allNgIdx)
{
    try {
        // 获取区域总数
        HTuple hv_NumRegions;
        CountObj(ho_Region_AllPin, &hv_NumRegions);

        // 获取图像尺寸
        HTuple hv_Width, hv_Height;
        GetImageSize(ho_showImg, &hv_Width, &hv_Height);

        // 设置字体样式
        HTuple hv_Font = "-Arial-50-*-*-*-0-";
        SetFont(hHalconID, hv_Font);

        // 第一部分：在每个区域中心显示索引数字（保持不变）
        for (HTuple hv_Index = 1; hv_Index <= hv_NumRegions; hv_Index += 1) {
            if (allNgIdx.count(hv_Index.I()))
            {
                //NG用红色字体
                SetColor(hHalconID, "red");
            }
            else
            {
                SetColor(hHalconID, "blue");
            }
            HObject ho_SingleRegion;
            SelectObj(ho_Region_AllPin, &ho_SingleRegion, hv_Index);

            // 获取区域中心坐标
            HTuple hv_Row, hv_Column, hv_Area;
            AreaCenter(ho_SingleRegion, &hv_Area, &hv_Row, &hv_Column);

            // 生成索引文本（索引从1开始显示）
            HTuple hv_Text = (hv_Index).TupleString("d");

            // 设置文本位置为区域中心（可添加偏移量避免遮挡）
            SetTposition(hHalconID, hv_Row + 15, hv_Column - 15);
            WriteString(hHalconID, hv_Text);
        }

        // 第二部分：动态分列显示坐标信息
        HTuple hv_StartRow = hv_Height - 1000;  // 起始行（距底部1000像素）
        HTuple hv_BaseCol = hv_Width - 1500;    // 基准列（距右侧1500像素）
        HTuple hv_LineHeight = 70;             // 行高
        HTuple hv_ColOffset = 1500;             // 列间偏移量

        for (HTuple hv_Idx = 1; hv_Idx <= hv_NumRegions; hv_Idx += 1) {
            // 获取坐标值
            HTuple xPos = m_message_2D.hv_OffsetX_MM_Array[hv_Idx - 1];
            HTuple yPos = m_message_2D.hv_OffsetY_MM_Array[hv_Idx - 1];
            // 格式化文本
            HTuple hv_CoordStr = hv_Idx.TupleString("d") + ": X offset: " +
                xPos.TupleString(".2f") + ", Y offset:" +
                yPos.TupleString(".2f") + ", Height: 0.0";


            // 动态计算列位置
            HTuple hv_CurrentCol, hv_CurrentRow;
            HTuple hv_ColWidth = 1000;  // 例如：每列宽度300像素
            HTuple hv_MaxPerColumn = 15;  // 每列最大条目数

            if (hv_NumRegions.I() > 20)
            {
                //超过20个针
                HTuple hv_StartRow = hv_Height - 1200;  // 起始行（距底部1000像素）
                hv_BaseCol = hv_Width - 1000;
                // 计算列号和行号
                HTuple hv_Column = (hv_Idx - 1) / hv_MaxPerColumn;         // 列索引（0,1,2...）
                HTuple hv_RowInColumn = (hv_Idx - 1) % hv_MaxPerColumn;    // 当前列内的行号（0~19）

                // 计算位置
                hv_CurrentCol = hv_BaseCol - (hv_Column * hv_ColWidth);     // 向右排列改为加法，向左用减法
                hv_CurrentRow = hv_StartRow + (hv_RowInColumn + 1) * hv_LineHeight; // +1避免首行从0开始

            }
            else
            {
                if (hv_Idx <= 10) {
                    // 前10个显示在右列
                    hv_CurrentCol = hv_BaseCol;
                    hv_CurrentRow = hv_StartRow + (hv_Idx * hv_LineHeight);
                }
                else {
                    // 超过10个显示在左列（右列偏移）
                    hv_CurrentCol = hv_BaseCol - hv_ColOffset;
                    // 计算左列中的行号（从第11个重新计数）
                    HTuple hv_LeftRowIndex = hv_Idx - 10;
                    hv_CurrentRow = hv_StartRow + (hv_LeftRowIndex * hv_LineHeight);
                }
            }

            if (allNgIdx.count(hv_Idx.I()))
            {
                //NG用红色字体
                SetColor(hHalconID, "red");
            }
            else
            {
                SetColor(hHalconID, "blue");
            }
            // 设置文本位置并输出
            SetTposition(hHalconID, hv_CurrentRow, hv_CurrentCol);
            if (hv_NumRegions.I() > 20)
            {
                // 设置字体样式
                HTuple hv_Font = "-Arial-35-*-*-*-0-";
                SetFont(hHalconID, hv_Font);
            }
            else
            {
                // 设置字体样式
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
        // 获取区域总数
        HTuple hv_NumRegions;
        CountObj(ho_Region_AllPin, &hv_NumRegions);

        // 获取图像尺寸
        HTuple hv_Width, hv_Height;
        GetImageSize(ho_showImg, &hv_Width, &hv_Height);

        // 设置字体样式
        HTuple hv_Font = "-Arial-50-*-*-*-0-";
        SetFont(hHalconID, hv_Font);

        // 第一部分：在每个区域中心显示索引数字（保持不变）
        for (HTuple hv_Index = 1; hv_Index <= hv_NumRegions; hv_Index += 1) {
            if (allNgIdx.count(hv_Index.I()))
            {
                //NG用红色字体
                SetColor(hHalconID, "red");
            }
            else
            {
                SetColor(hHalconID, "blue");
            }
            HObject ho_SingleRegion;
            SelectObj(ho_Region_AllPin, &ho_SingleRegion, hv_Index);

            // 获取区域中心坐标
            HTuple hv_Row, hv_Column, hv_Area;
            AreaCenter(ho_SingleRegion, &hv_Area, &hv_Row, &hv_Column);

            // 生成索引文本（索引从1开始显示）
            HTuple hv_Text = (hv_Index).TupleString("d");

            // 设置文本位置为区域中心（可添加偏移量避免遮挡）
            SetTposition(hHalconID, hv_Row + 15, hv_Column - 15);
            WriteString(hHalconID, hv_Text);
        }

        // 第二部分：动态分列显示坐标信息
        HTuple hv_StartRow = hv_Height - 1000;  // 起始行（距底部1000像素）
        HTuple hv_BaseCol = hv_Width - 1500;    // 基准列（距右侧1000像素）
        HTuple hv_LineHeight = 70;             // 行高
        HTuple hv_ColOffset = 1500;             // 列间偏移量

        for (HTuple hv_Idx = 1; hv_Idx <= hv_NumRegions; hv_Idx += 1) {
            // 获取坐标值
            HTuple xPos = m_message_2D.hv_OffsetX_MM_Array[hv_Idx - 1];
            HTuple yPos = m_message_2D.hv_OffsetY_MM_Array[hv_Idx - 1];
            HTuple hPos = 0;
            if (hv_Idx.I() - 1 < Height_3D.size())
            {
                hPos = m_message_3D.PinHeight[hv_Idx.I() - 1];
            }
            // 格式化文本
            HTuple hv_CoordStr = hv_Idx.TupleString("d") + ": X offset: " +
                xPos.TupleString(".2f") + ", Y offset:" +
                yPos.TupleString(".2f") + ", Height:" + hPos.TupleString(".2f");


            // 动态计算列位置
            HTuple hv_CurrentCol, hv_CurrentRow;
            HTuple hv_ColWidth = 1000;  // 例如：每列宽度300像素
            HTuple hv_MaxPerColumn = 15;  // 每列最大条目数

            if (hv_NumRegions.I() > 20)
            {
                HTuple hv_StartRow = hv_Height - 1200;  // 起始行（距底部1000像素）
                hv_BaseCol = hv_Width - 1000;
                // 计算列号和行号
                HTuple hv_Column = (hv_Idx - 1) / hv_MaxPerColumn;         // 列索引（0,1,2...）
                HTuple hv_RowInColumn = (hv_Idx - 1) % hv_MaxPerColumn;    // 当前列内的行号（0~19）

                // 计算位置
                hv_CurrentCol = hv_BaseCol - (hv_Column * hv_ColWidth);     // 向右排列改为加法，向左用减法
                hv_CurrentRow = hv_StartRow + (hv_RowInColumn + 1) * hv_LineHeight; // +1避免首行从0开始

            }
            else
            {
                if (hv_Idx <= 10) {
                    // 前10个显示在右列
                    hv_CurrentCol = hv_BaseCol;
                    hv_CurrentRow = hv_StartRow + (hv_Idx * hv_LineHeight);
                }
                else {
                    // 超过10个显示在左列（右列偏移）
                    hv_CurrentCol = hv_BaseCol - hv_ColOffset;
                    // 计算左列中的行号（从第11个重新计数）
                    HTuple hv_LeftRowIndex = hv_Idx - 10;
                    hv_CurrentRow = hv_StartRow + (hv_LeftRowIndex * hv_LineHeight);
                }
            }
           

            // 设置文本位置并输出
            SetTposition(hHalconID, hv_CurrentRow, hv_CurrentCol);
            if (allNgIdx.count(hv_Idx.I()))
            {
                //NG用红色字体
                SetColor(hHalconID, "red");
            }
            else
            {
                SetColor(hHalconID, "blue");
            }

            if (hv_NumRegions.I() > 20)
            {
                // 设置字体样式
                HTuple hv_Font = "-Arial-35-*-*-*-0-";
                SetFont(hHalconID, hv_Font);
            }
            else
            {
                // 设置字体样式
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

//展示视觉处理的图片
void pinLabel::showVisionImage(bool isNG)
{
    if (!m_img.IsInitialized()) {
        qWarning() << "Vision Halcon image is not initialized.";
        return;
    }

    try {
        // 确保Halcon窗口已初始化
        initializeHalconWindow();

        ClearWindow(m_hHalconID);

        HTuple hv_WindowID;
        // 创建一个临时的窗口ID来抓取当前图像
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
            // 在左上角显示 "NG"
            WriteString(hv_WindowID, "NG");
        }
        else
        {
            SetColor(hv_WindowID, "green");
            HTuple  hv_Font = "-Arial-500-*-*-*-0-";  // 字体参数：Arial、像素、普通样式

            // 设置字体
            SetFont(hv_WindowID, hv_Font);
            WriteString(hv_WindowID, "OK");
        }

        // 将当前窗口内容保存到一个新的图像变量
        DumpWindowImage(&ho_showImg, hv_WindowID);

        // 关闭临时窗口
        CloseWindow(hv_WindowID);
    }
    catch (const HalconCpp::HException& e) {
        machineLog->write("Vision Error displaying Halcon image or region:" + QString(e.ErrorMessage().Text()), Normal);
    }
}

//展示视觉处理的图片
void pinLabel::showVisionImage(Vision_Message message)
{
    if (!m_img.IsInitialized()) {
        qWarning() << "Vision Halcon image is not initialized.";
        return;
    }

    try {
        // 确保Halcon窗口已初始化
        initializeHalconWindow();

        ClearWindow(m_hHalconID);

        HTuple hv_WindowID;
        // 创建一个临时的窗口ID来抓取当前图像
        HTuple hv_width, hv_height;
        GetImageSize(m_img, &hv_width, &hv_height);
        OpenWindow(0, 0, hv_width, hv_height, m_hLabelID, "buffer", "", &hv_WindowID);
        SetPart(hv_WindowID, 0, 0, hv_height - 1, hv_width - 1);

        CopyObj(m_img, &ho_showImg, 1, 1);
        DispObj(ho_showImg, hv_WindowID);

        vector<QString> allNGmes;
        //填充所有区域及信息
        if (message.BarMes.size() > 0)
        {
            //有一维码信息
            for (int i  = 0;i < message.BarMes.size();i ++)
            {
                auto mes = message.BarMes[i];
                HTuple color;
                QString ngtext;
                if (!mes.first.isNG)
                {
                    //OK
                    // 覆盖区域颜色  
                    SetDraw(hv_WindowID, "margin");
                    SetColor(hv_WindowID, "green");
                    SetLineWidth(hv_WindowID, 5);
                    if(mes.second.ho_RegionBarCode.IsInitialized())
                        DispObj(mes.second.ho_RegionBarCode, hv_WindowID);
                    ngtext = " :OK";

                    if (mes.second.ho_RegionBarCode.IsInitialized())
                    {
                        //在区域边上写上备注
                        QString tip = mes.first.Tip + ngtext;
                        // 获取区域最小外接矩形坐标
                        HTuple hv_Row1, hv_Column1, hv_Row2, hv_Column2;
                        SmallestRectangle1(mes.second.ho_RegionBarCode, &hv_Row1, &hv_Column1, &hv_Row2, &hv_Column2);

                        // 设置文本位置在区域右上方
                        HTuple  hv_Font = "-Arial-50-*-*-*-0-";
                        SetFont(hv_WindowID, hv_Font);
                        SetTposition(hv_WindowID, hv_Row1 - 50, hv_Column2 + 20); // 偏移坐标
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
                    // 覆盖区域颜色  
                    ngtext = " :NG"; 
                    allNGmes.push_back(mes.first.Tip + ngtext);
                }



            }
        }
        if (message.QRMes.size() > 0)
        {
            //有二维码信息
            for (int i = 0; i < message.QRMes.size(); i++)
            {
                auto mes = message.QRMes[i];
                HTuple color;
                QString ngtext;
                if (!mes.first.isNG)
                {
                    //OK
                    // 覆盖区域颜色  
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
                        //在区域边上写上备注
                        QString tip = mes.first.Tip + ngtext;
                        // 获取区域最小外接矩形坐标
                        HTuple hv_Row1, hv_Column1, hv_Row2, hv_Column2;
                        SmallestRectangle1(mes.second.ho_RegionCode2d, &hv_Row1, &hv_Column1, &hv_Row2, &hv_Column2);

                        // 设置文本位置在区域右上方
                        HTuple  hv_Font = "-Arial-50-*-*-*-0-";
                        SetFont(hv_WindowID, hv_Font);
                        SetTposition(hv_WindowID, hv_Row1 - 50, hv_Column2 + 20); // 偏移坐标
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
                    // 覆盖区域颜色  
                    ngtext = " :NG";
                    allNGmes.push_back(mes.first.Tip + ngtext);
                }


            }
        }
        if (message.TempMes.size() > 0)
        {
            //有模板匹配维码信息
            for (int i = 0; i < message.TempMes.size(); i++)
            {
                auto mes = message.TempMes[i];
                HTuple color;
                QString ngtext;
                if (!mes.first.isNG)
                {
                    //OK
                    // 覆盖区域颜色  
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
                        //在区域边上写上备注
                        QString tip = mes.first.Tip + ngtext;
                        // 获取区域最小外接矩形坐标
                        HTuple hv_Row1, hv_Column1, hv_Row2, hv_Column2;
                        SmallestRectangle1(mes.second.ho_Region_Template, &hv_Row1, &hv_Column1, &hv_Row2, &hv_Column2);

                        // 设置文本位置在区域右上方
                        HTuple  hv_Font = "-Arial-50-*-*-*-0-";
                        SetFont(hv_WindowID, hv_Font);
                        SetTposition(hv_WindowID, hv_Row1 - 50, hv_Column2 + 20); // 偏移坐标
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
                    // 覆盖区域颜色  
                    ngtext = " :NG";
                    allNGmes.push_back(mes.first.Tip + ngtext);
                }


            }
        }
        if (message.BlobMes.size() > 0)
        {
            //有Blob信息
            for (int i = 0; i < message.BlobMes.size(); i++)
            {
                auto mes = message.BlobMes[i];
                HTuple color;
                QString ngtext;
                if (!mes.first.isNG)
                {
                    //OK
                    // 覆盖区域颜色  
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
                        //在区域边上写上备注
                        QString tip = mes.first.Tip + ngtext;
                        // 获取区域最小外接矩形坐标
                        HTuple hv_Row1, hv_Column1, hv_Row2, hv_Column2;
                        SmallestRectangle1(mes.second.ho_DestRegions, &hv_Row1, &hv_Column1, &hv_Row2, &hv_Column2);

                        // 设置文本位置在区域右上方
                        HTuple  hv_Font = "-Arial-50-*-*-*-0-";
                        SetFont(hv_WindowID, hv_Font);
                        SetTposition(hv_WindowID, hv_Row1 - 50, hv_Column2 + 20); // 偏移坐标
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
                    // 覆盖区域颜色  
                    ngtext = " :NG";
                    allNGmes.push_back(mes.first.Tip + ngtext);
                }


            }
        }

        // 在右上角显示所有NG信息
        if (allNGmes.size() > 0)
        {
            //有NG出现
            SetColor(hv_WindowID, "red");
            SetTposition(hv_WindowID, 0, 0); // 明确设置在左上角
            HTuple  hv_Font = "-Arial-500-*-*-*-0-";
            SetFont(hv_WindowID, hv_Font);
            // 在左上角显示 "NG"
            WriteString(hv_WindowID, "NG");

            // 1. 获取图像尺寸
            HTuple hv_ImageWidth, hv_ImageHeight;
            GetImageSize(ho_showImg, &hv_ImageWidth, &hv_ImageHeight);

            // 2. 设置固定偏移值
            HTuple hv_RightMargin = 800;   // 右侧固定偏移量（可调整）
            HTuple hv_TopStart = 50;        // 顶部起始位置（可调整）
            HTuple hv_LineSpacing = 90;     // 行间距（可调整）

            // 3. 设置字体参数
            hv_Font = "-Arial-80-*-*-*-0-";
            SetFont(hv_WindowID, hv_Font);
            SetColor(hv_WindowID, "red");   // 使用红色突出显示

            // 4. 计算起始列位置（确保不超出边界）
            HTuple hv_StartCol = hv_ImageWidth - hv_RightMargin;
            if (hv_StartCol < 0) hv_StartCol = 0;

            // 5. 显示所有NG信息
            HTuple hv_CurrentRow = hv_TopStart;
            for (int i = 0; i < allNGmes.size(); i++)
            {
                // 设置文本位置（右上角固定偏移）
                SetTposition(hv_WindowID, hv_CurrentRow, hv_StartCol);

                // 写入文本
                WriteString(hv_WindowID, allNGmes[i].toStdString().c_str());

                // 移动到下一行位置
                hv_CurrentRow += hv_LineSpacing;

                // 检查是否超出图像底部
                if (hv_CurrentRow > (hv_ImageHeight - 50))
                    break;
            }
        }
        else
        {
            SetColor(hv_WindowID, "green");
            HTuple  hv_Font = "-Arial-500-*-*-*-0-";  // 字体参数：Arial、像素、普通样式
            SetTposition(hv_WindowID, 0, 0); // 明确设置在左上角
            // 设置字体
            SetFont(hv_WindowID, hv_Font);
            WriteString(hv_WindowID, "OK");
        }

        // 将当前窗口内容保存到一个新的图像变量
        DumpWindowImage(&ho_showImg, hv_WindowID);

        // 关闭临时窗口
        CloseWindow(hv_WindowID);
    }
    catch (const HalconCpp::HException& e) {
        machineLog->write("Vision Error displaying Halcon image or region:" + QString(e.ErrorMessage().Text()), Normal);
    }
    //message.clear();
}

// 显示圆测距的图片
void pinLabel::showCircleMeasure(ResultParamsStruct_Circle cir1, ResultParamsStruct_Circle cir2, PointItemConfig config, double dis, bool isNG)
{
    if (!m_img.IsInitialized()) {
        qWarning() << "CircleMeasure Halcon image is not initialized.";
        return;
    }

    try {
        // 确保Halcon窗口已初始化
        initializeHalconWindow();

        ClearWindow(m_hHalconID);

        HTuple hv_WindowID;
        // 创建一个临时的窗口ID来抓取当前图像
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
            // 在左上角显示 "NG"
            WriteString(hv_WindowID, "圆测距NG");
        }
        else
        {
            SetColor(hv_WindowID, "green");
            HTuple  hv_Font = "-Arial-500-*-*-*-0-";  // 字体参数：Arial、像素、普通样式

            // 设置字体
            SetFont(hv_WindowID, hv_Font);
            WriteString(hv_WindowID, "圆测距OK"); 
        }
        //画两个圆轮廓
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
            //画测距直线
            //第一个点
            HTuple cir1RowCenter = cir1.hv_CircleRowCenter;
            HTuple cir1ColumnCenter = cir1.hv_CircleColumnCenter;
            //第二个点
            HTuple cir2RowCenter = cir2.hv_CircleRowCenter;
            HTuple cir2ColumnCenter = cir2.hv_CircleColumnCenter;
            // 计算直线中点坐标
            HTuple hv_MidRow = (cir1RowCenter + cir2RowCenter) / 2.0;
            HTuple hv_MidCol = (cir1ColumnCenter + cir2ColumnCenter) / 2.0;
            // 设置文本位置（中点上方）
            HTuple hv_TextRow = hv_MidRow - 150;
            HTuple hv_TextCol = hv_MidCol + 20;
            // 设置测距长度文本位置在区域右上方
            QString tip = "圆测距长度 ： " + QString::number(dis);
            HTuple  hv_Font = "-Arial-60-*-*-*-0-";
            SetFont(hv_WindowID, hv_Font);
            SetTposition(hv_WindowID, hv_TextRow, hv_TextCol); // 偏移坐标
            WriteString(hv_WindowID, tip.toStdString().c_str());

            // 生成直线对象
            HObject ho_LineContour;
            GenContourPolygonXld(
                &ho_LineContour,
                HTuple().Append(cir1RowCenter).Append(cir2RowCenter),
                HTuple().Append(cir1ColumnCenter).Append(cir2ColumnCenter)
            );

            // 设置绘图样式
            SetDraw(hv_WindowID, "margin");
            //SetColor(hv_WindowID, "green");
            SetLineWidth(hv_WindowID, 3);
            DispObj(ho_LineContour, hv_WindowID);
        }
        // 将当前窗口内容保存到一个新的图像变量
        DumpWindowImage(&ho_showImg, hv_WindowID);

        // 关闭临时窗口
        CloseWindow(hv_WindowID);
    }
    catch (const HalconCpp::HException& e) {
        machineLog->write("CircleMeasure Error displaying Halcon image or region:" + QString(e.ErrorMessage().Text()), Normal);
    }
}

//直接显示报错NG的图片
void pinLabel::showNGimage(HObject photo, PointItemConfig config, bool isNG)
{
    if (!photo.IsInitialized()) {
        qWarning() << "showNGimage Halcon image is not initialized.";
        return;
    }

    try {
        // 确保Halcon窗口已初始化
        initializeHalconWindow();

        ClearWindow(m_hHalconID);

        HTuple hv_WindowID;
        // 创建一个临时的窗口ID来抓取当前图像
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
            // 在左上角显示 "NG"
            QString text = config.Tip + " NG";
            WriteString(hv_WindowID, text.toStdString().c_str());
        }
        else
        {
            SetColor(hv_WindowID, "green");
            HTuple  hv_Font = "-Arial-500-*-*-*-0-";  // 字体参数：Arial、像素、普通样式

            // 设置字体
            SetFont(hv_WindowID, hv_Font);
            // 在左上角显示 "NG"
            QString text = config.Tip + " OK";
            WriteString(hv_WindowID, text.toStdString().c_str());
        }
        // 将当前窗口内容保存到一个新的图像变量
        DumpWindowImage(&ho_showImg, hv_WindowID);

        // 关闭临时窗口
        CloseWindow(hv_WindowID);
    }
    catch (const HalconCpp::HException& e) {
        machineLog->write("showNGimage Error displaying Halcon image or region:" + QString(e.ErrorMessage().Text()), Normal);
    }
}