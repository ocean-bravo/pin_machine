QT += core qml quick
TEMPLATE = lib
CONFIG += plugin c++17 release
TARGET = my_image_provider


HEADERS += \
    my_image_provider_plugin.h \
    my_image_provider.h \

SOURCES += \
    my_image_provider_plugin.cpp \

DISTFILES = qmldir

copyFiles($$DISTFILES, libs/MyImageProvider)

target.path = $${INSTALL_PATH}/libs/MyImageProvider

INSTALLS += target
