TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += main.cpp \
    net_protocol.cpp \
    server_network.cpp \
    mtig/mtig_host.c \
    mpc07/mpc07.cpp


HEADERS += \
    net_protocol.h \
    server_network.h \
    mtig/mtig_host.h \
    mpc07/mpc07.h


INCLUDEPATH += D:\Program_Files\pyth\include \


LIBS += -LD:\Program_Files\pyth\libs -lpython27
