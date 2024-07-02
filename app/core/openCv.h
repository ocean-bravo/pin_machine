#pragma once

#include <QString>
#include <QObject>
#include <QImage>
#include <QQueue>
#include <QFutureWatcher>

#include <vector>
#include <tuple>
#include "singleton.h"

#include <opencv2/core/types.hpp>

class OpenCvPrivate;

class OpenCv : public QObject, public Singleton<OpenCv>
{
    Q_OBJECT

public:
    struct Blob
    {
        double xMm = 0.0;
        double yMm = 0.0;
        double diameterMm = 0.0;
    };

    //using Blob = std::tuple<QPointF, double>;
    using BlobsOnImage = std::tuple<QImage, QVector<Blob>, QVariantMap>;
    //using BlobsOnImage2 = std::tuple<QVector<Blob>, QString, QString, int, int>; // координаты центра, размеры изображения

    static double corr(QImage cap1, QImage cap2);

    void searchCirclesLive(QImage img);
    void blobDetectorLive(QImage img, QVariantMap options);

    void appendToBlobDetectorQueue(QImage img, QVariantMap options);
    void clearQueue();

    static OpenCv::BlobsOnImage detectBlobs(QImage img, QVariantMap options);

signals:
    void circleChanged(QImage);
    void queueIsEmpty();

private:
    OpenCv();
    ~OpenCv();

    void blobDetectorCaptured();

    OpenCvPrivate* const _impl;
    QScopedPointer<QThread> _thread;

    QQueue<std::tuple<QImage, QVariantMap>> _detectBlobQueue; // Опции обнаружения блобов, само изображение

    QFutureWatcher<OpenCv::BlobsOnImage> _blobWatcherCaptured;
    QFutureWatcher<OpenCv::BlobsOnImage> _blobWatcherLive;

    mutable QMutex _mutex;

    friend class Singleton<OpenCv>;
};

class OpenCvPrivate : public QObject
{
    Q_OBJECT

public:
    OpenCvPrivate();

public slots:
    void searchCirclesLive(QImage img);

signals:
    void circleChanged(QImage);

private:
    QImage searchCirclesWorker(QImage img);

    QFutureWatcher<QImage> _circleWatcherLive;
};

inline OpenCv& opencv()
{
    return OpenCv::instance();
}
