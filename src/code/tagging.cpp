/*
 *   Copyright 2018 Camilo Higuita <milo.h@aol.com>
 *
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU Library General Public License as
 *   published by the Free Software Foundation; either version 2, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details
 *
 *   You should have received a copy of the GNU Library General Public
 *   License along with this program; if not, write to the
 *   Free Software Foundation, Inc.,
 *   51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */
#include "tagging.h"

#include <QMimeDatabase>

#include <QFileInfo>
#include <QDateTime>
#include <QDebug>
#include <QRegularExpression>

#include <QThread>
#include <QCoreApplication>

#include <QSqlError>
#include <QSqlQuery>
#include <QSqlRecord>

#include "fmstatic.h"
#include "tagdb.h"

Q_GLOBAL_STATIC(Tagging, taggingInstance)

Tagging::~Tagging() {}

Tagging::Tagging() : QObject()
{
    this->setApp();
    connect(qApp, &QCoreApplication::aboutToQuit, [this]()
    {
        qDebug() << "Lets remove Tagging singleton instance and all opened Tagging DB connections.";
        
        qDeleteAll(m_dbs);
        m_dbs.clear();
    });
}

TAGDB * Tagging::db()
{
    if(m_dbs.contains(QThread::currentThread()))
    {
        qDebug() << "Using existing TAGGINGDB instance";
        
        return m_dbs[QThread::currentThread()];
    }
    
    qDebug() << "Creating new TAGGINGDB instance";
    
    auto new_db = new TAGDB;
    m_dbs.insert(QThread::currentThread(), new_db);
    return new_db;
}

Tagging *Tagging::getInstance()
{
    return taggingInstance();
}

const QVariantList Tagging::get(const QString &queryTxt, std::function<bool(QVariantMap &item)> modifier)
{
    QVariantList mapList;

    auto query = this->db()->getQuery(queryTxt);

    if (query.exec()) {
        const auto keys = FMH::MODEL_NAME.keys();

        while (query.next()) {
            QVariantMap data;
            for (const auto &key : keys) {
                
                if (query.record().indexOf(FMH::MODEL_NAME[key]) > -1) {
                    data[FMH::MODEL_NAME[key]] = query.value(FMH::MODEL_NAME[key]).toString();
                }
            }

            if (modifier) {
                if (!modifier(data))
                {
                    continue;
                }
            }

            mapList << data;
        }

    } else
    {
        qDebug() << query.lastError() << query.lastQuery();
    }

    return mapList;
}

bool Tagging::tagExists(const QString &tag, const bool &strict) 
{
    return !strict ? this->db()->checkExistance(TAG::TABLEMAP[TAG::TABLE::TAGS], FMH::MODEL_NAME[FMH::MODEL_KEY::TAG], tag)
            : this->db()->checkExistance(QString(QStringLiteral("select t.tag from APP_TAGS where t.org = '%1' and t.tag = '%2'"))
            .arg(this->appOrg, tag));
}

bool Tagging::urlTagExists(const QString &url, const QString &tag) 
{
    return this->db()->checkExistance(QString(QStringLiteral("select * from TAGS_URLS where url = '%1' and tag = '%2'")).arg(url, tag));
}

void Tagging::setApp()
{
    this->appName = qApp->applicationName();
    this->appComment = QString();
    this->appOrg = qApp->organizationDomain().isEmpty() ? QString(QStringLiteral("org.maui.%1")).arg(this->appName) : qApp->organizationDomain();
    this->app(); // here register the app
}

bool Tagging::tag(const QString &tag, const QString &color, const QString &comment)
{
    if (tag.isEmpty())
        return false;
    
    if(tag.contains(QStringLiteral(" ")) || tag.contains(QStringLiteral("\n")))
    {
        return false;
    }
    
    QVariantMap tag_map {
        {FMH::MODEL_NAME[FMH::MODEL_KEY::TAG], tag},
        {FMH::MODEL_NAME[FMH::MODEL_KEY::COLOR], color},
        {FMH::MODEL_NAME[FMH::MODEL_KEY::ADDDATE], QDateTime::currentDateTime().toString(Qt::TextDate)},
        {FMH::MODEL_NAME[FMH::MODEL_KEY::COMMENT], comment},
    };
    
    this->db()->insert(TAG::TABLEMAP[TAG::TABLE::TAGS], tag_map);
    
    const QVariantMap appTag_map {
        {FMH::MODEL_NAME[FMH::MODEL_KEY::TAG], tag},
        {FMH::MODEL_NAME[FMH::MODEL_KEY::ORG], this->appOrg},
        {FMH::MODEL_NAME[FMH::MODEL_KEY::ADDDATE], QDateTime::currentDateTime().toString(Qt::TextDate)}};

    if (this->db()->insert(TAG::TABLEMAP[TAG::TABLE::APP_TAGS], appTag_map)) {
        setTagIconName(tag_map);
        Q_EMIT this->tagged(tag_map);
        return true;
    }

    return false;
}

bool Tagging::tagUrl(const QString &url, const QString &tag, const QString &color, const QString &comment)
{
    const auto myTag = tag.trimmed();

    this->tag(myTag, color, comment);

    QMimeDatabase mimedb;

    QVariantMap tag_url_map {{FMH::MODEL_NAME[FMH::MODEL_KEY::URL], url},
                             {FMH::MODEL_NAME[FMH::MODEL_KEY::TAG], myTag},
                             {FMH::MODEL_NAME[FMH::MODEL_KEY::TITLE], QFileInfo(url).baseName()},
                             {FMH::MODEL_NAME[FMH::MODEL_KEY::MIME], mimedb.mimeTypeForFile(url).name()},
                             {FMH::MODEL_NAME[FMH::MODEL_KEY::ADDDATE], QDateTime::currentDateTime()},
                             {FMH::MODEL_NAME[FMH::MODEL_KEY::COMMENT], comment}};

    if(this->db()->insert(TAG::TABLEMAP[TAG::TABLE::TAGS_URLS], tag_url_map))
    {
        qDebug() << "tagging url" << url <<tag;
        Q_EMIT this->urlTagged(url, myTag);
        
        //         auto fileMetaData = KFileMetaData::UserMetaData(QUrl::fromUserInput(url).toLocalFile());
        //         fileMetaData.setTags({tag});
        
        return true;
    }
    
    return false;
}

bool Tagging::updateUrlTags(const QString &url, const QStringList &tags, const bool &strict)
{
    this->removeUrlTags(url, strict);

    for (const auto &tag : std::as_const(tags))
    {
        this->tagUrl(url, tag);
    }

    return true;
}

bool Tagging::updateUrl(const QString &url, const QString &newUrl) 
{
    return this->db()->update(TAG::TABLEMAP[TAG::TABLE::TAGS_URLS], {{FMH::MODEL_KEY::URL, newUrl}}, {{FMH::MODEL_NAME[FMH::MODEL_KEY::URL], url}});
}

QVariantList Tagging::getUrlsTags(const bool &strict)  //all used tags, meaning, all tags that are used with an url in tags_url table
{
    const auto query = !strict ? QString(QStringLiteral("select distinct t.* from TAGS t inner join TAGS_URLS turl where t.tag = turl.tag order by t.tag asc")) :
                                 QString(QStringLiteral("select distinct t.* from TAGS t inner join APP_TAGS at on at.tag = t.tag inner join TAGS_URLS turl on t.tag = turl.tag where at.org = '%1' order by t.tag asc")).arg(this->appOrg);

    return this->get(query, &setTagIconName);
}

bool Tagging::setTagIconName(QVariantMap &item)
{
    item.insert(QStringLiteral("icon"), item.value(QStringLiteral("tag")).toString() == QStringLiteral("fav") ? QStringLiteral("love") : QStringLiteral("tag"));
    return true;
}

QVariantList Tagging::getAllTags(const bool &strict) 
{
    return !strict ? this->get(QStringLiteral("select * from tags"), &setTagIconName)
                   : this->get(QString(QStringLiteral("select t.* from TAGS t inner join APP_TAGS at on t.tag = at.tag where at.org = '%1'")).arg(this->appOrg),
                               &setTagIconName);
}

QVariantList Tagging::getUrls(const QString &tag, const bool &strict, const int &limit, const QString &mimeType, std::function<bool(QVariantMap &item)> modifier)
{
    return !strict ? this->get(QString(QStringLiteral("select distinct * from TAGS_URLS where tag = '%1' and mime like '%2%' limit %3")).arg(tag, mimeType, QString::number(limit)), modifier)
                   : this->get(QString(QStringLiteral("select distinct turl.*, t.color, t.comment as tagComment from TAGS t "
                                                      "inner join APP_TAGS at on t.tag = at.tag "
                                                      "inner join TAGS_URLS turl on turl.tag = t.tag "
                                                      "where at.org = '%1' and turl.mime like '%4%' "
                                                      "and t.tag = '%2' limit %3"))
                               .arg(this->appOrg, tag, QString::number(limit), mimeType),
                               modifier);
}

QVariantList Tagging::getUrlTags(const QString &url, const bool &strict)
{
    return !strict ? this->get(QString(QStringLiteral("select distinct turl.*, t.color, t.comment as tagComment from tags t inner join TAGS_URLS turl on turl.tag = t.tag where turl.url  = '%1'")).arg(url))
                   : this->get(QString(QStringLiteral("select distinct t.* from TAGS t inner join APP_TAGS at on t.tag = at.tag inner join TAGS_URLS turl on turl.tag = t.tag "
                                                      "where at.org = '%1' and turl.url = '%2'"))
                               .arg(this->appOrg, url));
}

bool Tagging::removeUrlTags(const QString &url, const bool &strict) // same as removing the url from the tags_urls
{
    Q_UNUSED(strict)
    return this->removeUrl(url);
}

bool Tagging::removeUrlTag(const QString &url, const QString &tag)
{
    qDebug() << "Remove url tag" << url << tag;
    FMH::MODEL data {{FMH::MODEL_KEY::URL, url}, {FMH::MODEL_KEY::TAG, tag}};
    if(this->db()->remove(TAG::TABLEMAP[TAG::TABLE::TAGS_URLS], data))
    {
        Q_EMIT this->urlTagRemoved(tag, url);
        return true;
    }
    
    return false;
}

bool Tagging::removeUrl(const QString &url)
{
    if(this->db()->remove(TAG::TABLEMAP[TAG::TABLE::TAGS_URLS], {{FMH::MODEL_KEY::URL, url}}))
    {
        Q_EMIT this->urlRemoved(url);
    }
    
    return false;
}

bool Tagging::app()
{
    qDebug() << "REGISTER APP" << this->appName << this->appOrg << this->appComment;
    const QVariantMap app_map {
        {FMH::MODEL_NAME[FMH::MODEL_KEY::NAME], this->appName},
        {FMH::MODEL_NAME[FMH::MODEL_KEY::ORG], this->appOrg},
        {FMH::MODEL_NAME[FMH::MODEL_KEY::ADDDATE], QDateTime::currentDateTime()},
        {FMH::MODEL_NAME[FMH::MODEL_KEY::COMMENT], this->appComment},
    };

    return this->db()->insert(TAG::TABLEMAP[TAG::TABLE::APPS], app_map);
}

bool Tagging::removeTag(const QString& tag, const bool &strict)
{    
    if(strict)
    {
        FMH::MODEL data0 {{FMH::MODEL_KEY::TAG, tag}, {FMH::MODEL_KEY::ORG, this->appOrg}};

        if(this->db()->remove(TAG::TABLEMAP[TAG::TABLE::APP_TAGS], data0))
        {
            return true;
        }

    }else
    {
        FMH::MODEL data1 {{FMH::MODEL_KEY::TAG, tag}};

        if(this->db()->remove(TAG::TABLEMAP[TAG::TABLE::TAGS_URLS], data1))
        {
            FMH::MODEL data2 {{FMH::MODEL_KEY::TAG, tag}, {FMH::MODEL_KEY::ORG, this->appOrg}};

            if(this->db()->remove(TAG::TABLEMAP[TAG::TABLE::APP_TAGS], data2))
            {
                if(this->db()->remove(TAG::TABLEMAP[TAG::TABLE::TAGS], data1))
                {
                    Q_EMIT this->tagRemoved(tag);
                    return true;
                }
            }
        }
    }


    
    return false;
}

static bool doNameFilter(const QString &name, const QStringList &filters)
{
    const auto filtersAccumulate = std::accumulate(filters.constBegin(), filters.constEnd(), QVector<QRegularExpression> {}, [](QVector<QRegularExpression> &res, const QString &filter) -> QVector<QRegularExpression> {
            QString wildcardExp = QRegularExpression::wildcardToRegularExpression(filter).replace(QStringLiteral("[^/]"), QStringLiteral("."));
            QRegularExpression reg(wildcardExp, QRegularExpression::CaseInsensitiveOption);
            res.append(reg);
            return res;
});
    
    for (const auto &filter : filtersAccumulate) {
        qDebug() << "trying to match" << name << filter;
        if (filter.match(name).hasMatch()) {
            qDebug() << "trying to match" << true;

            return true;
        }
    }
    return false;
}

QList<QUrl> Tagging::getTagUrls(const QString &tag, const QStringList &filters, const bool &strict, const int &limit, const QString &mime)
{
    QList<QUrl> urls;
    
    std::function<bool(QVariantMap & item)> filter = nullptr;
    
    if (!filters.isEmpty())
    {
        filter = [filters](QVariantMap &item) -> bool
        {
            return doNameFilter(FMH::mapValue(item, FMH::MODEL_KEY::URL), filters);
        };
    }
    
    const auto tagUrls = getUrls(tag, strict, limit, mime, filter);
    for (const auto &data : tagUrls)
    {
        const auto url = QUrl(data.toMap()[FMH::MODEL_NAME[FMH::MODEL_KEY::URL]].toString());
        if (url.isLocalFile() && !FMH::fileExists(url))
            continue;
        urls << url;
    }

    return urls;
}

FMH::MODEL_LIST Tagging::getTags(const int &limit)
{
    Q_UNUSED(limit);
    FMH::MODEL_LIST data;
    const auto tags = getAllTags(false);
    for (const auto &tag : tags)
    {
        const QVariantMap item = tag.toMap();
        const auto label = item.value(FMH::MODEL_NAME[FMH::MODEL_KEY::TAG]).toString();
        
        data << FMH::MODEL {{FMH::MODEL_KEY::PATH, FMStatic::PATHTYPE_URI[FMStatic::PATHTYPE_KEY::TAGS_PATH] + label},
        {FMH::MODEL_KEY::ICON, item.value(FMH::MODEL_NAME[FMH::MODEL_KEY::ICON], QStringLiteral("tag")).toString()},
        {FMH::MODEL_KEY::MODIFIED, QDateTime::fromString(item.value(FMH::MODEL_NAME[FMH::MODEL_KEY::ADDDATE]).toString(), Qt::TextDate).toString()},
        {FMH::MODEL_KEY::IS_DIR, QStringLiteral("true")},
        {FMH::MODEL_KEY::LABEL, label},
        {FMH::MODEL_KEY::TYPE, FMStatic::PathTypeLabel(FMStatic::PATHTYPE_KEY::TAGS_PATH)}};
}

return data;
}

FMH::MODEL_LIST Tagging::getUrlTags(const QUrl &url)
{
    return FMH::toModelList(this->getUrlTags(url.toString(), false));
}

bool Tagging::addTagToUrl(const QString tag, const QUrl &url)
{
    return this->tagUrl(url.toString(), tag);
}

bool Tagging::removeTagToUrl(const QString tag, const QUrl &url)
{
    return this->removeUrlTag(url.toString(), tag);
}

bool Tagging::toggleFav(const QUrl &url)
{
    if (this->isFav(url))
        return this->unFav(url);
    
    return this->fav(url);
}

bool Tagging::fav(const QUrl &url)
{
    return this->tagUrl(url.toString(), QStringLiteral("fav"), QStringLiteral("#e91e63"));
}

bool Tagging::unFav(const QUrl &url) 
{
    return this->removeUrlTag(url.toString(), QStringLiteral("fav"));
}

bool Tagging::isFav(const QUrl &url, const bool &strict)
{
    Q_UNUSED(strict)
    
    return urlTagExists(url.toString(), QStringLiteral("fav"));
}

