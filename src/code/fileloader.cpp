#include "fileloader.h"
#include "fmstatic.h"
#include "tagging.h"

#include <QDebug>
#include <QDirIterator>

using namespace FMH;

std::function<FMH::MODEL(const QUrl &url)> FileLoader::informer = &FMStatic::getFileInfoModel;

FileLoader::FileLoader(QObject *parent) : QObject(parent)
  , m_thread(new QThread)
{
    qRegisterMetaType<QDir::Filters>("QDir::Filters");
    qRegisterMetaType<FMH::MODEL>("FMH::MODEL");
    qRegisterMetaType<FMH::MODEL_LIST>("FMH::MODEL_LIST");
    this->moveToThread(m_thread);
    connect(m_thread, &QThread::finished, m_thread, &QObject::deleteLater);
    connect(this, &FileLoader::start, this, &FileLoader::getFiles);
    m_thread->start();
}

FileLoader::~FileLoader()
{
    m_thread->quit();
    m_thread->wait();
}

void FileLoader::setBatchCount(const uint &count)
{
    m_batchCount = count;
}

uint FileLoader::batchCount() const
{
    return m_batchCount;
}

void FileLoader::requestPath(const QList<QUrl> &urls, const bool &recursive, const QStringList &nameFilters, const QDir::Filters &filters, const uint &limit)
{
    qDebug() << "FROM file loader" << urls;
    Q_EMIT this->start(urls, recursive, nameFilters, filters, limit);
}

void FileLoader::getFiles(QList<QUrl> paths, bool recursive, const QStringList &nameFilters, const QDir::Filters &filters, uint limit)
{
    qDebug() << "GETTING FILES";
    uint count = 0; // total count
    uint i = 0; // count per batch
    uint batch = 0; // batches count
    MODEL_LIST res;
    MODEL_LIST res_batch;

    if (!FileLoader::informer)
    {
        qWarning() << "FileLoader Informaer can not be nullptr";
        return;
    }

    for (const auto &path : paths)
    {
        if(FMStatic::getPathType(path) == FMStatic::PATHTYPE_KEY::TAGS_PATH)
        {
            for(const auto &url : Tagging::getInstance()->getTagUrls(path.toString().replace(QStringLiteral("tags:///"), QStringLiteral("")),
                                                                     nameFilters,
                                                                     true,
                                                                     limit))
            {               
                MODEL map = FileLoader::informer(url);
                
                if (map.isEmpty())
                    continue;
                
                Q_EMIT itemReady(map, paths);
                res << map;
                res_batch << map;
                i++;
                count++;
                
                if (i == m_batchCount) // send a batch
                {
                    Q_EMIT itemsReady(res_batch, paths);
                    res_batch.clear();
                    batch++;
                    i = 0;
                }
                
                if (count == limit)
                    break;
            }
            
            continue;
        }
        
        if (QFileInfo(path.toLocalFile()).isDir() && path.isLocalFile() && fileExists(path))
        {
            QDir dir(path.toLocalFile());
            dir.setNameFilters(nameFilters);
            dir.setFilter(filters);
            dir.setSorting(QDir::SortFlag::DirsFirst | QDir::SortFlag::Time);
            
            QDirIterator it(dir, recursive ? QDirIterator::Subdirectories : QDirIterator::NoIteratorFlags);

            while (it.hasNext())
            {
                const auto url = QUrl::fromLocalFile(it.next());
                MODEL map = FileLoader::informer(url);

                if (map.isEmpty())
                    continue;

                Q_EMIT itemReady(map, paths);
                res << map;
                res_batch << map;
                i++;
                count++;

                if (i == m_batchCount) // send a batch
                {
                    Q_EMIT itemsReady(res_batch, paths);
                    res_batch.clear();
                    batch++;
                    i = 0;
                }

                if (count == limit)
                    break;
            }
        }

        if (count == limit)
            break;
    }
    Q_EMIT itemsReady(res_batch, paths);
    Q_EMIT finished(res, paths);
}
