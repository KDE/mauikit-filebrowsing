#pragma once
#include <QString>
#include <QQuickImageProvider>
#include "filebrowsing_export.h"

/**
 * @private
 */
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

class FILEBROWSING_EXPORT Thumbnailer : public QQuickAsyncImageProvider
{
public:
    QQuickImageResponse *requestImageResponse(const QString &id, const QSize &requestedSize) override;
};

