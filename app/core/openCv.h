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
    using BlobsOnImage = std::tuple<QImage, QVector<Blob>>; // 3 параметр - временно
    //using BlobsOnImage2 = std::tuple<QVector<Blob>, QString, QString, int, int>; // координаты центра, размеры изображения


    static double corr(QImage cap1, QImage cap2);

    void searchCirclesLive(QImage img);
    void blobDetectorLive(QImage img, QVariantMap options);

    void appendToBlobDetectorQueue(QVariantMap options, QImage img);

    Q_INVOKABLE void blobDetectorUpdated(QImage img, QVariantMap options);
    std::tuple<bool, double, double, double> smallRegionBlob() const;

signals:
    void circleChanged(QImage);
    void blobChanged(QImage);

    void smallRegionBlobDetectionFinished();

    void smallRegionBlobImage(QImage);

private:
    OpenCv();
    ~OpenCv();

    OpenCvPrivate* const _impl;
    QScopedPointer<QThread> _thread;

    QQueue<std::tuple<QVariantMap, QImage>> _detectBlobQueue; // Опции обнаружения блобов, само изображение
    //QVector<BlobInfo2> _detectBlobResult;
    QFutureWatcher<OpenCv::BlobsOnImage> _blobWatcherCaptured;
    QFutureWatcher<OpenCv::BlobsOnImage> _blobWatcherCapturedSmallRegion;
    QMetaObject::Connection _smallRegConn;
    std::tuple<bool, double, double, double> _smallRegionBlob;

    friend class Singleton<OpenCv>;
};

class OpenCvPrivate : public QObject
{
    Q_OBJECT

public:
    OpenCvPrivate();

public slots:
    void searchCirclesLive(QImage img);
    void blobDetectorLive(QImage img, QVariantMap options);

signals:
    void circleChanged(QImage);
    void blobChanged(QImage);
    void smallRegionBlobImage(QImage);

private:
    QImage searchCirclesWorker(QImage img);

    QFutureWatcher<QImage> _circleWatcherLive;
    QFutureWatcher<OpenCv::BlobsOnImage> _blobWatcherLive;
    QMutex _blobQueueMutex;
};

inline OpenCv& opencv()
{
    return OpenCv::instance();
}
