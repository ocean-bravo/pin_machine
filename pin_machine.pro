TEMPLATE = subdirs
CONFIG += ordered

# Версия ПО вычислятся как число коммитов в текущей ветке между меткой reference_version и последним коммитом
# Ветка и дата последнего коммита записывается в переменные, откуда записывается в лог
CURRENT_BRANCH = $$system(git rev-parse --abbrev-ref HEAD)
unix:  GIT_CURRENT_VERSION = $$system(git log start..$${CURRENT_BRANCH} --oneline --first-parent | wc -l)
win32: GIT_CURRENT_VERSION = $$system(git log start..$${CURRENT_BRANCH} --oneline --first-parent | find /c /v \"\")

# Внимание: порядок следования подпроектов учитывает зависимости между ними.
SUBDIRS += \
    app \

OTHER_FILES += \
    $$PWD/.qmake.conf \
    $$PWD/qmake_tools.pri \

$$type(SUBDIRS)
