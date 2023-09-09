#pragma once

#include <QString>
#include <QObject>
#include <QImage>

#include <vector>
#include <tuple>

class OpenCvPrivate;

class OpenCv : public QObject
{
    Q_OBJECT

public:
    OpenCv();
    ~OpenCv();

    void searchCircles(QImage img, QByteArray ba);
    void blobDetector(QImage img, QByteArray ba);

signals:
        void imageChanged(QImage);
            void blobChanged(QImage);

private:
    OpenCvPrivate* const _impl;
    QScopedPointer<QThread> _thread;
};

class OpenCvPrivate : public QObject
{
    Q_OBJECT

public:
    OpenCvPrivate();
    ~OpenCvPrivate();


    void init();

public slots:
    void searchCircles(QImage img, QByteArray ba);
    void blobDetector(QImage img, QByteArray ba);

signals:
    void imageChanged(QImage);
    void blobChanged(QImage);

private:

    QAtomicInteger<bool> _done = true;

};

