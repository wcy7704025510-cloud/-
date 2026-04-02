INCLUDEPATH += $$PWD $$PWD/net $$PWD/mediator
HEADERS += $$PWD/net/INet.h $$PWD/net/TcpNet.h $$PWD/net/Thread_pool.h $$PWD/net/packdef.h
SOURCES += $$PWD/net/TcpNet.cpp $$PWD/net/Thread_pool.cpp \
    $$PWD/mediator/INetMediator.cpp
HEADERS += $$PWD/mediator/INetMediator.h
SOURCES +=
