TEMPLATE = subdirs
CONFIG += ordered

# Внимание: порядок следования подпроектов учитывает зависимости между ними.
SUBDIRS += \
    aux \
    core \
    gui \
    core/process \

$$type(SUBDIRS)
