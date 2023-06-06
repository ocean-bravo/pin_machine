isEmpty(QMAKE_TOOLS_PRI) {
QMAKE_TOOLS_PRI = used

# Флаг для вывода дополнительной информации
ENABLE_QMAKE_DEBUG = false

# Вспомогательные функции

# Проверяет существование пути или файла, если нет - останавливает парсинг.
defineReplace(checked) {
    NAME = $$system_path($$1)
    !exists($$NAME): error(Path or file $$NAME not exists. $$2)
    return ($$1)
}

# Печатает значение переменной в виде ПЕРЕМЕННАЯ = ЗНАЧЕНИЕ или
# ПЕРЕМЕННАЯ: значение1 ... значение N,
# если значение является списком
defineReplace(type) {

    contains(ENABLE_QMAKE_DEBUG, true) {
        VAR = $$unique($$1)
        isEmpty(VAR): VAR = [ empty ]

        count(VAR, 1) {
            message($$1 = $$VAR)
        } else {
            message($$1: $$VAR)
        }
    }

    return ($$1)
}

# Печатает строку с обрамлением как заголовок
defineReplace(typeHeader) {
    contains(ENABLE_QMAKE_DEBUG, true) {
        unix {
            message(========================================================================)
            message(    $$1)
            message(========================================================================)
        }
        win32 {
            system(@echo ------------------------------------------------------------------------------)
            system(@echo.)
            system(@echo $$1)
            system(@echo.)
            system(@echo ------------------------------------------------------------------------------)
        }
    }
    return ($$1)
}

# Проверяет свойство на равенство true (в любом регистре)
defineTest(isTrue) {
    TMP_LOWER = $$lower($$unique($$1))
    isEqual(TMP_LOWER, true) : return(true)
    return(false)
}

# Возвращает нормализованное значение логической переменной (второй параметр - значение по умолчанию)
defineReplace(setBool) {
    TMP = $$unique($$1)
    isEmpty(TMP) {
        TMP = $$2
        SUFFIX = [ default ]
    }
    isTrue(TMP) {
        TMP = true
    } else {
        TMP = false
    }
    contains(ENABLE_QMAKE_DEBUG, true):message($$1 = $${TMP} $${SUFFIX})
    return($$TMP)
}

# Присваивает путь по умолчанию заданной переменной, если она не определена.
# Относительный путь исчисляется от каталога HOME (HOMEPATH под Windows, если HOME не определена)
# абсолютный - остается без изменений.
# Печатает значение переменной в виде VARIABLE = $$VARIABLE
# если использовалось значение по умолчанию - добавляет суффикc [ default ]
defineReplace(setPath) {
    TMP = $$unique($$1)
    isEmpty(TMP) {
        VAR = $$2

        !isEqual(VAR, $$absolute_path($$VAR)) {
            HOME = $$(HOME)
            win32: isEmpty(HOME): HOME = $$(HOMEPATH)

            VAR = $$absolute_path($${HOME}/$${VAR})
        }

        !isEmpty(VAR) {
            TMP = $$system_path($$VAR)
            SUFFIX = [ default ]
        } else {
            SUFFIX = [ empty ]
        }
    } else {
        # В описании функции указано, что путь присваивается, если переменная
        # неопределена, поэтому неясно, нужен ли этот код, он не используется
        TMP = $$system_path($$absolute_path($${TMP}, $${REPOS_ROOT}))
    }

    contains(ENABLE_QMAKE_DEBUG, true):message($$1 = $${TMP} $${SUFFIX})
    return($${TMP})
}

# Возвращает команды для печати строки-заголовка
defineReplace(header) {
    win32 {
        RESULT = @echo "------------------------------------------------------------------------------" & \
            @echo. & \
            @echo "$$1" & \
            @echo. & \
            @echo "------------------------------------------------------------------------------"
    }
    return ($$RESULT)
}

# Возвращает команду копирования каталога из $$1 в $$2 с преобразованием
# разделителей путей в нативные и обрамлением путей в кавычки.
defineReplace(copyDir) {
    unix: SUFFIX = /.
    return ($$QMAKE_COPY_DIR \"$$system_path($$1)$${SUFFIX}\" \"$$system_path($$2)\")
}

# Возвращает команду копирования файла $$1 в каталог $$2 с преобразованием
# разделителей путей в нативные и обрамлением путей в кавычки.
defineReplace(copyFile) {
    return ($$QMAKE_COPY \"$$system_path($$1)\" \"$$system_path($$2)\")
}

# Возвращает команду копирования файлов по маске $$1 в каталог $$2 с преобразованием
# разделителей путей в нативные и экранированием пробелов.
defineReplace(copyFilesByPattern) {
    COMMAND = $$QMAKE_COPY

    win32 {
        PATTERN = $$system_path($$1)
        COMMAND += $$replace(PATTERN, " ", "\ ")
    }
    unix {
        COMMAND += -d

        FILES = $$files($$1)
        for(FILE, FILES) {
            FILE = $$system_path($$FILE)
            COMMAND += $$replace(FILE, " ", "\ ")
        }
    }

    PATH = $$system_path($$2)
    COMMAND += $$replace(PATH, " ", "\ ")

    return ($$COMMAND)
}

# Создает директорию в директории установки приложения.
# Можно передать 1 или 2 параметра, будут созданы директории одна внутри другой
defineTest(createDirectory) {
    destDir = $$1
    childDir = $$2

    # Можно бы эти if убрать, т.е. в случае childDir пустого, было бы mkdir destDir/ т.е. добавлен / в конце. Он для этой команды ни на что не влияет.
    isEmpty(childDir) {
        target = $${destDir}_dir # например, если передана директория test в качестве аргумента, будет присвоено test_dir
        $${target}.commands = $$sprintf($$QMAKE_MKDIR_CMD, $${INSTALL_PATH}$${DELIMETER}$$destDir)
    }

    !isEmpty(childDir) {
        target = $${destDir}_$${childDir}_dir # например, если переданы 2 директории test и learn в качестве аргумента, будет присвоено test_learn_dir
        $${target}.commands = $$sprintf($$QMAKE_MKDIR_CMD, $${INSTALL_PATH}$${DELIMETER}$$destDir$${DELIMETER}$$childDir)
    }

    $${target}.path = $${INSTALL_PATH}

    #message($${target})
    #message($${target}.path)
    #message($${target}.commands)

    INSTALLS += $$target

    export($${target}.path)
    export($${target}.commands)
    export(INSTALLS)

    return(true) ## так надо. test функция должна возвращать bool
}

# Копирует файлы в директорию установки приложения.
# 1 параметр - файлы источника, может быть список
# 2 параметр - путь назначения. Может быть пустой. Тогда копируется в корень директории приложения. Ожидается в виде test или test/learn.
defineTest(copyFiles) {
    target = $$list() # генерируется уникальное имя
    files_list = $$1
    dest = $$2

    isEmpty(dest) {
         $${target}.path = $${INSTALL_PATH}
    }

    !isEmpty(dest) {
         $${target}.path = $${INSTALL_PATH}/$$dest
    }

    #eval(target_path = \$\$$${target}.path) # без eval не получится
    #message(target path: $${target}.path)
    #message("file list": $$files_list)

    $${target}.files = $$files_list

    INSTALLS += $$target

    export($${target}.path)
    export($${target}.files)
    export(INSTALLS)

    return(true)
}

# спец версия копирования исполняемых файлов
# нужна для убирания предупреждения, вроде "strip: /home/mint/deploy/crimp/crimp.sh: file format not recognized"
# Проблема из-за флага +x на файле. Только в релизной версии.
# нужно убрать в исходном коде флаг +x, и копировать этой фукнцией. Она добавит +x
# TODO: сделать обобщенную версию функции, для debug и release
# не нужна для win32. TODO: сделать для win32
defineTest(copyShellFile) {
    target = $$list() # генерируется уникальное имя
    shellFile = $$1
    dest = $$2

    isEmpty(dest) {
         $${target}.path = $${INSTALL_PATH}
    }

    !isEmpty(dest) {
         $${target}.path = $${INSTALL_PATH}$${DELIMETER}$$dest
    }

    #eval(target_path = \$\$$${target}.path) # без eval не получится
    #message($$target_path)
    #message($$files_list)

    $${target}.files = $$shellFile
    $${target}.extra = chmod 755 $$shellFile # вот он костыль

    INSTALLS += $$target

    export($${target}.path)
    export($${target}.files)
    export(INSTALLS)

    return(true)
}

}
