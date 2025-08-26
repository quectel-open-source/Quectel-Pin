#pragma once

#include <QDialog>
#include "ui_qg_showResultPicDlg.h"
#include <QGridLayout>
#include "QGraphicsViews.h"  

class qg_showResultPicDlg : public QDialog {
    Q_OBJECT
public:
    explicit qg_showResultPicDlg(QWidget* parent = nullptr);
    ~qg_showResultPicDlg();

    // ��̬��ʾͼƬ�Ľӿ�
    void showImage(const QImage& image);
    // �������ͼƬ
    void clearImages();
    // ���ز���ʾָ��·���µ����� BMP ͼƬ
    void loadAndShowBmpImages(const QString& imageDirPath);

private:
    Ui::qg_showResultPicDlgClass ui;

    QGridLayout* gridLayout = nullptr;  // ���񲼾�
    int currentRow = 0;                 // ��ǰ��
    int currentCol = 0;                 // ��ǰ��
    const int maxColumns = 3;           // ÿ���������

private:
    // ������������֤·����Ч��
    bool validateImagePath(const QString& path);
};