#pragma once

namespace google_breakpad {
    class MinidumpDescriptor;
}

class Breakpad
{
public:
    Breakpad();

    static bool dumpCallback(const google_breakpad::MinidumpDescriptor&, void*, bool succeeded);
};
