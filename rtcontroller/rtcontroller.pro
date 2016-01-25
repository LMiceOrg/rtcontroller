TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += main.cpp \
    net_protocol.cpp \
    server_network.cpp \
    mpc07/mpc07.cpp \
    okcapture/ok_capture.cpp


HEADERS += \
    net_protocol.h \
    server_network.h \
    mpc07/mpc07.h \
    okcapture/ok_capture.h \
    okcapture/Okapi64.h

LIBS += -lUser32
#INCLUDEPATH += D:/Program Files/pyth/include \

#LIBS += -LD:/Program Files/pyth/libs -lpython27

LIBS += -LD:/PosNo2/RTcontroller/rtcontroller/rtcontroller/okcapture -lokapi64
