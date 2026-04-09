INCLUDEPATH += $$PWD \
               $$PWD/opencv-release/include \
               $$PWD/ffmpeg-4.2.2/include

HEADERS += \
    $$PWD/H264Encoder.h \
    $$PWD/VideoWorker.h \
    $$PWD/h264decoder.h \
    $$PWD/videoread.h \
    $$PWD/screenread.h \
    $$PWD/VideoCommon.h \
    $$PWD/threadworker.h

SOURCES += \
    $$PWD/H264Encoder.cpp \
    $$PWD/VideoWorker.cpp \
    $$PWD/h264decoder.cpp \
    $$PWD/videoread.cpp \
    $$PWD/screenread.cpp \
    $$PWD/threadworker.cpp



# OpenCV libs
LIBS += -L$$PWD/opencv-release/bin/ -llibopencv_core420
LIBS += -L$$PWD/opencv-release/bin/ -llibopencv_highgui420
LIBS += -L$$PWD/opencv-release/bin/ -llibopencv_imgproc420
LIBS += -L$$PWD/opencv-release/bin/ -llibopencv_imgcodecs420
LIBS += -L$$PWD/opencv-release/bin/ -llibopencv_video420
LIBS += -L$$PWD/opencv-release/bin/ -llibopencv_videoio420
LIBS += -L$$PWD/opencv-release/bin/ -llibopencv_objdetect420
LIBS += -L$$PWD/ffmpeg-4.2.2/lib/ -lavcodec -lavdevice -lavfilter -lavformat -lavutil -lpostproc -lswresample -lswscale
