#include "Pin2.h"
#include <QtWidgets/QApplication>
#include "PinWindow.h"
#include "PinDetect.h"
#include "rs_motion.h"
#include "qg_VersionDialog.h"
#include "QUIWidget.h"

#include <DbgHelp.h>
#pragma comment(lib, "DbgHelp.lib")

// ���� MiniDump �ĺ���
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

// �쳣������
LONG WINAPI ExceptionFilter(EXCEPTION_POINTERS* exceptionInfo) {
    CreateMiniDump(exceptionInfo);
    return EXCEPTION_EXECUTE_HANDLER;
}

//���汾��Ϣ
int readVersionJson(int& in_version)
{
    // ��ȡ�ļ�·��
    QString filePath(qApp->applicationDirPath() + "/AppVersion.json");

    QFile file(filePath);
    if (file.open(QIODevice::ReadOnly)) {
        QByteArray data = file.readAll();
        QJsonDocument doc = QJsonDocument::fromJson(data);
        if (doc.isObject()) {
            QJsonObject jsonObj = doc.object();
            if (jsonObj.contains("version")) {
                int version = jsonObj["version"].toInt();
                qDebug() << "��ȡ�İ汾��: " << version;
                in_version = version;
            }
            else {
                qDebug() << "δ�ҵ� version �ֶ�";
                //���ļ��е�һ�δ��������ѡ��汾
                return -1;
            }
        }
        file.close();
    }
    else {
        qDebug() << "�޷����ļ�: " << filePath;
        //���ļ��е�һ�δ��������ѡ��汾
        return -1;
    }
    return 0;
}

void saveVersionToFile(int version)
{
    // ��ȡ�ļ�·��
    QString filePath(qApp->applicationDirPath() + "/AppVersion.json");

    // ���� JSON �������� version
    QJsonObject jsonObj;
    jsonObj["version"] = version;

    // �� QJsonObject ת��Ϊ QJsonDocument
    QJsonDocument doc(jsonObj);

    // ���� JSON ���ݵ��ļ�
    QFile file(filePath);
    if (file.open(QIODevice::WriteOnly)) {
        file.write(doc.toJson());
        file.close();
        qDebug() << "�ɹ�����汾��: " << version;
    }
    else {
        qDebug() << "�޷������ļ�: " << filePath;
    }
}

int main(int argc, char *argv[])
{
    try {
        SetUnhandledExceptionFilter(ExceptionFilter);
        QApplication a(argc, argv);
        // ÿ�θ���ͼ��ʱ���Ӱ汾��
        QApplication::setApplicationName("QVision");
        QApplication::setApplicationVersion("0.1.0"); 
        // ����Ӧ�ó���ͼ��
        a.setWindowIcon(QIcon(":/icons/icon.png"));
        QTranslator translator;
        QString translationDir = QCoreApplication::applicationDirPath() + "/translations";

        if (translator.load("Translation_zh_CN.qm", translationDir)) {
            a.installTranslator(&translator);
        }

        //��ȡ�汾
        int rtn = readVersionJson(LSM->m_version);
        if (rtn != 0)
        {
            //�޿��ð汾��
#ifndef DEBUG_LHC
            //������֤�߼�
            bool isPasswordOK = false;
            while (!isPasswordOK) { // ѭ��ֱ��������ȷ
                // ������������Ի���
                QInputDialog pwdDialog;
                pwdDialog.setWindowTitle("������֤");
                pwdDialog.setLabelText("������������룺");
                pwdDialog.setTextEchoMode(QLineEdit::Password); // ������������

                pwdDialog.setOkButtonText("ȷ��");
                pwdDialog.setCancelButtonText("ȡ��");

                // ִ�жԻ���
                if (pwdDialog.exec() == QDialog::Accepted) {
                    // ��֤���루��������"888888"��
                    if (pwdDialog.textValue() == "888888") {
                        isPasswordOK = true;
                    }
                    else {
                        // ���������ʾ
                        QMessageBox::critical(nullptr, "����", "������������ԣ�");
                    }
                }
                else {
                    // �û�ȡ������ֱ���˳�
                    return -1;
                }
            }
#endif // !DEBUG_LHC

            // �����汾�Ի���
            qg_VersionDialog verDialog(LSM->m_version);
            int result = verDialog.exec();

            if (result == QDialog::Accepted) {
                int tp = verDialog.getVersion();
                LSM->m_version = tp;
                //д�뵱ǰ�汾
                saveVersionToFile(LSM->m_version);
            }
            else {
                return -1;
            }
        }

        //ȷ���忨�汾
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

        //��ʼ���˶�����
        LSM->init();

        //��ȡ�˶�����
        LSM->loadMotionConfig();

        //QUIWidget qui;
        std::unique_ptr<Pin2> app = std::make_unique<Pin2>();
        //qui.setMainWidget(app.get());
        //qui.show();
        app->show();
        int execResult = a.exec();
        app.reset();

        // �ڳ������ʱ���� LSM
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
