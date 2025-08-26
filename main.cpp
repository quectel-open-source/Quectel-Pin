#include "Pin2.h"
#include <QtWidgets/QApplication>
#include "PinWindow.h"
#include "PinDetect.h"
#include "rs_motion.h"
#include "qg_VersionDialog.h"
#include "QUIWidget.h"

#include <DbgHelp.h>
#pragma comment(lib, "DbgHelp.lib")

// 生成 MiniDump 的函数
void CreateMiniDump(EXCEPTION_POINTERS* exceptionInfo) {
    HANDLE hFile = CreateFile(L"CrashDump.dmp", GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
    if (hFile != INVALID_HANDLE_VALUE) {
        MINIDUMP_EXCEPTION_INFORMATION dumpInfo;
        dumpInfo.ThreadId = GetCurrentThreadId();
        dumpInfo.ExceptionPointers = exceptionInfo;
        dumpInfo.ClientPointers = FALSE;

        MiniDumpWriteDump(
            GetCurrentProcess(),
            GetCurrentProcessId(),
            hFile,
            MiniDumpNormal,
            &dumpInfo,
            NULL,
            NULL
        );
        CloseHandle(hFile);
    }
}

// 异常处理函数
LONG WINAPI ExceptionFilter(EXCEPTION_POINTERS* exceptionInfo) {
    CreateMiniDump(exceptionInfo);
    return EXCEPTION_EXECUTE_HANDLER;
}

//读版本信息
int readVersionJson(int& in_version)
{
    // 获取文件路径
    QString filePath(qApp->applicationDirPath() + "/AppVersion.json");

    QFile file(filePath);
    if (file.open(QIODevice::ReadOnly)) {
        QByteArray data = file.readAll();
        QJsonDocument doc = QJsonDocument::fromJson(data);
        if (doc.isObject()) {
            QJsonObject jsonObj = doc.object();
            if (jsonObj.contains("version")) {
                int version = jsonObj["version"].toInt();
                qDebug() << "读取的版本号: " << version;
                in_version = version;
            }
            else {
                qDebug() << "未找到 version 字段";
                //此文件夹第一次打开软件，先选择版本
                return -1;
            }
        }
        file.close();
    }
    else {
        qDebug() << "无法打开文件: " << filePath;
        //此文件夹第一次打开软件，先选择版本
        return -1;
    }
    return 0;
}

void saveVersionToFile(int version)
{
    // 获取文件路径
    QString filePath(qApp->applicationDirPath() + "/AppVersion.json");

    // 创建 JSON 对象并设置 version
    QJsonObject jsonObj;
    jsonObj["version"] = version;

    // 将 QJsonObject 转换为 QJsonDocument
    QJsonDocument doc(jsonObj);

    // 保存 JSON 数据到文件
    QFile file(filePath);
    if (file.open(QIODevice::WriteOnly)) {
        file.write(doc.toJson());
        file.close();
        qDebug() << "成功保存版本号: " << version;
    }
    else {
        qDebug() << "无法保存文件: " << filePath;
    }
}

int main(int argc, char *argv[])
{
    try {
        SetUnhandledExceptionFilter(ExceptionFilter);
        QApplication a(argc, argv);
        // 每次更新图标时增加版本号
        QApplication::setApplicationName("QVision");
        QApplication::setApplicationVersion("0.1.0"); 
        // 设置应用程序图标
        a.setWindowIcon(QIcon(":/icons/icon.png"));
        QTranslator translator;
        QString translationDir = QCoreApplication::applicationDirPath() + "/translations";

        if (translator.load("Translation_zh_CN.qm", translationDir)) {
            a.installTranslator(&translator);
        }

        //读取版本
        int rtn = readVersionJson(LSM->m_version);
        if (rtn != 0)
        {
            //无可用版本号
#ifndef DEBUG_LHC
            //密码验证逻辑
            bool isPasswordOK = false;
            while (!isPasswordOK) { // 循环直到密码正确
                // 创建密码输入对话框
                QInputDialog pwdDialog;
                pwdDialog.setWindowTitle("密码验证");
                pwdDialog.setLabelText("请输入访问密码：");
                pwdDialog.setTextEchoMode(QLineEdit::Password); // 设置密码掩码

                pwdDialog.setOkButtonText("确定");
                pwdDialog.setCancelButtonText("取消");

                // 执行对话框
                if (pwdDialog.exec() == QDialog::Accepted) {
                    // 验证密码（暂用密码"888888"）
                    if (pwdDialog.textValue() == "888888") {
                        isPasswordOK = true;
                    }
                    else {
                        // 密码错误提示
                        QMessageBox::critical(nullptr, "错误", "密码错误，请重试！");
                    }
                }
                else {
                    // 用户取消输入直接退出
                    return -1;
                }
            }
#endif // !DEBUG_LHC

            // 创建版本对话框
            qg_VersionDialog verDialog(LSM->m_version);
            int result = verDialog.exec();

            if (result == QDialog::Accepted) {
                int tp = verDialog.getVersion();
                LSM->m_version = tp;
                //写入当前版本
                saveVersionToFile(LSM->m_version);
            }
            else {
                return -1;
            }
        }

        //确定板卡版本
        switch (LSM->m_version)
        {
        case ENUM_VERSION::TwoPin:
        case ENUM_VERSION::EdPin:
            LSM->m_motionVersion = ENUM_MOTIONVERSION::LS_Pulse;
            break;
        case ENUM_VERSION::LsPin:
        case ENUM_VERSION::JmPin:
        case ENUM_VERSION::JmHan:
            LSM->m_motionVersion = ENUM_MOTIONVERSION::LS_EtherCat;
            break;
        case ENUM_VERSION::BtPin:
            LSM->m_motionVersion = ENUM_MOTIONVERSION::LS_Pulse_1000;
            break;
        default:
            break;
        }

        //初始化运动配置
        LSM->init();

        //读取运动配置
        LSM->loadMotionConfig();

        //QUIWidget qui;
        std::unique_ptr<Pin2> app = std::make_unique<Pin2>();
        //qui.setMainWidget(app.get());
        //qui.show();
        app->show();
        int execResult = a.exec();
        app.reset();

        // 在程序结束时销毁 LSM
        LSM->destroyInstance();
        return execResult;
    }
    catch (const std::exception& e) {
        qDebug() << "Exception caught: " << e.what();
        return -1;
    }
    catch (...) {
        qDebug() << "Unknown exception caught!";
        return -1;
    }
}
