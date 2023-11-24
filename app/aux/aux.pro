TEMPLATE = aux

DISTFILES = $$PWD/*.sh \
            $$PWD/*.png \

copyFiles($$DISTFILES) # Копирует файлы в директорию установки приложения
