#pragma execution_character_set("utf-8")

#include "quiwidget.h"

QUIWidget::QUIWidget(QWidget *parent) : QDialog(parent)
{
    this->initControl();
    this->initForm();

	setPixmap(QUIWidget::BtnMenu_Min, "./qss/darkstyle/min.png");
	setPixmap(QUIWidget::BtnMenu_Max, "./qss/darkstyle/max.png");
	setPixmap(QUIWidget::BtnMenu_Close, "./qss/darkstyle/close.png");
	//设置标题文本居中
	setAlignment(Qt::AlignCenter);
	//设置窗体可拖动大小
    this->setSizeGripEnabled(true);
}

QUIWidget::~QUIWidget()
{
    delete widgetMain;
}

void QUIWidget::setStyle(const QString &qssFile)
{
    QFile file(qssFile);

    if (file.open(QFile::ReadOnly)) {
        QString qss = QLatin1String(file.readAll());
        //QString paletteColor = qss.mid(20, 7);
        //qApp->setPalette(QPalette(QColor(paletteColor)));
        qApp->setStyleSheet(qss);
        file.close();
    }
}

void QUIWidget::setFormInCenter(QWidget *frm)
{
    int frmX = frm->width();
    int frmY = frm->height();

	// 获取主屏幕可用区域（去掉任务栏）
	QScreen* screen = QGuiApplication::primaryScreen();
	QRect deskRect = screen->availableGeometry();

	// 计算居中坐标
	QPoint movePoint(deskRect.x() + (deskRect.width() - frmX) / 2,
		deskRect.y() + (deskRect.height() - frmY) / 2);

    frm->move(movePoint);
}

bool QUIWidget::eventFilter(QObject *obj, QEvent *evt)
{
    static QPoint mousePoint;
    static bool mousePressed = false;

    QMouseEvent *event = static_cast<QMouseEvent *>(evt);
    if (event->type() == QEvent::MouseButtonPress) {
        if (event->button() == Qt::LeftButton) {
            mousePressed = true;
            mousePoint = event->globalPos() - this->pos();
            return true;
        }
    } else if (event->type() == QEvent::MouseButtonRelease) {
        mousePressed = false;
        return true;
    } else if (event->type() == QEvent::MouseMove) {
        if (!max && mousePressed && (event->buttons() && Qt::LeftButton)) {
            this->move(event->globalPos() - mousePoint);
            return true;
        }
    } else if (event->type() == QEvent::MouseButtonDblClick) {
        //以下写法可以将双击识别限定在标题栏
        if (this->btnMenu_Max->isVisible() && obj == this->widget_title) {
        //if (this->btnMenu_Max->isVisible()) {
            this->on_btnMenu_Max_clicked();
            return true;
        }
    }

    return QWidget::eventFilter(obj, evt);
}

QString QUIWidget::getTitle() const
{
    return this->title;
}

Qt::Alignment QUIWidget::getAlignment() const
{
    return this->alignment;
}

QSize QUIWidget::sizeHint() const
{
    return QSize(600, 450);
}

QSize QUIWidget::minimumSizeHint() const
{
    return QSize(200, 150);
}

void QUIWidget::initControl()
{
    this->setObjectName(QString::fromUtf8("QUIWidget"));
    this->resize(900, 750);
    verticalLayout1 = new QVBoxLayout(this);
    verticalLayout1->setSpacing(0);
    verticalLayout1->setContentsMargins(11, 11, 11, 11);
    verticalLayout1->setObjectName(QString::fromUtf8("verticalLayout1"));
    verticalLayout1->setContentsMargins(1, 1, 1, 1);
    widgetMain = new QWidget(this);
    widgetMain->setObjectName(QString::fromUtf8("widgetMain"));
    widgetMain->setStyleSheet(QString::fromUtf8(""));
    verticalLayout2 = new QVBoxLayout(widgetMain);
    verticalLayout2->setSpacing(0);
    verticalLayout2->setContentsMargins(11, 11, 11, 11);
    verticalLayout2->setObjectName(QString::fromUtf8("verticalLayout2"));
    verticalLayout2->setContentsMargins(0, 0, 0, 0);
    widget_title = new QWidget(widgetMain);
    widget_title->setObjectName(QString::fromUtf8("widget_title"));
    QSizePolicy sizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
    sizePolicy.setHorizontalStretch(0);
    sizePolicy.setVerticalStretch(0);
    sizePolicy.setHeightForWidth(widget_title->sizePolicy().hasHeightForWidth());
    widget_title->setSizePolicy(sizePolicy);
    widget_title->setMinimumSize(QSize(0, 30));
    horizontalLayout4 = new QHBoxLayout(widget_title);
    horizontalLayout4->setSpacing(0);
    horizontalLayout4->setContentsMargins(11, 11, 11, 11);
    horizontalLayout4->setObjectName(QString::fromUtf8("horizontalLayout4"));
    horizontalLayout4->setContentsMargins(0, 0, 0, 0);
    lab_Ico = new QLabel(widget_title);
    lab_Ico->setObjectName(QString::fromUtf8("lab_Ico"));
    QSizePolicy sizePolicy1(QSizePolicy::Minimum, QSizePolicy::Preferred);
    sizePolicy1.setHorizontalStretch(0);
    sizePolicy1.setVerticalStretch(0);
    sizePolicy1.setHeightForWidth(lab_Ico->sizePolicy().hasHeightForWidth());
    lab_Ico->setSizePolicy(sizePolicy1);
    lab_Ico->setMinimumSize(QSize(30, 0));
    lab_Ico->setAlignment(Qt::AlignCenter);

    horizontalLayout4->addWidget(lab_Ico);

    lab_Title = new QLabel(widget_title);
    lab_Title->setObjectName(QString::fromUtf8("lab_Title"));
    QSizePolicy sizePolicy2(QSizePolicy::Expanding, QSizePolicy::Preferred);
    sizePolicy2.setHorizontalStretch(0);
    sizePolicy2.setVerticalStretch(0);
    sizePolicy2.setHeightForWidth(lab_Title->sizePolicy().hasHeightForWidth());
    lab_Title->setSizePolicy(sizePolicy2);
    lab_Title->setAlignment(Qt::AlignLeading | Qt::AlignLeft | Qt::AlignVCenter);

    horizontalLayout4->addWidget(lab_Title);

    widget_menu = new QWidget(widget_title);
    widget_menu->setObjectName(QString::fromUtf8("widget_menu"));
    sizePolicy1.setHeightForWidth(widget_menu->sizePolicy().hasHeightForWidth());
    widget_menu->setSizePolicy(sizePolicy1);
    horizontalLayout = new QHBoxLayout(widget_menu);
    horizontalLayout->setSpacing(0);
    horizontalLayout->setContentsMargins(11, 11, 11, 11);
    horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
    horizontalLayout->setContentsMargins(0, 0, 0, 0);

    btnMenu_Min = new QPushButton(widget_menu);
    btnMenu_Min->setObjectName(QString::fromUtf8("btnMenu_Min"));
    QSizePolicy sizePolicy4(QSizePolicy::Minimum, QSizePolicy::Expanding);
    sizePolicy4.setHorizontalStretch(0);
    sizePolicy4.setVerticalStretch(0);
    sizePolicy4.setHeightForWidth(btnMenu_Min->sizePolicy().hasHeightForWidth());
    btnMenu_Min->setSizePolicy(sizePolicy4);
    btnMenu_Min->setMinimumSize(QSize(30, 0));
    btnMenu_Min->setMaximumSize(QSize(30, 16777215));
    btnMenu_Min->setCursor(QCursor(Qt::ArrowCursor));
    btnMenu_Min->setFocusPolicy(Qt::NoFocus);

    horizontalLayout->addWidget(btnMenu_Min);

    btnMenu_Max = new QPushButton(widget_menu);
    btnMenu_Max->setObjectName(QString::fromUtf8("btnMenu_Max"));
    sizePolicy4.setHeightForWidth(btnMenu_Max->sizePolicy().hasHeightForWidth());
    btnMenu_Max->setSizePolicy(sizePolicy4);
    btnMenu_Max->setMinimumSize(QSize(30, 0));
    btnMenu_Max->setMaximumSize(QSize(30, 16777215));
    btnMenu_Max->setCursor(QCursor(Qt::ArrowCursor));
    btnMenu_Max->setFocusPolicy(Qt::NoFocus);

    horizontalLayout->addWidget(btnMenu_Max);

    btnMenu_Close = new QPushButton(widget_menu);
    btnMenu_Close->setObjectName(QString::fromUtf8("btnMenu_Close"));
    sizePolicy4.setHeightForWidth(btnMenu_Close->sizePolicy().hasHeightForWidth());
    btnMenu_Close->setSizePolicy(sizePolicy4);
    btnMenu_Close->setMinimumSize(QSize(30, 0));
    btnMenu_Close->setMaximumSize(QSize(30, 16777215));
    btnMenu_Close->setCursor(QCursor(Qt::ArrowCursor));
    btnMenu_Close->setFocusPolicy(Qt::NoFocus);

    horizontalLayout->addWidget(btnMenu_Close);
    horizontalLayout4->addWidget(widget_menu);
    verticalLayout2->addWidget(widget_title);

    widget = new QWidget(widgetMain);
    widget->setObjectName(QString::fromUtf8("widget"));
    verticalLayout3 = new QVBoxLayout(widget);
    verticalLayout3->setSpacing(0);
    verticalLayout3->setContentsMargins(11, 11, 11, 11);
    verticalLayout3->setObjectName(QString::fromUtf8("verticalLayout3"));
    verticalLayout3->setContentsMargins(0, 0, 0, 0);

    verticalLayout2->addWidget(widget);
    verticalLayout1->addWidget(widgetMain);

    connect(this->btnMenu_Min, SIGNAL(clicked(bool)), this, SLOT(on_btnMenu_Min_clicked()));
    connect(this->btnMenu_Max, SIGNAL(clicked(bool)), this, SLOT(on_btnMenu_Max_clicked()));
    connect(this->btnMenu_Close, SIGNAL(clicked(bool)), this, SLOT(on_btnMenu_Close_clicked()));
}

void QUIWidget::initForm()
{
    //设置标题及对齐方式
    setTitle("QUI Demo");
    setAlignment(Qt::AlignLeft | Qt::AlignVCenter);

    mainWidget = 0;
    max = false;
    location = this->geometry();
    this->setProperty("form", true);
    this->setWindowFlags(Qt::FramelessWindowHint | Qt::WindowSystemMenuHint | Qt::WindowMinMaxButtonsHint);

    //绑定事件过滤器监听鼠标移动
    this->installEventFilter(this);
    this->widget_title->installEventFilter(this);
}

void QUIWidget::setPixmap(QUIWidget::Widget widget, const QString &file, const QSize &size)
{
    QPixmap pix = QPixmap(file);
    //按照宽高比自动缩放
    pix = pix.scaled(size, Qt::KeepAspectRatio);

    if (widget == QUIWidget::Lab_Ico) {
        this->lab_Ico->setPixmap(pix);

    } else if (widget == QUIWidget::BtnMenu_Min) {
        this->btnMenu_Min->setIcon(QIcon(file));
    } else if (widget == QUIWidget::BtnMenu_Max) {
        this->btnMenu_Max->setIcon(QIcon(file));
    } else if (widget == QUIWidget::BtnMenu_Close) {
        this->btnMenu_Close->setIcon(QIcon(file));
    }
}

void QUIWidget::setVisible(QUIWidget::Widget widget, bool visible)
{
    if (widget == QUIWidget::Lab_Ico) {
        this->lab_Ico->setVisible(visible);
    } else if (widget == QUIWidget::BtnMenu_Min) {
        this->btnMenu_Min->setVisible(visible);
    } else if (widget == QUIWidget::BtnMenu_Max) {
        this->btnMenu_Max->setVisible(visible);
    } else if (widget == QUIWidget::BtnMenu_Close) {
        this->btnMenu_Close->setVisible(visible);
    }
}

void QUIWidget::setOnlyCloseBtn()
{
    this->btnMenu_Min->setVisible(false);
    this->btnMenu_Max->setVisible(false);
}

void QUIWidget::setTitleHeight(int height)
{
    this->widget_title->setFixedHeight(height);
}

void QUIWidget::setBtnWidth(int width)
{
    this->lab_Ico->setFixedWidth(width);
    this->btnMenu_Min->setFixedWidth(width);
    this->btnMenu_Max->setFixedWidth(width);
    this->btnMenu_Close->setFixedWidth(width);
}

void QUIWidget::setTitle(const QString &title)
{
    if (this->title != title) {
        this->title = title;
        this->lab_Title->setText(title);
        this->setWindowTitle(this->lab_Title->text());
    }
}

void QUIWidget::setAlignment(Qt::Alignment alignment)
{
    if (this->alignment != alignment) {
        this->alignment = alignment;
        this->lab_Title->setAlignment(alignment);
    }
}

void QUIWidget::setMainWidget(QWidget *mainWidget)
{
    //一个QUI窗体对象只能设置一个主窗体
    if (this->mainWidget == 0) {
        //将子窗体添加到布局
        this->widget->layout()->addWidget(mainWidget);
        //自动设置大小
        resize(mainWidget->width(), mainWidget->height() + this->widget_title->height());

        this->mainWidget = mainWidget;
        this->mainWidget->installEventFilter(this);
    }
}

void QUIWidget::on_btnMenu_Min_clicked()
{
    showMinimized();
}

void QUIWidget::on_btnMenu_Max_clicked()
{
    if (max) {
        this->setGeometry(location);
    } else {
		// 保存当前窗口位置和大小
		location = this->geometry();

		// 获取当前窗口所在屏幕的可用区域（去掉任务栏）
		QScreen* screen = this->screen(); // Qt 5.14+/Qt 6
		if (!screen) screen = QGuiApplication::primaryScreen();
		QRect available = screen->availableGeometry();

		this->setGeometry(available);
    }

    max = !max;
}

void QUIWidget::on_btnMenu_Close_clicked()
{
    close();
}
