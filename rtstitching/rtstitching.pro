#-------------------------------------------------
#
# Project created by QtCreator 2016-01-21T21:23:04
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = rtstitching
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp

HEADERS  += mainwindow.h

FORMS    += mainwindow.ui

macx-clang {
INCLUDEPATH += /Users/hehao/work/lib/opencv-3.1.0/include \
/Users/hehao/work/lib/opencv-3.1.0/osx \
/Users/hehao/work/lib/opencv-3.1.0/modules/core/include \
/Users/hehao/work/lib/opencv-3.1.0/modules/highgui/include \
/Users/hehao/work/lib/opencv-3.1.0/modules/imgcodecs/include \
/Users/hehao/work/lib/opencv-3.1.0/modules/stitching/include \
/Users/hehao/work/lib/opencv-3.1.0/modules/imgproc/include \
/Users/hehao/work/lib/opencv-3.1.0/modules/features2d/include \
/Users/hehao/work/lib/opencv-3.1.0/modules/flann/include \
/Users/hehao/work/lib/opencv-3.1.0/modules/videoio/include


LIBS += -L/Users/hehao/work/lib/opencv-3.1.0/osx/lib \
    -lopencv_core -lopencv_imgproc -lopencv_flann -lopencv_imgcodecs \
    -lopencv_videoio -lopencv_highgui -lopencv_features2d -lopencv_stitching

LIBS += -lopencv_ml -lopencv_video \
-lopencv_objdetect -lopencv_photo -lopencv_calib3d \
-lopencv_videostab -lopencv_shape

QMAKE_MACOSX_DEPLOYMENT_TARGET = 10.9
}
