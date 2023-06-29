#include "thumbnailer.h"

#ifdef KIO_AVAILABLE
#include <KIO/PreviewJob>
#endif

#include <QDebug>
#include <QImage>
#include <QGuiApplication>

QQuickImageResponse *Thumbnailer::requestImageResponse(const QString &id, const QSize &requestedSize)
{
    AsyncImageResponse *response = new AsyncImageResponse(id, requestedSize);
    return response;
}

AsyncImageResponse::AsyncImageResponse(const QString &id, const QSize &requestedSize)
    : m_id(id)
    , m_requestedSize(requestedSize)
{
#ifdef KIO_AVAILABLE
    KIO::PreviewJob::setDefaultDevicePixelRatio(qApp->devicePixelRatio());
    QStringList plugins = KIO::PreviewJob::availablePlugins();
    //     qDebug() << plugins << KIO::PreviewJob::defaultPlugins();
    auto job = new KIO::PreviewJob(KFileItemList() << KFileItem(QUrl::fromUserInput(id)), requestedSize, &plugins);

    connect(job, &KIO::PreviewJob::gotPreview, [this](KFileItem, QPixmap pixmap) {
        m_image = pixmap.toImage();
        Q_EMIT this->finished();
    });

    connect(job, &KIO::PreviewJob::failed, [this](KFileItem) {
        m_error = QStringLiteral("Thumbnail Previewer job failed");
        this->cancel();
        Q_EMIT this->finished();
    });

    job->start();
#endif
}

QQuickTextureFactory *AsyncImageResponse::textureFactory() const
{
    return QQuickTextureFactory::textureFactoryForImage(m_image);
}

QString AsyncImageResponse::errorString() const
{
    return m_error;
}
