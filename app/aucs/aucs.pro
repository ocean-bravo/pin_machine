TEMPLATE = aux

DISTFILES = $$PWD/*.sh \
            $$PWD/*.png \
            $$PWD/*.ini \
    rebuild.sh

copyFiles($$DISTFILES) # Копирует файлы в директорию установки приложения

copyFiles($$PWD/find_blob_scenes/*, find_blob_scenes)
