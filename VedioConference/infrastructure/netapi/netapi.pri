INCLUDEPATH += $$PWD \
               $$PWD/mediator \
               $$PWD/net

HEADERS += \
    $$PWD/net/packdef.h \
    $$PWD/net/INet.h \
    $$PWD/net/TcpClient.h \
    $$PWD/net/UdpNet.h \
    $$PWD/mediator/INetMediator.h

SOURCES += \
    $$PWD/net/INet.cpp \
    $$PWD/net/TcpClient.cpp \
    $$PWD/net/UdpNet.cpp \
    $$PWD/mediator/INetMediator.cpp

win32:LIBS += -lws2_32
