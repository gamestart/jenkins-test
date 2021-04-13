
INCLUDEPATH += $$PWD/../../include/camera

CONFIG(debug,  debug|release){
    LIBS += -L$$PWD/../../lib/camera/debug
} else {
    LIBS += -L$$PWD/../../lib/camera/release
}

#编译时打开时只需增加 编译参数 CONFIG +=dalsa
dalsa{
    DEFINES+=DALSA

    CONFIG(debug,  debug|release){
        LIBS += -ldalsagigecamera
    } else {
        LIBS += -ldalsagigecamera
    }
}

#编译时打开时只需增加 编译参数 CONFIG +=hik
hik{
    DEFINES+=HIK

    LIBS += -lhikcamera
}

#编译时打开时只需增加 编译参数 CONFIG +=basler
basler{
    DEFINES+=BASLER

    LIBS += -lbaslergigecamera
}

HEADERS +=  \
           $$PWD/cameradevice.h \
           $$PWD/cameramanager.h \
           $$PWD/captureimage.h \
           $$PWD/monitorthread.h \
           $$PWD/virtualcamera.h

SOURCES +=  \
           $$PWD/cameradevice.cpp \
           $$PWD/cameramanager.cpp \
           $$PWD/captureimage.cpp \
           $$PWD/monitorthread.cpp \
           $$PWD/virtualcamera.cpp



