QT += quick serialport widgets network gui printsupport concurrent quickcontrols2 quickwidgets core-private

CONFIG += console qtquickcompiler qml_debug

TRANSLATIONS += core_language_de.ts core_language_pl.ts core_language_fr.ts


DEFINES += CURRENT_BRANCH=$${CURRENT_BRANCH}
DEFINES += GIT_CURRENT_VERSION=$${GIT_CURRENT_VERSION}

message(current branch: $${CURRENT_BRANCH})
message(commits to reference_version: $${GIT_CURRENT_VERSION})
message(last commit date: $${LAST_COMMIT_DATE})

QMAKE_SUBSTITUTES += version.h.in # недокументированная фича. файл шаблона, в который qmake подставит значение и создаст файл version.h
# сделал для того, чтобы при обновлении коммита и запуске qmake пересоздался файл version.h с версией коммита и пересобрался зависимый от него файл engine.cpp
# т.е. без полной пересборки обновить версию отображаемую в программе.

TEMPLATE = app

TARGET = pin_machine

include($$PWD/log/log.pri)

INCLUDEPATH += /usr/include/opencv4/
LIBS += -L/usr/lib/x86_64-linux-gnu/ \
-lopencv_aruco -lopencv_bgsegm -lopencv_bioinspired \
-lopencv_calib3d -lopencv_ccalib -lopencv_core -lopencv_datasets \
-lopencv_dnn_objdetect -lopencv_dnn -lopencv_dpm -lopencv_face \
-lopencv_features2d -lopencv_flann -lopencv_freetype -lopencv_fuzzy \
-lopencv_hfs -lopencv_highgui -lopencv_imgcodecs \
-lopencv_img_hash -lopencv_imgproc -lopencv_line_descriptor \
-lopencv_ml -lopencv_objdetect -lopencv_optflow -lopencv_phase_unwrapping \
-lopencv_photo -lopencv_plot -lopencv_quality -lopencv_reg -lopencv_rgbd \
-lopencv_saliency -lopencv_shape -lopencv_stereo -lopencv_stitching \
-lopencv_structured_light -lopencv_superres -lopencv_surface_matching -lopencv_text \
-lopencv_tracking -lopencv_videoio -lopencv_video -lopencv_videostab \
-lopencv_ximgproc -lopencv_xobjdetect -lopencv_xphoto \
-lturbojpeg # libturbojpeg0-dev

OTHER_FILES +=

HEADERS += *.h

SOURCES += *.cpp

FORMS += *.ui

target.path = $${INSTALL_PATH}

message($${INSTALL_PATH})
INSTALLS += target
