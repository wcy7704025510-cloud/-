INCLUDEPATH += $$PWD $$PWD/net $$PWD/mediator
HEADERS += $$PWD/net/INet.h $$PWD/net/TcpNet.h $$PWD/net/Thread_pool.h $$PWD/common/packdef.h \
    $$PWD/common/MyMap.h \
    $$PWD/common/NetBuffer.h \
    $$PWD/net/KcpNet.h \
    $$PWD/net/ikcp.h
SOURCES += $$PWD/net/TcpNet.cpp $$PWD/net/Thread_pool.cpp \
    $$PWD/mediator/INetMediator.cpp \
    $$PWD/net/KcpNet.cpp \
    $$PWD/net/ikcp.cpp
HEADERS += $$PWD/mediator/INetMediator.h
