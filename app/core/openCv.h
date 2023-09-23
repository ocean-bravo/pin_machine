#pragma once

#include <QString>
#include <QObject>
#include <QImage>
#include <QMutex>

#include <vector>

class OpenCvPrivate;

class OpenCv : public QObject
{
    Q_OBJECT

public:
    OpenCv();
    ~OpenCv();

    void searchCircles(QImage img);
    void blobDetector(QImage img);

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
    void searchCircles(QImage img);
    void blobDetector(QImage img);

signals:
    void imageChanged(QImage);
    void blobChanged(QImage);

private:
    QMutex _jobDone;
};

