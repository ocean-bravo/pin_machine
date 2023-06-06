HEADERS += \
        $$PWD/breakpad.h \

SOURCES += \
        $$PWD/breakpad.cpp \

INCLUDEPATH += $${PROJECT_ROOT}/../breakpad/include
INCLUDEPATH += $${PROJECT_ROOT}/../breakpad/include/breakpad

# breakpad собран и лежит в директории breakpad на одном уровне с smcrimp_v8
LIBS += $${PROJECT_ROOT}/../breakpad/lib/libbreakpad_client.a
