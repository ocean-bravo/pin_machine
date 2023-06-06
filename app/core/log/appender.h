#pragma once

class QString;

class Appender
{
public:
    virtual ~Appender() = default;
    virtual void append(const QString& msg) = 0;
};
