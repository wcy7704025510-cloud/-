TEMPLATE = app
CONFIG += console c++11
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += \
    src/Mediator/TcpMediator.cpp \
    src/Mysql.cpp \
    src/Net/TcpNet.cpp \
    src/Net/Thread_pool.cpp \
    src/err_str.cpp \
    src/kernel/CKernel.cpp \
    src/kernel/clogic.cpp \
    src/main.cpp

DISTFILES += \
    src/makefile

HEADERS += \
    include/Mediator/INetMediator.h \
    include/Mediator/TcpMediator.h \
    include/Mysql.h \
    include/Net/INet.h \
    include/Net/TcpNet.h \
    include/Net/Thread_pool.h \
    include/err_str.h \
    include/kernel/CKernel.h \
    include/kernel/clogic.h \
    include/packdef.h

INCLUDEPATH += ./include

LIBS += -lpthread -lmysqlclient
