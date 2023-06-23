TEMPLATE = subdirs
CONFIG += ordered

# Внимание: порядок следования подпроектов учитывает зависимости между ними.
SUBDIRS += \
    aucs \
    core \
    gui \
    core/process \
    #core/my_image_provider \

$$type(SUBDIRS)
