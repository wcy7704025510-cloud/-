HEADERS += \
    $$PWD/VedioCommon.h \
    $$PWD/myfacedetact.h \
    $$PWD/screenread.h \
    $$PWD/threadworker.h \
    $$PWD/vedioread.h

SOURCES += \
    $$PWD/myfacedetact.cpp \
    $$PWD/screenread.cpp \
    $$PWD/threadworker.cpp \
    $$PWD/vedioread.cpp

INCLUDEPATH += F:\videoProject\VedioConference\VedioApi\opencv-release\include\opencv2\
            F:\videoProject\VedioConference\VedioApi\opencv-release\include

LIBS+=  F:\videoProject\VedioConference\VedioApi\opencv-release\lib\libopencv_calib3d420.dll.a\
        F:\videoProject\VedioConference\VedioApi\opencv-release\lib\libopencv_core420.dll.a\
        F:\videoProject\VedioConference\VedioApi\opencv-release\lib\libopencv_features2d420.dll.a\
        F:\videoProject\VedioConference\VedioApi\opencv-release\lib\libopencv_flann420.dll.a\
        F:\videoProject\VedioConference\VedioApi\opencv-release\lib\libopencv_highgui420.dll.a\
        F:\videoProject\VedioConference\VedioApi\opencv-release\lib\libopencv_imgproc420.dll.a\
        F:\videoProject\VedioConference\VedioApi\opencv-release\lib\libopencv_ml420.dll.a\
        F:\videoProject\VedioConference\VedioApi\opencv-release\lib\libopencv_objdetect420.dll.a\
        F:\videoProject\VedioConference\VedioApi\opencv-release\lib\libopencv_video420.dll.a\
        F:\videoProject\VedioConference\VedioApi\opencv-release\lib\libopencv_videoio420.dll.a
