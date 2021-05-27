#ifndef THUMBNAILER_H
#define THUMBNAILER_H

#include <QObject>
#include <QString>
#include <QQuickImageProvider>

class AsyncImageResponse : public QQuickImageResponse
{
public:
    AsyncImageResponse(const QString &id, const QSize &requestedSize);
    QQuickTextureFactory *textureFactory() const override;
    QString errorString() const override;

private:
    QString m_id;
    QSize m_requestedSize;
    QImage m_image;
    QString m_error;
};

class Thumbnailer : public QQuickAsyncImageProvider
{
public:
    QQuickImageResponse *requestImageResponse(const QString &id, const QSize &requestedSize) override;
};

#endif // THUMBNAILER_H
