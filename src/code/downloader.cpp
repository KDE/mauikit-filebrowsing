#include "downloader.h"
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QFile>

// #if defined Q_OS_LINUX && !defined Q_OS_ANDROID
// #include <KIO/CopyJob>
// #include <KJob>
// #endif

FMH::Downloader::Downloader(QObject *parent)
: QObject(parent)
, manager(new QNetworkAccessManager(this))    
, reply(nullptr)
, file(nullptr)
, array(new QByteArray)
{        
}

void FMH::Downloader::setConnections()
{
    if(!reply)
    {
        return;
    }
    
    reply->disconnect();
    connect(reply, &QNetworkReply::downloadProgress, this, &Downloader::onDownloadProgress);    
    connect(reply, &QIODevice::readyRead, this, &Downloader::onReadyRead);     
    connect(reply, &QNetworkReply::finished, this, &Downloader::onReplyFinished);
    connect(reply, &QNetworkReply::errorOccurred, [this](QNetworkReply::NetworkError)
    {
        emit this->warning(reply->errorString());
    });
    
    //     connect(reply, &QNetworkReply::sslErrors, [this](const QList<QSslError> &errors)
    //     {
    //         emit this->warning(reply->sslErrors().);
    //     });
}

FMH::Downloader::~Downloader()
{
    qDebug() << "DELETEING DOWNLOADER";
    this->array->clear();
}

bool FMH::Downloader::isFinished() const
{
    if(!reply)
    {
        return false;
    }
    
   return reply->isFinished();
}

bool FMH::Downloader::isRunning() const
{
    if(!reply)
    {
        return false;
    }
    
   return reply->isRunning();
}


void FMH::Downloader::stop()
{
    if(!reply)
    {
        return;
    }
    
    if(this->reply->isRunning())
    {
        this->reply->abort();
        this->reply->close();
        emit this->aborted();        
        
        if(m_saveToFile)
        {
            if(this->file)
            {
                this->file->remove();
            }
        }else
        {
            this->array->clear();    
        }
    }    
}

void FMH::Downloader::downloadFile(const QUrl &source, const QUrl &destination)
{
    #ifdef KIO_COPYJOB_H
    KIO::CopyJob *downloadJob = KIO::copy(source, destination);
    
    connect(downloadJob, &KIO::CopyJob::warning, [=](KJob *job, QString message)
    {
        Q_UNUSED(job)
        emit this->warning(message);
    });
    
    connect(downloadJob, &KIO::CopyJob::processedSize, [=](KJob *job, qulonglong size)
    {
        emit this->progress(size, job->percent());
    });
    
    connect(downloadJob, &KIO::CopyJob::finished, [=](KJob *job) 
    {
        emit this->downloadReady();
        emit this->done();
    });
    
    #else
    if (destination.isEmpty() || source.isEmpty())
        return;
    
    QNetworkRequest request;
    request.setAttribute(QNetworkRequest::RedirectPolicyAttribute, true);
    request.setUrl(source);
    
    m_saveToFile = true;
    
    file = new QFile;
    file->setFileName(destination.toLocalFile());
    if (!file->open(QIODevice::WriteOnly))
    {
        emit this->warning("Can not open file to write download");
        return;
    }
    
    this->reply = manager->get(request);    
    this->setConnections();
    
    #endif
}

void FMH::Downloader::getArray(const QUrl &fileURL, const QMap<QString, QString> &headers)
{
    if (fileURL.isEmpty())
        return;
    
    QNetworkRequest request;
    request.setUrl(fileURL);
    if (!headers.isEmpty()) 
    {
        const auto keys = headers.keys();
        for (const auto &key : keys)
            request.setRawHeader(key.toLocal8Bit(), headers[key].toLocal8Bit());
    }
    
    m_saveToFile = false;
    reply = manager->get(request);
    this->setConnections();
}

void FMH::Downloader::onDownloadProgress(qint64 bytesRead, qint64 bytesTotal)
{
    if(bytesTotal <= 0)
    {
        return;
    }
    
    qDebug() << "DOWNLOAD PROGRESS" << ((bytesRead * 100) / bytesTotal);
    emit this->progress((bytesRead * 100) / bytesTotal);
}

void FMH::Downloader::onReadyRead()
{
    switch (reply->error()) 
    {
        case QNetworkReply::NoError: 
        {            
            if(m_saveToFile)
            {
                this->file->write(reply->readAll());
                
            }else
            {
                this->array->append(reply->readAll());
            }
            
            break;
        }
        
        default: 
        {
            qDebug() << reply->errorString();
            emit this->warning(reply->errorString());
        }
    }
}

void FMH::Downloader::onReplyFinished()
{   
   switch (reply->error()) 
   {
       case QNetworkReply::NoError:
       {           
           if(m_saveToFile)
           {        
               if (file->isOpen())
               {
                   file->close();            
               }   
               
               emit this->fileSaved(this->file->fileName());        
               file->deleteLater();
               
           }else
           {
               emit this->dataReady(*this->array);
           }
           
           emit this->done();           
           emit this->downloadReady();
           break;
       }
       
       default:
       {
           emit this->warning(reply->errorString());
       }
   }    
}
