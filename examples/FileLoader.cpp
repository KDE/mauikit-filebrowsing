#include <QCoreApplication>
#include <QDebug>
#include <QUrl>
#include <QObject>
#include <iostream>

#include <MauiKit4/FileBrowsing/fileloader.h>
#include <MauiKit4/FileBrowsing/fmstatic.h>

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    FMH::FileLoader loader;
    QStringList urls = {FMStatic::DownloadsPath, FMStatic::PicturesPath};

    QObject::connect(&loader, &FMH::FileLoader::itemsReady, [=](FMH::MODEL_LIST items, QList<QUrl> urls)
                     {
                         for(const auto &item : items)
                             qDebug() << item[FMH::MODEL_KEY::NAME];

                         qDebug() << "items ready for:" << urls << items.length();
                     });

    QObject::connect(&loader, &FMH::FileLoader::finished, [=](FMH::MODEL_LIST items, QList<QUrl> urls)
                     {
                         qDebug() << "Finished process" << urls << items.length();
                     });

    loader.setBatchCount(10);
    loader.requestPath(QUrl::fromStringList(urls), true);

    return a.exec();
}
