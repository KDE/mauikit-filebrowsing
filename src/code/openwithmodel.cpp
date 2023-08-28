#include "openwithmodel.h"

#ifdef KIO_AVAILABLE
#include <KApplicationTrader>
#include <KFileItem>
#include <KIO/ApplicationLauncherJob>
#include <KService>
#endif

OpenWithModel::OpenWithModel(QObject* parent) : MauiList(parent)
{
}

void OpenWithModel::componentComplete()
{
    this->setList();
    connect(this, &OpenWithModel::urlsChanged, this, &OpenWithModel::setList);
}

const FMH::MODEL_LIST &OpenWithModel::items() const
{
    return m_list;
}

void OpenWithModel::setUrls(const QStringList& urls)
{
    m_urls = urls;
    Q_EMIT urlsChanged();
}

QStringList OpenWithModel::urls() const
{
    return m_urls;
}

void OpenWithModel::openWith(const int& index)
{
    if(index < 0 && index >= m_list.count())
    {
        return;
    }
#ifdef KIO_AVAILABLE
    KService::Ptr service(new KService(this->m_list[index][FMH::MODEL_KEY::EXECUTABLE]));
    auto *job = new KIO::ApplicationLauncherJob(service, this);
    job->setUrls(QUrl::fromStringList(m_urls));
    job->start();
#endif
}

#ifdef KIO_AVAILABLE
static FMH::MODEL createActionItem(const QExplicitlySharedDataPointer< KService > &service)
{
    FMH::MODEL map
    {
        {FMH::MODEL_KEY::LABEL, service->name().replace(QStringLiteral("&"), QStringLiteral("&&"))},
        {FMH::MODEL_KEY::ICON, service->icon()},
        {FMH::MODEL_KEY::COMMENT, service->comment()},
        {FMH::MODEL_KEY::ID, QStringLiteral("_kicker_fileItem_openWith")},
        {FMH::MODEL_KEY::EXECUTABLE, service->entryPath()}
        //         item["serviceExec"] = service->exec();
    };
    
    return map;
}
#endif

void OpenWithModel::setList()
{
    if(m_urls.isEmpty())
    {
        return;
    }
    
    QUrl url = QUrl::fromUserInput(m_urls.first());
    if(url.isEmpty() || !url.isValid())
    {
        return;
    }
    
    m_list.clear();
    Q_EMIT this->preListChanged();

#ifdef KIO_AVAILABLE
    KFileItem fileItem(url);
    
    const auto services = KApplicationTrader::queryByMimeType(fileItem.mimetype());
    for (const auto &service : services) {
        m_list << createActionItem(service);
    }
    
    Q_EMIT postListChanged();
#endif
}
