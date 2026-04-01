INCLUDEPATH += $$PWD \
               $$PWD/opencv-release/include

HEADERS += \
    $$PWD/VideoWorker.h \
    $$PWD/videoread.h \
    $$PWD/screenread.h \
    $$PWD/myfacedetact.h \
    $$PWD/VideoCommon.h \
    $$PWD/threadworker.h

SOURCES += \
    $$PWD/VideoWorker.cpp \
    $$PWD/videoread.cpp \
    $$PWD/screenread.cpp \
    $$PWD/myfacedetact.cpp \
    $$PWD/threadworker.cpp

# OpenCV libs
LIBS += -L$$PWD/opencv-release/bin/ -llibopencv_core420
LIBS += -L$$PWD/opencv-release/bin/ -llibopencv_highgui420
LIBS += -L$$PWD/opencv-release/bin/ -llibopencv_imgproc420
LIBS += -L$$PWD/opencv-release/bin/ -llibopencv_imgcodecs420
LIBS += -L$$PWD/opencv-release/bin/ -llibopencv_video420
LIBS += -L$$PWD/opencv-release/bin/ -llibopencv_videoio420
LIBS += -L$$PWD/opencv-release/bin/ -llibopencv_objdetect420