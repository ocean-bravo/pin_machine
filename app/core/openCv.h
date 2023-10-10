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
    using BlobInfo = std::tuple<QImage, std::vector<cv::KeyPoint>>;
    using BlobInfo2 = std::tuple<std::vector<cv::KeyPoint>, QString, QString, int, int>; // координаты центра, размеры изображения



    void searchCirclesLive(QImage img);
    void blobDetectorLive(QImage img);

    void blobDetectorCaptured(QImage img);

    static QImage drawText(const QImage& img, const QString& text);

    void foundBlobs() const;
    Q_INVOKABLE void resetFoundBlobs();

    static QImage drawCross(const QImage& img);


    Q_INVOKABLE void blobDetectorUpdated(QImage img);
    Q_INVOKABLE QString smallRegionBlob() const;

signals:
    void circleChanged(QImage);
    void blobChanged(QImage);

    void smallRegionBlobChanged();

private:
    OpenCv();
    ~OpenCv();

    OpenCvPrivate* const _impl;
    QScopedPointer<QThread> _thread;

    QQueue<QImage> _detectBlobQueue;
    QVector<BlobInfo2> _detectBlobResult;
    QFutureWatcher<OpenCv::BlobInfo> _blobWatcherCaptured;
    QFutureWatcher<OpenCv::BlobInfo> _blobWatcherCapturedSmallRegion;
    QMetaObject::Connection _smallRegConn;
    QString _smallRegionBlob;

    friend class Singleton<OpenCv>;
};

class OpenCvPrivate : public QObject
{
    Q_OBJECT

public:
    OpenCvPrivate();
    void init();

public slots:
    void searchCirclesLive(QImage img);
    void blobDetectorLive(QImage img);

signals:
    void circleChanged(QImage);
    void blobChanged(QImage);

private:
    QImage searchCirclesWorker(QImage img);

    QFutureWatcher<QImage> _circleWatcherLive;
    QFutureWatcher<OpenCv::BlobInfo> _blobWatcherLive;
    QMutex _blobQueueMutex;
};

inline OpenCv& opencv()
{
    return OpenCv::instance();
}
