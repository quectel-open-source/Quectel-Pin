// qg_showResultPicDlg.cpp
#include "qg_showResultPicDlg.h"
#include <QFileInfo>
#include "qc_log.h"
#include <QDir>
#include <QImageReader>

qg_showResultPicDlg::qg_showResultPicDlg(QWidget* parent)
    : QDialog(parent){
    ui.setupUi(this);

    // ��ʼ��������������
    ui.scrollArea_ShowPic->setWidgetResizable(true);
    ui.scrollAreaWidgetContents_ShowPic->setSizePolicy(
        QSizePolicy::Expanding, QSizePolicy::Expanding);

    setWindowFlags(Qt::Dialog | Qt::WindowMinimizeButtonHint | Qt::WindowMaximizeButtonHint | Qt::WindowCloseButtonHint);
}

qg_showResultPicDlg::~qg_showResultPicDlg() {
    clearImages(); // ȷ��������Դ
}

// ��ʾͼƬ�����񲼾�
void qg_showResultPicDlg::showImage(const QImage& image) {
    // ����ͼ����ͼ�ؼ�
    QGraphicsViews* graphicsView = new QGraphicsViews(ui.scrollAreaWidgetContents_ShowPic);
    graphicsView->DispImage(image);

    // ��ʼ�����֣��ӳٳ�ʼ����
    if (!gridLayout) {
        // ɾ���ɲ��֣�����У�
        QLayout* oldLayout = ui.scrollAreaWidgetContents_ShowPic->layout();
        if (oldLayout) {
            delete oldLayout;
        }

        // ���������񲼾�
        gridLayout = new QGridLayout(ui.scrollAreaWidgetContents_ShowPic);
        gridLayout->setContentsMargins(10, 10, 10, 10);
        gridLayout->setHorizontalSpacing(15);
        gridLayout->setVerticalSpacing(15);

        // ���ȷ����п�
        for (int c = 0; c < maxColumns; c++) {
            gridLayout->setColumnStretch(c, 1);
        }

        ui.scrollAreaWidgetContents_ShowPic->setLayout(gridLayout);
    }

    // ��ӿؼ�������
    gridLayout->addWidget(graphicsView, currentRow, currentCol);

    // ������������
    currentCol++;
    if (currentCol >= maxColumns) {
        currentCol = 0;
        currentRow++;
    }
}

// �������ͼƬ
void qg_showResultPicDlg::clearImages() {
    // ɾ�������ӿؼ�
    QLayoutItem* child;
    if (gridLayout == nullptr)
        return;
    while ((child = gridLayout->takeAt(0)) != nullptr) {
        delete child->widget();
        delete child;
    }

    // ���ò���״̬
    currentRow = 0;
    currentCol = 0;
    gridLayout = nullptr; // �´ε��� showImage �����³�ʼ��
}

void qg_showResultPicDlg::loadAndShowBmpImages(const QString& imageDirPath) {
    // ���� 1: ��֤·����Ч��
    if (!validateImagePath(imageDirPath)) {
        machineLog->write("��Ч��ͼƬ·��: " + imageDirPath, Normal);
        return;
    }

    // ���� 2: ��ȡĿ¼������ BMP �ļ�
    QDir dir(imageDirPath);
    QStringList bmpFiles = dir.entryList(
        { "*.jpg", "*.JPG" },
        QDir::Files | QDir::Readable,
        QDir::Name
    );

    if (bmpFiles.isEmpty()) {
        machineLog->write("Ŀ¼��û���ҵ� JPG �ļ�: " + imageDirPath, Normal);
        return;
    }

    //// ���� 3: ����������ͼƬ
    //foreach(const QString & fileName, bmpFiles) {
    //    QString filePath = dir.absoluteFilePath(fileName);

    //    QImageReader reader(filePath);
    //    reader.setAutoTransform(true);

    //    if (!reader.canRead()) {
    //        machineLog->write(
    //            QString("�޷���ȡͼƬ: %1, ����: %2").arg(filePath).arg(reader.errorString()),
    //            Normal
    //        );
    //        continue;
    //    }

    //    QImage image = reader.read();
    //    if (image.isNull()) {
    //        machineLog->write(
    //            QString("ͼƬ����ʧ��: %1, ����: %2").arg(filePath).arg(reader.errorString()),
    //            Normal
    //        );
    //        continue;
    //    }

    //    // ���� 4: ��ʾ������
    //    //showImage(image);
    //}

    // ���� 3: ����������ͼƬ
    QVector<QGraphicsViews*> graphicsViews; // �洢���д�������ͼ�ؼ�

    foreach(const QString& fileName, bmpFiles) {
        QString filePath = dir.absoluteFilePath(fileName);

        QImageReader reader(filePath);
        reader.setAutoTransform(true);

        if (!reader.canRead()) {
            machineLog->write(
                QString("�޷���ȡͼƬ: %1, ����: %2").arg(filePath).arg(reader.errorString()),
                Normal
            );
            continue;
        }

        QImage image = reader.read();
        if (image.isNull()) {
            machineLog->write(
                QString("ͼƬ����ʧ��: %1, ����: %2").arg(filePath).arg(reader.errorString()),
                Normal
            );
            continue;
        }

        // �������洢ͼ����ͼ�ؼ�����������ӵ����֣�
        QGraphicsViews* view = new QGraphicsViews(ui.scrollAreaWidgetContents_ShowPic);
        view->DispImage(image);
        graphicsViews.append(view);
    }

    // ���� 4: ��ʼ�������ò���
    if (graphicsViews.size() > 0) {
        // ɾ���ɲ��֣�����У�
        QLayout* oldLayout = ui.scrollAreaWidgetContents_ShowPic->layout();
        if (oldLayout) {
            delete oldLayout;
        }

        // ���������񲼾�
        QGridLayout* gridLayout = new QGridLayout(ui.scrollAreaWidgetContents_ShowPic);
        gridLayout->setContentsMargins(10, 10, 10, 10);
        gridLayout->setHorizontalSpacing(15);
        gridLayout->setVerticalSpacing(15);

        // ����ͼƬ������������
        int maxColumns = (graphicsViews.size() > 3) ? 3 : graphicsViews.size();

        // �����п���ȷ���
        for (int c = 0; c < maxColumns; c++) {
            gridLayout->setColumnStretch(c, 1);
        }

        // ���ؼ���ӵ�����
        int currentCol = 0;
        int currentRow = 0;
        for (int i = 0; i < graphicsViews.size(); i++) {
            gridLayout->addWidget(graphicsViews[i], currentRow, currentCol);

            // ������������
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
        machineLog->write("·��������: " + path, Normal);
        return false;
    }

    if (!info.isDir()) {
        machineLog->write("·������Ŀ¼: " + path, Normal);
        return false;
    }

    if (!info.isReadable()) {
        machineLog->write("Ŀ¼���ɶ�: " + path, Normal);
        return false;
    }

    return true;
}