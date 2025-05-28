
#include "tagslist.h"
#include "tagging.h"
#include <QTimer>

TagsList::TagsList(QObject *parent)
    : MauiList(parent)
    , m_refreshTimer(new QTimer(this))
{
    m_refreshTimer->setInterval(100);
    m_refreshTimer->setSingleShot(true);

    connect(m_refreshTimer, &QTimer::timeout, this, &TagsList::setList);

    m_tagging = Tagging::getInstance();
    connect(m_tagging, &Tagging::tagged, [this](QVariantMap)
            {
                if(this->m_urls.isEmpty())
                {
                    this->refresh();
                }
            });

    connect(m_tagging, &Tagging::tagRemoved, this, &TagsList::refresh);

    connect(m_tagging, &Tagging::urlTagged, [this](QString url, QString)
            {
                qDebug() << "Tagging url tagged" << url << m_urls.isDetached() << this;

                if(this->m_urls.contains(url))
                    this->refresh();
            });

    connect(m_tagging, &Tagging::urlTagRemoved, [this](QString, QString url)
            {
                if(this->m_urls.contains(url))
                    this->refresh();
            });

    connect(this, &TagsList::urlsChanged, this, &TagsList::setList);
    connect(this, &TagsList::strictChanged, this, &TagsList::setList);
}

TagsList::~TagsList()
{
    qDebug() << "Deleting single tagging instance" << this;
    m_tagging->disconnect();
    m_tagging = nullptr;
}

void TagsList::setList()
{
    Q_EMIT this->preListChanged();
    this->list.clear();
    this->list = getDBTags();
    
    Q_EMIT this->tagsChanged();
    Q_EMIT this->postListChanged();
}

FMH::MODEL_LIST TagsList::getDBTags() const
{
    
    if (this->m_urls.isEmpty())
    {
        return FMH::toModelList(m_tagging->getAllTags(this->strict));
        
    }else
    {
        return std::accumulate(this->m_urls.constBegin(), this->m_urls.constEnd(), FMH::MODEL_LIST(), [this](FMH::MODEL_LIST &list, const QString &url) {
            list << FMH::toModelList(m_tagging->getUrlTags(url, this->strict));
            return list;
        });
    }
}

void TagsList::refresh()
{
    m_refreshTimer->start();
}

bool TagsList::insert(const QString &tag)
{
    if (m_tagging->tag(tag.trimmed()))
        return true;

    return false;
}

void TagsList::insertToUrls(const QString &tag)
{
    if (m_urls.isEmpty())
        return;

    for (const auto &url : std::as_const(this->m_urls))
        m_tagging->tagUrl(url, tag);
}

void TagsList::updateToUrls(const QStringList &tags) //if there is only one url update the tags if there are more than one url then add the new tags
{
    if (this->m_urls.isEmpty())
        return;
    
    for (const auto &url : std::as_const(this->m_urls))
    {
        for(const auto &tag : tags)
        {
            m_tagging->tagUrl(url, tag);
        }
    }
}

void TagsList::removeFromUrls(const int &index)
{
    if (index >= this->list.size() || index < 0)
        return;

    if (this->m_urls.isEmpty())
        return;

    const auto tag = this->list[index][FMH::MODEL_KEY::TAG];

    for (const auto &url : std::as_const(m_urls))
    {
        m_tagging->removeUrlTag(url, tag);
    }

    this->remove(index);
}

void TagsList::removeFromUrls(const QString &tag)
{
    const auto index = indexOf(FMH::MODEL_KEY::TAG, tag);
    removeFromUrls(index);
}

bool TagsList::remove(const int &index)
{
    if (index >= this->list.size() || index < 0)
        return false;

    Q_EMIT this->preItemRemoved(index);
    this->list.removeAt(index);
    Q_EMIT this->tagsChanged();
    Q_EMIT this->postItemRemoved();

    return true;
}

void TagsList::removeFrom(const int &index, const QString &url)
{
    if (index >= this->list.size() || index < 0)
        return;

    if (m_tagging->removeUrlTag(url, this->list[index][FMH::MODEL_KEY::TAG]))
        this->remove(index);
}

void TagsList::erase(const int &index)
{
    Q_UNUSED(index)
}

const FMH::MODEL_LIST &TagsList::items() const
{
    return this->list;
}

bool TagsList::getStrict() const
{
    return this->strict;
}

void TagsList::setStrict(const bool &value)
{
    if (this->strict == value)
        return;

    this->strict = value;
    Q_EMIT this->strictChanged();
}

QStringList TagsList::getTags() const
{   
    return FMH::modelToList(this->list, FMH::MODEL_KEY::TAG);
}

QStringList TagsList::getNewTags() const
{   
    auto allTags = getTags();
    auto existingTags = FMH::modelToList(getDBTags(), FMH::MODEL_KEY::TAG);

    QStringListIterator i(existingTags);
    while(i.hasNext())
    {
        allTags.removeAll(i.next());
    }

    return allTags;
}

QStringList TagsList::getUrls() const
{
    return this->m_urls;
}

void TagsList::setUrls(const QStringList &value)
{
    if (this->m_urls == value)
        return;

    this->m_urls = value;
    Q_EMIT this->urlsChanged();
}

void TagsList::append(const QString &tag)
{
    this->append(FMH::MODEL {{FMH::MODEL_KEY::TAG, tag}, {FMH::MODEL_KEY::ICON, QStringLiteral("tag")}});
}

void TagsList::appendItem(const QVariantMap &tag)
{
    this->append(FMH::toModel(tag));
}

void TagsList::append(const FMH::MODEL &tag)
{
    if (this->exists(FMH::MODEL_KEY::TAG, tag[FMH::MODEL_KEY::TAG]))
        return;

    Q_EMIT this->preItemAppended();
    this->list << tag;
    Q_EMIT this->postItemAppended();
    Q_EMIT this->tagsChanged();
}

void TagsList::append(const QStringList &tags)
{
    for (const auto &tag : std::as_const(tags))
    {
        this->append(tag);
    }
}

bool TagsList::contains(const QString &tag)
{
    return this->exists(FMH::MODEL_KEY::TAG, tag);
}

void TagsList::componentComplete()
{
    this->setList();
}
