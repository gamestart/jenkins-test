
INCLUDEPATH += $$PWD/../../include/algo           \
               $$PWD/../../include/onnxruntime    \
               $$PWD/../../include/sodium

win32{
    LIBS += -L$$PWD/../../lib/onnxruntime    -lonnxruntime \
            -L$$PWD/../../lib/sodium         -llibsodium \
            -L$$PWD/../../lib/algo           -lVimoDetection \
            -L$$PWD/../../lib/algo           -lVimoClassification \
            -L$$PWD/../../lib/algo           -lVimoSegmentation \
            -L$$PWD/../../lib/algo           -lVimoOCR
}else{
    LIBS += -L$$PWD/../../lib/onnxruntime    -lonnxruntime \
            -L$$PWD/../../lib/sodium         -lsodium \
            -L$$PWD/../../lib/algo           -lVimoDetection \
            -L$$PWD/../../lib/algo           -lVimoClassification \
            -L$$PWD/../../lib/algo           -lVimoSegmentation \
            -L$$PWD/../../lib/algo           -lVimoOCR
}

HEADERS +=  \
           $$PWD/aialgomanager.h \
           $$PWD/algobase.h \
           $$PWD/classificationalgo.h \
           $$PWD/detectionalgo.h \
           $$PWD/ocralgo.h \
           $$PWD/segmentationalgo.h

SOURCES += \
           $$PWD/aialgomanager.cpp \
           $$PWD/algobase.cpp \
           $$PWD/classificationalgo.cpp \
           $$PWD/detectionalgo.cpp \
           $$PWD/ocralgo.cpp \
           $$PWD/segmentationalgo.cpp




