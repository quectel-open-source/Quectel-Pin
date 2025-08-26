#ifndef QUIWIDGET_H
#define QUIWIDGET_H

#include <QDialog>
#include <QWidget>
#include <QMutex>
#include "qlabel.h"
#include "qpushbutton.h"
#include "qboxlayout.h"
#include "qfile.h"
#include "qevent.h"
#include "qapplication.h"

#include <QGuiApplication>
#include <QScreen>

class QUIWidget : public QDialog
{
    Q_OBJECT

public:
    //将部分对象作为枚举值暴露给外部
    enum Widget {
        Lab_Ico = 0,                //左上角图标
        BtnMenu_Min = 1,            //最小化按钮
        BtnMenu_Max = 2,            //最大化按钮
        BtnMenu_Close = 3           //关闭按钮
    };

    explicit QUIWidget(QWidget *parent = 0);
    ~QUIWidget();

public:
    //设置全局样式
    static void setStyle(const QString &qssFile);
    //设置窗体居中显示 可传入别的窗口
    static void setFormInCenter(QWidget *frm);

protected:
    bool eventFilter(QObject *obj, QEvent *evt);

private:
    QVBoxLayout *verticalLayout1;
    QWidget *widgetMain;
    QVBoxLayout *verticalLayout2;
    QWidget *widget_title;
    QHBoxLayout *horizontalLayout4;
    QLabel *lab_Ico;
    QLabel *lab_Title;
    QWidget *widget_menu;
    QHBoxLayout *horizontalLayout;
    QPushButton *btnMenu_Min;
    QPushButton *btnMenu_Max;
    QPushButton *btnMenu_Close;
    QWidget *widget;
    QVBoxLayout *verticalLayout3;

private:        
    bool max;                       //是否处于最大化状态
    QRect location;                 //鼠标移动窗体后的坐标位置
    QWidget *mainWidget;            //主窗体对象

    QString title;                  //标题
    Qt::Alignment alignment;        //标题文本对齐

public:
    QString getTitle()              const;
    Qt::Alignment getAlignment()    const;
    QSize sizeHint()                const;
    QSize minimumSizeHint()         const;

private slots:
    void initControl();             //初始化控件
    void initForm();                //初始化窗体

private slots:
    void on_btnMenu_Min_clicked();
    void on_btnMenu_Max_clicked();
    void on_btnMenu_Close_clicked();

public Q_SLOTS:
    //设置部件图标
    void setPixmap(QUIWidget::Widget widget, const QString &file, const QSize &size = QSize(16, 16));
    //设置部件是否可见
    void setVisible(QUIWidget::Widget widget, bool visible = true);
    //设置只有关闭按钮
    void setOnlyCloseBtn();
    //设置标题栏高度
    void setTitleHeight(int height);
    //设置按钮统一宽度
    void setBtnWidth(int width);

    //设置标题及文本样式
    void setTitle(const QString &title);
    void setAlignment(Qt::Alignment alignment);

    //设置主窗体
    void setMainWidget(QWidget *mainWidget);

};
#endif // QUIWIDGET_H
