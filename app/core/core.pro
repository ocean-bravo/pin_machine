QT += quick serialport widgets network gui printsupport concurrent quickcontrols2

CONFIG += console qtquickcompiler

TRANSLATIONS += core_language_de.ts core_language_pl.ts core_language_fr.ts

# Версия ПО вычислятся как число коммитов в текущей ветке между меткой reference_version и последним коммитом
# Ветка и дата последнего коммита записывается в переменные, откуда записывается в лог
CURRENT_BRANCH = $$system(git rev-parse --abbrev-ref HEAD)
unix:  GIT_CURRENT_VERSION = $$system(git log start..$${CURRENT_BRANCH} --oneline --first-parent | wc -l)
win32: GIT_CURRENT_VERSION = $$system(git log start..$${CURRENT_BRANCH} --oneline --first-parent | find /c /v \"\")
LAST_COMMIT_DATE = $$system("git --no-pager log -1 --pretty=format:'%ad' --date=format:'%Y-%m-%d %H:%M:%S'")
LAST_COMMIT_SHA = $$system(git rev-parse HEAD)

DEFINES += CURRENT_BRANCH=$${CURRENT_BRANCH}
DEFINES += GIT_CURRENT_VERSION=$${GIT_CURRENT_VERSION}
# такая запись нужна, т.к. строка LAST_COMMIT_DATE выглядит так 2020-05-20 17:24:07 +0300 (c пробелами)
# в выводе компилятора выглядит так: -DLAST_COMMIT_DATE="\"2020-05-20 17:24:07 +0300\""
DEFINES += LAST_COMMIT_DATE=\"\\\""$${LAST_COMMIT_DATE}"\\\"\"

message(current branch: $${CURRENT_BRANCH})
message(commits to reference_version: $${GIT_CURRENT_VERSION})
message(last commit date: $${LAST_COMMIT_DATE})

QMAKE_SUBSTITUTES += version.h.in # недокументированная фича. файл шаблона, в который qmake подставит значение и создаст файл version.h
# сделал для того, чтобы при обновлении коммита и запуске qmake пересоздался файл version.h с версией коммита и пересобрался зависимый от него файл engine.cpp
# т.е. без полной пересборки обновить версию отображаемую в программе.

TEMPLATE = app

TARGET = pin_machine

include($$PWD/log/log.pri)

HEADERS += \
    serial.h \
    engine.h \
    utils.h \
    singleton.h \

SOURCES += \
    main.cpp \
    engine.cpp \
    serial.cpp \
    utils.cpp \

target.path = $${INSTALL_PATH}

message($${INSTALL_PATH})
INSTALLS += target
