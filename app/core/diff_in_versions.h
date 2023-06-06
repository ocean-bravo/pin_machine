#pragma once

#include <QtGlobal>
#include <QTextStream>

#if QT_VERSION < QT_VERSION_CHECK(5, 14, 0)
inline QTextStream& endline(QTextStream &s)
{
    return endl(s);
}
#endif

#if QT_VERSION >= QT_VERSION_CHECK(5, 14, 0)
inline QTextStream& endline(QTextStream &s)
{
    return Qt::endl(s);
}
#endif
