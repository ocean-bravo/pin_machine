QT += core qml quick
TEMPLATE = lib
CONFIG += plugin c++17 release
TARGET = process


HEADERS += \
    process_plugin.h \
    process.h \

SOURCES += \
    process_plugin.cpp \

DISTFILES = qmldir

copyFiles($$DISTFILES, libs/Process)

target.path = $${INSTALL_PATH}/libs/Process

INSTALLS += target
