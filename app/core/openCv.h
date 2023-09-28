#pragma once

#include <QString>
#include <QObject>
#include <QImage>
#include <QQueue>
#include <QFutureWatcher>

#include <vector>
#include <tuple>

#include <opencv2/core/types.hpp>

class OpenCvPrivate;

class OpenCv : public QObject
{
    Q_OBJECT

public:
    using BlobInfo = std::tuple<QImage, std::vector<cv::KeyPoint>>;

    OpenCv();
    ~OpenCv();

    void searchCircles(QImage img);
    void blobDetector(QImage img);

    void addToDetectBlobQueue(QImage img);

    QImage drawText(const QImage& img, const QString& text);

signals:
    void imageChanged(QImage);
    void blobChanged(QImage);

private:
    OpenCvPrivate* const _impl;
    QScopedPointer<QThread> _thread;

    QQueue<QImage> _detectBlobQueue;
};

class OpenCvPrivate : public QObject
{
    Q_OBJECT

public:
    OpenCvPrivate();
    void init();

public slots:
    void searchCircles(QImage img);
    void blobDetector(QImage img);

signals:
    void imageChanged(QImage);
    void blobChanged(QImage);

private:
    QImage searchCirclesWorker(QImage img);

    QFutureWatcher<QImage> _circleWatcher;
    QFutureWatcher<OpenCv::BlobInfo> _blobWatcher;
};

