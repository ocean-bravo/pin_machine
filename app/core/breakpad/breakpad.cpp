#include "breakpad.h"

#include <QDir>
#include <iostream>

#define typeof __typeof__

#include "client/linux/handler/exception_handler.h"

namespace {
    const char dumpPath[] = "./dumps";
}

Breakpad::Breakpad()
{
    QDir dir;
    dir.mkdir(dumpPath);
}

bool Breakpad::dumpCallback(const google_breakpad::MinidumpDescriptor&, void*, bool succeeded)
{
    std::cout << "breakpad crash dump created " << succeeded << std::endl;
    return succeeded;
}

static google_breakpad::ExceptionHandler exceptionHandler(
        google_breakpad::MinidumpDescriptor(dumpPath),
        nullptr, Breakpad::dumpCallback, nullptr, true, -1);
