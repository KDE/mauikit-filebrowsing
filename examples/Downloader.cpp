#include <QCoreApplication>
#include <QDebug>
#include <QUrl>
#include <QObject>
#include <iostream>

#include <MauiKit4/FileBrowsing/downloader.h>
#include <MauiKit4/FileBrowsing/fmstatic.h>

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    QUrl url = u"https://mauikit.org/wp-content/uploads/2021/08/index-1.png"_qs; //The remote file image URL to download.
    QString fileName = "/IndexAppDemo.png"; //The new name of the image to save it locally.
    QUrl savePath =  FMStatic::DownloadsPath+fileName; //The local downloads path where to save the image.
    FMH::Downloader downloader;

    QObject::connect(&downloader, &FMH::Downloader::progress, [=](int percent)
                     {
                         qDebug() << "Downloading " << percent << "%";
                     });

    QObject::connect(&downloader, &FMH::Downloader::done, [&a]()
                     {
                         qDebug() << "Download finished";
                         a.exit();
                     });

    QObject::connect(&downloader, &FMH::Downloader::fileSaved, [=](QString path)
                     {
                         qDebug() << "Downloaded file has been saved as " << path;
                     });

    downloader.downloadFile(url, savePath);

    return a.exec();
}
