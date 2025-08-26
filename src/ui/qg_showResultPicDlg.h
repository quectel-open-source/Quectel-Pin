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

    // 动态显示图片的接口
    void showImage(const QImage& image);
    // 清空所有图片
    void clearImages();
    // 加载并显示指定路径下的所有 BMP 图片
    void loadAndShowBmpImages(const QString& imageDirPath);

private:
    Ui::qg_showResultPicDlgClass ui;

    QGridLayout* gridLayout = nullptr;  // 网格布局
    int currentRow = 0;                 // 当前行
    int currentCol = 0;                 // 当前列
    const int maxColumns = 3;           // 每行最大列数

private:
    // 辅助函数：验证路径有效性
    bool validateImagePath(const QString& path);
};