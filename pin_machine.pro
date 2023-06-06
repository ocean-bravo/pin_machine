TEMPLATE = subdirs
CONFIG += ordered

# Внимание: порядок следования подпроектов учитывает зависимости между ними.
SUBDIRS += \
    app \

OTHER_FILES += \
    $$PWD/.qmake.conf \
    $$PWD/qmake_tools.pri \

$$type(SUBDIRS)
