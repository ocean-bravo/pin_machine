TEMPLATE = aux

DISTFILES = $$PWD/*.sh \
            $$PWD/*.png \
            $$PWD/*.ini \

copyFiles($$DISTFILES) # Копирует файлы в директорию установки приложения
