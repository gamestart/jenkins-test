QT += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets network

TARGET = smore_vtk
CONFIG += c++17
CONFIG += resources_big

INCLUDEPATH += $$PWD/../include \
               $$PWD/../include/opencv \
               $$PWD/../include/spdlog

CONFIG(debug,  debug|release){
    DESTDIR     =   $$OUT_PWD/debug/bin
    OBJECTS_DIR =   $$OUT_PWD/debug/obj
    MOC_DIR     =   $$OUT_PWD/debug/moc
    RCC_DIR     =   $$OUT_PWD/debug/rcc
    UI_DIR      =   $$OUT_PWD/debug/ui
} else {
    DESTDIR     =   $$OUT_PWD/release/bin
    OBJECTS_DIR =   $$OUT_PWD/release/obj
    MOC_DIR     =   $$OUT_PWD/release/moc
    RCC_DIR     =   $$OUT_PWD/release/rcc
    UI_DIR      =   $$OUT_PWD/release/ui
}


include($$PWD/uiframe/uiframe.pri)
include($$PWD/aialgoservice/aialgoservice.pri)
include($$PWD/cameraservice/cameraservice.pri)
include($$PWD/common/vtkcommon.pri)
include($$PWD/runtimecontroller/runtimecontroller.pri)
include($$PWD/configdatamanager/configdatamanager.pri)
include($$PWD/imagesaver/imagesaver.pri)
include($$PWD/logservice/logservice.pri)

SOURCES += main.cpp

RESOURCES += \
    $$PWD/vtk.qrc

win32{
    CONFIG(debug,  debug|release){
        LIBS += -L$$PWD/../lib/opencv/debug -lopencv_world420d
        vld{
            INCLUDEPATH += $$PWD/../include/vld
            LIBS += -L$$PWD/../lib/vld -lvld
            DEFINES += VLD
        }
    } else {
        LIBS += -L$$PWD/../lib/opencv/release -lopencv_world420
    }

    LIBS += -ldbghelp

    CONFIG(debug,  debug|release){
        SRC_DIR = $$PWD
        OUTPUT_DIR = $$OUT_PWD/debug/bin/
        SRC_DIR = $$replace(SRC_DIR, /, \\)
        OUTPUT_DIR = $$replace(OUTPUT_DIR, /, \\)
        QMAKE_POST_LINK += copy /Y $$SRC_DIR\install_debug.bat   $$OUTPUT_DIR  && cd $$OUTPUT_DIR  && call install_debug.bat
    } else {
        SRC_DIR = $$PWD
        OUTPUT_DIR = $$OUT_PWD/release/bin/
        SRC_DIR = $$replace(SRC_DIR, /, \\)
        OUTPUT_DIR = $$replace(OUTPUT_DIR, /, \\)
        QMAKE_POST_LINK += copy /Y $$SRC_DIR\install_release.bat   $$OUTPUT_DIR  && cd $$OUTPUT_DIR && call install_release.bat
    }
}else{
    LIBS += -lstdc++fs

    LIBS += -L$$PWD/../lib/opencv/release -lopencv_core -lopencv_imgproc -lopencv_imgcodecs -lopencv_calib3d
    LIBS += $$PWD/../lib/algo/libm.so.6
    LIBS += $$PWD/../lib/algo/libstdc++.so.6
    LIBS += -L$$PWD/../lib/basler \
            -L$$PWD/../lib/hik/release
    LIBS += -Wl,-rpath-link,$$PWD/../lib/basler
    LIBS += -Wl,-rpath-link,$$PWD/../lib/hik/release
    LIBS += -Wl,-rpath-link,$$PWD/../lib/algo
    LIBS += -Wl,-rpath-link,$$PWD/../lib/opencv/release

    CONFIG(debug,  debug|release){
        SRC_DIR = $$PWD
        OUTPUT_DIR = $$OUT_PWD/debug/bin/
        SRC_DIR = $$replace(SRC_DIR, /, \\)
        OUTPUT_DIR = $$replace(OUTPUT_DIR, /, \\)
    } else {
        SRC_DIR = $$PWD
        OUTPUT_DIR = $$OUT_PWD/release/bin/
        SRC_DIR = $$replace(SRC_DIR, /, \\)
        OUTPUT_DIR = $$replace(OUTPUT_DIR, /, \\)
    }
}
