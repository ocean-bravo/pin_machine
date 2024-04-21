TEMPLATE = aux

DISTFILES = $$PWD/*.qml \
            $$PWD/*.js \
    OpLabel.qml \
    OpPanelInsertParameters.qml

TRANSLATIONS += gui_language_de.ts \
                gui_language_pl.ts \
                gui_language_fr.ts \

# копирование QML файлов
copyFiles($$DISTFILES, gui)

# копирование изображений
images = $$PWD/images/*.png \
         $$PWD/images/*.jpg \
         $$PWD/images/*.svg \

copyFiles($$images, gui/images)
