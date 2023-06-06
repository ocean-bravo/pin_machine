isEmpty(PARAM_INSTALL_PATH_PRI) {
PARAM_INSTALL_PATH_PRI = used

# INSTALL_PATH
# Путь установки. По умолчанию принимает значение
# Linux: $HOME/deploy/crimp
# Windows: %HOME%/deploy/crimp, либо
# %HOMEPATH%/deploy/crimp, если HOME не определена.

INSTALL_PATH = $$(INSTALL_PATH)
INSTALL_PATH = $$setPath(INSTALL_PATH, deploy/pin_machine)

}
