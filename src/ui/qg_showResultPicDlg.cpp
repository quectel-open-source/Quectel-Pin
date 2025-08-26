// qg_showResultPicDlg.cpp
#include "qg_showResultPicDlg.h"
#include <QFileInfo>
#include "qc_log.h"
#include <QDir>
#include <QImageReader>

qg_showResultPicDlg::qg_showResultPicDlg(QWidget* parent)
    : QDialog(parent){
    ui.setupUi(this);

    // 初始化滚动区域属性
    ui.scrollArea_ShowPic->setWidgetResizable(true);
    ui.scrollAreaWidgetContents_ShowPic->setSizePolicy(
        QSizePolicy::Expanding, QSizePolicy::Expanding);

    setWindowFlags(Qt::Dialog | Qt::WindowMinimizeButtonHint | Qt::WindowMaximizeButtonHint | Qt::WindowCloseButtonHint);
}

qg_showResultPicDlg::~qg_showResultPicDlg() {
    clearImages(); // 确保清理资源
}

// 显示图片到网格布局
void qg_showResultPicDlg::showImage(const QImage& image) {
    // 创建图像视图控件
    QGraphicsViews* graphicsView = new QGraphicsViews(ui.scrollAreaWidgetContents_ShowPic);
    graphicsView->DispImage(image);

    // 初始化布局（延迟初始化）
    if (!gridLayout) {
        // 删除旧布局（如果有）
        QLayout* oldLayout = ui.scrollAreaWidgetContents_ShowPic->layout();
        if (oldLayout) {
            delete oldLayout;
        }

        // 创建新网格布局
        gridLayout = new QGridLayout(ui.scrollAreaWidgetContents_ShowPic);
        gridLayout->setContentsMargins(10, 10, 10, 10);
        gridLayout->setHorizontalSpacing(15);
        gridLayout->setVerticalSpacing(15);

        // 均匀分配列宽
        for (int c = 0; c < maxColumns; c++) {
            gridLayout->setColumnStretch(c, 1);
        }

        ui.scrollAreaWidgetContents_ShowPic->setLayout(gridLayout);
    }

    // 添加控件到布局
    gridLayout->addWidget(graphicsView, currentRow, currentCol);

    // 更新行列索引
    currentCol++;
    if (currentCol >= maxColumns) {
        currentCol = 0;
        currentRow++;
    }
}

// 清空所有图片
void qg_showResultPicDlg::clearImages() {
    // 删除所有子控件
    QLayoutItem* child;
    if (gridLayout == nullptr)
        return;
    while ((child = gridLayout->takeAt(0)) != nullptr) {
        delete child->widget();
        delete child;
    }

    // 重置布局状态
    currentRow = 0;
    currentCol = 0;
    gridLayout = nullptr; // 下次调用 showImage 会重新初始化
}

void qg_showResultPicDlg::loadAndShowBmpImages(const QString& imageDirPath) {
    // 步骤 1: 验证路径有效性
    if (!validateImagePath(imageDirPath)) {
        machineLog->write("无效的图片路径: " + imageDirPath, Normal);
        return;
    }

    // 步骤 2: 获取目录下所有 BMP 文件
    QDir dir(imageDirPath);
    QStringList bmpFiles = dir.entryList(
        { "*.jpg", "*.JPG" },
        QDir::Files | QDir::Readable,
        QDir::Name
    );

    if (bmpFiles.isEmpty()) {
        machineLog->write("目录中没有找到 JPG 文件: " + imageDirPath, Normal);
        return;
    }

    //// 步骤 3: 遍历并加载图片
    //foreach(const QString & fileName, bmpFiles) {
    //    QString filePath = dir.absoluteFilePath(fileName);

    //    QImageReader reader(filePath);
    //    reader.setAutoTransform(true);

    //    if (!reader.canRead()) {
    //        machineLog->write(
    //            QString("无法读取图片: %1, 错误: %2").arg(filePath).arg(reader.errorString()),
    //            Normal
    //        );
    //        continue;
    //    }

    //    QImage image = reader.read();
    //    if (image.isNull()) {
    //        machineLog->write(
    //            QString("图片加载失败: %1, 错误: %2").arg(filePath).arg(reader.errorString()),
    //            Normal
    //        );
    //        continue;
    //    }

    //    // 步骤 4: 显示到界面
    //    //showImage(image);
    //}

    // 步骤 3: 遍历并加载图片
    QVector<QGraphicsViews*> graphicsViews; // 存储所有创建的视图控件

    foreach(const QString& fileName, bmpFiles) {
        QString filePath = dir.absoluteFilePath(fileName);

        QImageReader reader(filePath);
        reader.setAutoTransform(true);

        if (!reader.canRead()) {
            machineLog->write(
                QString("无法读取图片: %1, 错误: %2").arg(filePath).arg(reader.errorString()),
                Normal
            );
            continue;
        }

        QImage image = reader.read();
        if (image.isNull()) {
            machineLog->write(
                QString("图片加载失败: %1, 错误: %2").arg(filePath).arg(reader.errorString()),
                Normal
            );
            continue;
        }

        // 创建并存储图像视图控件（不立即添加到布局）
        QGraphicsViews* view = new QGraphicsViews(ui.scrollAreaWidgetContents_ShowPic);
        view->DispImage(image);
        graphicsViews.append(view);
    }

    // 步骤 4: 初始化并配置布局
    if (graphicsViews.size() > 0) {
        // 删除旧布局（如果有）
        QLayout* oldLayout = ui.scrollAreaWidgetContents_ShowPic->layout();
        if (oldLayout) {
            delete oldLayout;
        }

        // 创建新网格布局
        QGridLayout* gridLayout = new QGridLayout(ui.scrollAreaWidgetContents_ShowPic);
        gridLayout->setContentsMargins(10, 10, 10, 10);
        gridLayout->setHorizontalSpacing(15);
        gridLayout->setVerticalSpacing(15);

        // 根据图片数量决定列数
        int maxColumns = (graphicsViews.size() > 3) ? 3 : graphicsViews.size();

        // 设置列宽均匀分配
        for (int c = 0; c < maxColumns; c++) {
            gridLayout->setColumnStretch(c, 1);
        }

        // 将控件添加到布局
        int currentCol = 0;
        int currentRow = 0;
        for (int i = 0; i < graphicsViews.size(); i++) {
            gridLayout->addWidget(graphicsViews[i], currentRow, currentCol);

            // 更新行列索引
            currentCol++;
            if (currentCol >= maxColumns) {
                currentCol = 0;
                currentRow++;
            }
        }

        ui.scrollAreaWidgetContents_ShowPic->setLayout(gridLayout);
    }
}

bool qg_showResultPicDlg::validateImagePath(const QString& path) {
    QFileInfo info(path);

    if (!info.exists()) {
        machineLog->write("路径不存在: " + path, Normal);
        return false;
    }

    if (!info.isDir()) {
        machineLog->write("路径不是目录: " + path, Normal);
        return false;
    }

    if (!info.isReadable()) {
        machineLog->write("目录不可读: " + path, Normal);
        return false;
    }

    return true;
}