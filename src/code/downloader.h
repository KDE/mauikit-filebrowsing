#pragma once

#include <QObject>
#include <QString>
#include <QMap>

#include "filebrowsing_export.h"

class QNetworkAccessManager;
class QNetworkReply;
class QFile;

namespace FMH
{
    /**
     * @brief The Downloader class
     * This is a quick helper to download remote content and save it as local files. 
     * 
     * @note Allows to make get request using optional headers.
     * 
     * @code
     * #include <QCoreApplication> 
     * #include <QDebug>
     * #include <QUrl>
     * #include <QObject>
     * #include <iostream>
     * 
     * #include <MauiKit4/FileBrowsing/downloader.h>
     * #include <MauiKit4/FileBrowsing/fmstatic.h>
     * 
     * int main(int argc, char *argv[])
     * {
     *   QCoreApplication a(argc, argv);
     * 
     * QUrl url = u"https://mauikit.org/wp-content/uploads/2021/08/index-1.png"_qs; //The remote file image URL to download.
     * QString fileName = "/IndexAppDemo.png"; //The new name of the image to save it locally.
     * QUrl savePath =  FMStatic::DownloadsPath+fileName; //The local downloads path where to save the image.
     * FMH::Downloader downloader;
     * 
     * QObject::connect(&downloader, &FMH::Downloader::progress, [=](int percent) {
     * qDebug() << "Downloading " << percent << "%"; });
     * 
     * QObject::connect(&downloader, &FMH::Downloader::done, [&a]() {
     *      qDebug() << "Download finished";
     *      a.exit(); });
     * 
     * QObject::connect(&downloader, &FMH::Downloader::fileSaved, [=](QString path) {
     *      qDebug() << "Downloaded file has been saved as " << path; });
     * 
     * downloader.downloadFile(url, savePath);
     * 
     * return a.exec();
     * }
     * @endcode
     */
    class FILEBROWSING_EXPORT Downloader : public QObject
    {
        Q_OBJECT
        
    public:
        Downloader(QObject *parent = nullptr);
        
        virtual ~Downloader();
        
        /**
         * @brief Given a source URL to a file it downloads it to a given local destination
         * @param source the remote file URL
         * @param destination the local file URL destination with a name an suffix extension type
         */
        void downloadFile(const QUrl &source, const QUrl &destination);
        
        /**
         * @brief Given a URL make a get request and once the reply is ready emits a signal with the retrieved array data.
         * @note Usually this is used to retrieved structured information from a web API, such a a JSON or a XML file, to later be parsed.
         * @see dataReady
         * @param fileURL the end point URL to make the request
         * @param headers the optional set of HTTP request headers structured as a QMap of a key value pair of plain strings.
         */
        void getArray(const QUrl &fileURL, const QMap<QString, QString> &headers = {});
        
        /**
         * @brief Force to stop the process of the data request or the downloading of the file.
         */
        void stop();
        
        /**
         * @brief Whether the downloading or current request is still in progress.
         */
        bool isRunning() const;
        
        /**
         * @brief Whether the process has finished successfully.
         * @see aborted
         */
        bool isFinished() const;
        
    private:
        QNetworkAccessManager *manager;
        QNetworkReply *reply;
        QFile *file;
        
        QByteArray *array;
        
        bool m_saveToFile = false;
        void setConnections();
        
    Q_SIGNALS:
        
        /**
         * @brief Emitted while the process is ongoing 
         * @param percent the process value from 0 to 100
         */
        void progress(int percent);
        
        /**
         * @brief Emitted when the downloading has finished.
         */
        void downloadReady();
        
        /**
         * @brief Emitted when the download or data request has been aborted manually by calling the `stop` method.
         */
        void aborted();
        
        /**
         * @brief Emitted after the downloading has finished and the file has been saved successfully.
         * @param path the location path of the new saved file
         */
        void fileSaved(QString path);
        
        /**
         * @brief Emitted when there is a warning message, for example when an error has occurred during the process or at the end.
         */
        void warning(QString warning);
        
        /**
         * @brief Emitted when the data is ready and can be parsed.
         * @param array the array raw data
         */
        void dataReady(QByteArray array);
        
        /**
         * @brief Emitted when the process has been finished. Emitted as the `downloadReady` signal.
         */
        void done();
        
    private Q_SLOTS:
        /**
         * @private
         */
        void onDownloadProgress(qint64 bytesRead, qint64 bytesTotal);
        
        /**
         * @private
         */
        void onReadyRead();
        
        /**
         * @private
         */
        void onReplyFinished();
    };
}
