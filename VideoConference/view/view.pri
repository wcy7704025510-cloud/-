INCLUDEPATH += $$PWD \
               $$PWD/login \
               $$PWD/room \
               $$PWD/main_frame

FORMS += \
    $$PWD/login/logindialog.ui \
    $$PWD/room/roomdialog.ui \
    $$PWD/room/usershow.ui \
    $$PWD/main_frame/videoconferencedialog.ui

HEADERS += \
    $$PWD/login/logindialog.h \
    $$PWD/room/roomdialog.h \
    $$PWD/room/usershow.h \
    $$PWD/main_frame/videoconferencedialog.h

SOURCES += \
    $$PWD/login/logindialog.cpp \
    $$PWD/room/roomdialog.cpp \
    $$PWD/room/usershow.cpp \
    $$PWD/main_frame/videoconferencedialog.cpp

