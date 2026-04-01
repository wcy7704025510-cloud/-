TEMPLATE = app
CONFIG += console c++11
CONFIG -= app_bundle
CONFIG -= qt

LIBS += -lpthread
LIBS += -lpthread -lmysqlclient
INCLUDEPATH += \
    $$PWD/core \
    $$PWD/logic \
    $$PWD/infrastructure/netapi/net \
    $$PWD/infrastructure/netapi/mediator \
    $$PWD/infrastructure/dbapi \
    $$PWD/utils

include(core/core.pri)
include(logic/logic.pri)
include(infrastructure/infrastructure.pri)
include(utils/utils.pri)

