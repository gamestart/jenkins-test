#include "aialgoservice/aialgomanager.h"
#include "cameraservice/cameramanager.h"
#include "common/crashdumper.h"
#include "common/vtkcommon.h"
#include "configdatamanager/configmanager.h"
#include "imagesaver/asyncimagesaver.h"
#include "runtimecontroller/runtimecontroller.h"
#include "uiframe/commonuiset/messagedialog/messagedialog.h"
#include "uiframe/mainuiset/mainframe.h"
#include <QApplication>
#include <QComboBox>
#include <QFile>
#include <QFontDatabase>
#include <QSharedMemory>
#include <QTextCodec>

#ifdef VLD
#include "vld.h"
#endif

Q_DECLARE_METATYPE(cv::Mat);

bool singleApplication(const QString &flag)
{
    static std::unique_ptr<QSharedMemory> share_mem{nullptr};

    if (share_mem)
        return true;

    share_mem = std::make_unique<QSharedMemory>(flag);
    for (auto count = 0; count < 2; count++)
    {
        if (share_mem->attach(QSharedMemory::ReadOnly))
            share_mem->detach();
    }

    if (share_mem->create(1))
        return true;

    return false;
}

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    QTranslator translator;
    int lang = 0;
    if (lang)
    {
        translator.load("://uiframe/translations/smore_vtk_en.qm");
        qApp->setProperty("lang", "en");
    }
    else
    {
        translator.load("://uiframe/translations/smore_vtk_zh-cn.qm");
        qApp->setProperty("lang", "zh");
    }
    qApp->installTranslator(&translator);
    if (!singleApplication("smore_vtk.exe"))
    {
        vtk::display::MessageDialog message_box_dialog(
            vtk::display::DialogType::WarningDialog,
            QObject::tr("Only one instance is allowed, please close another instance to continue!"));
        message_box_dialog.exec();
        return 0;
    }

    vtk::applogger::initializeLog();
    vtk::common::registerCrashDumper();

    qRegisterMetaType<cv::Mat>("cv::Mat");
    qRegisterMetaType<std::string>("std::string");
    QTextCodec::setCodecForLocale(QTextCodec::codecForName("UTF-8"));
    auto zh_font_id = QFontDatabase::addApplicationFont("://uiframe/font/SourceHanSansCN-Medium.ttf");
    // auto en_font_id = QFontDatabase::addApplicationFont("://uiframe/font/Roboto-Regular.ttf");
    auto font_families = QFontDatabase::applicationFontFamilies(zh_font_id);
    if (!font_families.empty())
        app.setFont(font_families.at(0));

    QFile styleFile("://uiframe/style/stylesheet.qss");
    styleFile.open(QFile::ReadOnly);
    QString style = QString::fromUtf8(styleFile.readAll());
    app.setStyleSheet(style);
    styleFile.close();

    vtk::config::ConfigManager::getInstance().init();
    std::vector<std::string> camera_id_list;
    for (auto &camera_item : vtk::config::ConfigManager::getInstance().getCameraConfigData())
    {
        camera_id_list.push_back(camera_item.id.toStdString());
    }
    vtk::cameraservice::CameraManager::getInstance().resetDisconnectCameraIdList(camera_id_list);
    vtk::cameraservice::CameraManager::getInstance().init();
    vtk::control::RuntimeController::getInstance().init();
    qApp->setWindowIcon(QIcon("://uiframe/images/logo.ico"));

    vtk::display::MainFrame w;
    w.setTranslator(&translator);
    w.show();
    auto result = app.exec();
    vtk::algoservice::AIAlgoManager::getInstance().clearAlgos();
    vtk::cameraservice::CameraManager::getInstance().clearCameras();
    vtk::imagesave::AsyncImageSaver::waitQuit();
    return result;
}

// 筛选菜单禁用鼠标滚轮
void QComboBox::wheelEvent(QWheelEvent *e)
{
    // do nothing
}
