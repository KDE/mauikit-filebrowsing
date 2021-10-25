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

#ifndef TAGGING_H
#define TAGGING_H

#include <QObject>

#include "filebrowsing_export.h"
#include <MauiKit/Core/fmh.h>

#define MAX_LIMIT 9999

/**
 * @brief The Tagging class
 * Provides quick methods to access and modify the tags associated to files.
 * This class follows a singleton pattern and it is not thread safe, so only the main thread can have access to it.
 */

class TAGDB;
class FILEBROWSING_EXPORT Tagging : public QObject
{
    Q_OBJECT
public:
    /**
     * @brief getInstance
     * Returns an instance to the tagging object. This instance can only be accessed from the main thread, otherwise it will return a nullptr and segfault
     * @return
     */
    static Tagging *getInstance()
    {        
        if (m_instance)
            return m_instance;

        m_instance = new Tagging;
        return m_instance;
    }

    
public slots:
    
    /**
     * @brief get
     * Retrieve the information into a model, optionally you can pass a modifier callback function to manipulate or discard items in the model
     * @param query
     * Query to be retrieved
     * @param modifier
     * A callback function that sends as an argument a reference to the current item being retrieved, which can be modified, and expects a boolean value to be returned to decide if such item should de added to the model or not
     * @return
     * The model
     */
    const QVariantList get(const QString &query, std::function<bool(QVariantMap &item)> modifier = nullptr);
    
    /**
     * @brief tagExists
     * Checks if a given tag exists, it can be strictly enforced, meaning it is checked if the tag was created by the application making the request
     * @param tag
     * The tag to search
     * @param strict
     * If the search should be strictly enforced. If strict is true then the tag should have been created by the app making the request, otherwise checks if the tag exists and could have been created by any other application.
     * @return
     */
    bool tagExists(const QString &tag, const bool &strict = false);
    
    /**
     * @brief urlTagExists
     * Checks if a given tag is associated to a give file URL. The check can be strictly enforced, meaning the given URL was tagged by the application making the request
     * @param url
     * The file URL
     * @param tag
     * The tag to perfrom the check
     * @param strict
     * Strictly enforced check
     * @return
     */
    bool urlTagExists(const QString &url, const QString &tag);
    
    /* INSERTIIONS */
    /**
     * @brief tag
     * Adds a new tag, the newly created tag gets associated to the app making the call. If the tag already exists nothing is changed //TODO if the tag exists the app making the request should get associated to the tag too
     * @param tag
     * The name of the tag
     * @param color
     * Optional color for the tag
     * @param comment
     * Optional comment for the tag \deprecated
     * @return
     * Returns if the operation was sucessfull, meaning the tag was created
     */
    bool tag(const QString &tag, const QString &color = QString(), const QString &comment = QString());
    
    /**
     * @brief tagUrl
     * Adds a tag to a given file URL, if the given tag doesnt exists then it gets created
     * @param url
     * File URL to be tagged
     * @param tag
     * Tag to be added to the file URL
     * @param color \deprecated
     * Optional color
     * @param comment
     * Optional comment
     * @return
     */
    bool tagUrl(const QString &url, const QString &tag, const QString &color = QString(), const QString &comment = QString());
    
    /* UPDATES */
    /**
     * @brief updateUrlTags
     * Updates the tags associated to a file URL. If any of the given tags doesnt exists then they get created, if a tag associated to the current file URL is missing in the new passed tags then those get removed
     * @param url
     * File URL
     * @param tags
     * New set of tags to be associated to the file URL
     * @return
     */
    bool updateUrlTags(const QString &url, const QStringList &tags, const bool &strict = false);
    
    /**
     * @brief updateUrl
     * Updates a file URL to a new URL, preserving all associated tags. This is useful if a file is rename or moved to a new location
     * @param url
     * Previous file URL
     * @param newUrl
     * New file URL
     * @return
     */
    bool updateUrl(const QString &url, const QString &newUrl);
    
    /* QUERIES */
    
    /**
     * @brief getUrlsTags \deprecated
     * Give a list of all tags associated to files
     * @param strict
     * @return
     */
    QVariantList getUrlsTags(const bool &strict = false);
    
    /**
     * @brief getAllTags
     * Retruns a list model of all the tags. The model can be strictly enforced to only tags that were created by the application making the call
     * @param strict
     * If true returns only tags created by the application making the request
     * @return
     * Model with the info of all the requested tags
     */
    QVariantList getAllTags(const bool &strict = false);
    
    /**
     * @brief getUrls
     * Returns a model of all the file URLs associated to a tag, the result can be strictly enforced to only file URLs associated to a tag created by the application making the request, restrinct it to a maximum limit, filter by the
     * mimetype or just add a modifier function
     * @param tag
     * Tag name to perfrom the search
     * @param strict
     * Strictly enforced to only file URLs associated to the given tag created by the application making the request
     * @param limit
     * Maximum limit of results
     * @param mimeType
     * Filter by mimetype, for example: "image/\*" or "image/png"
     * @param modifier
     * A callback function that sends as an argument a reference to the current item being retrieved, which can be modified, and expects a boolean value to be returned to decide if such item should be added to the model or not
     * @return
     */
    QVariantList getUrls(const QString &tag, const bool &strict = false, const int &limit = MAX_LIMIT, const QString &mimeType = "", std::function<bool(QVariantMap &item)> modifier = nullptr);
    
    /**
     * @brief getUrlTags
     * Returns a model list of all the tags associated to a file URL. The result can be strictly enforced to only tags created by the application making the call
     * @param url
     * File URL
     * @param strict
     * Strictly enforced to only tags created by the application making the request
     * @return
     */
    QVariantList getUrlTags(const QString &url, const bool &strict = false);
    
    /* DELETES */
    /**
     * @brief removeUrlTags
     * Given a file URL remove all the tags associated to it
     * @param url
     * File URL
     * @return
     * If the operation was sucessfull
     */
    bool removeUrlTags(const QString &url, const bool &strict = false);
    
    /**
     * @brief removeUrlTag
     * Removes a given tag associated to a given file URL
     * @param url
     * File URL
     * @param tag
     * Tag associated to file URL to be removed
     * @return
     * If the operation was sucessfull
     */
    bool removeUrlTag(const QString &url, const QString &tag);
    
    /**
     * @brief removeUrl /todo
     * Removes a URL with its associated tags
     * @param url
     * File URL
     * @return
     * If the operation was sucessfull
     */
    bool removeUrl(const QString &url);
    
    bool removeTag(const QString &tag, const bool &strict = false);
    
    /**
     * @brief isFav
     * Checks if a file URL has been marked as favorite. This works if the tagging component has been enabled, otherwise returns fase as default.
     * @param url
     * The file URL to be checked
     * @param strict
     * Strictly check if the file has been marked as favorite by the app making the request or not
     * @return
     */
    bool isFav(const QUrl &url, const bool &strict = false);
    
    /**
     * @brief unFav
     * If the file has been marked as favorite then the tag is removed. This works if the tagging component has been enabled, otherwise returns fase as default.
     * @param url
     * The file URL
     * @return
     * If the operation has been sucessfull
     */
    bool unFav(const QUrl &url);
    
    /**
     * @brief fav
     * Marks a file URL as favorite.  This works if the tagging component has been enabled, otherwise returns fase as default.
     * @param url
     * File URL
     * @return
     * If the operation has been sucessfull
     */
    bool fav(const QUrl &url);
    
    /**
     * @brief toggleFav
     * Toogle the fav tag of a given file, meaning, if a file is marked as fav then the tag gets removed and if it is not marked then the fav tag gets added.
     * @param url
     * The file URL
     * @return
     * If the operation has been sucessfull
     */
    bool toggleFav(const QUrl &url);
    
    /**
     * @brief getTagUrls
     * Shortcut for gettings a list of file URLs associated to a tag, the resulting list of URLs can be filtered by regular expression or by mimetype and limited
     * @param tag
     * The tag to look up
     * @param filters
     * The regular expresions list
     * @param strict
     * If strict then the URLs returned are only associated to the application making the call, meaning, that such tag was added by such application only.
     * @param limit
     * The maximum limit number of URLs to be returned
     * @param mime
     * The mimetype filtering, for example, "image/\*" or "image/png", "audio/mp4"
     * @return
     */
    QList<QUrl> getTagUrls(const QString &tag, const QStringList &filters, const bool &strict = false, const int &limit = 9999, const QString &mime = "");
    
    /**
     * @brief getTags
     * Get all the tags avaliable with detailed information packaged as a FMH::MODEL_LIST
     * @param limit
     * Maximum numbers of tags
     * @return
     * Model of tags
     */
    FMH::MODEL_LIST getTags(const int &limit = 5); 
    
    /**
     * @brief getUrlTags
     * Return a model of tags associated to a file URL
     * @param url
     * The file URL
     * @return
     * Modle of the tags
     */
    FMH::MODEL_LIST getUrlTags(const QUrl &url);
        
    /**
     * @brief addTagToUrl
     * Adds a tag to a given file URL
     * @param tag
     * The wanted tag, if the tag doesnt exists it is created
     * @param url
     * The file URL
     * @return
     * If the operation has been sucessfull
     */
    bool addTagToUrl(const QString tag, const QUrl &url);
    
    /**
     * @brief removeTagToUrl
     * Removes a tag from a file URl if the tags exists
     * @param tag
     * the lookup tag
     * @param url
     * The file URL
     * @return
     * If the operation has been sucessfull
     */
    bool removeTagToUrl(const QString tag, const QUrl &url);    

private:
    static Tagging *m_instance;
    
    Tagging();
    ~Tagging();
    Tagging(const Tagging &) = delete;
    Tagging &operator=(const Tagging &) = delete;
    Tagging(Tagging &&) = delete;
    Tagging &operator=(Tagging &&) = delete;

    void setApp();

    QString appName;
    QString appComment;
    QString appOrg;

    //register the app to the db
    bool app();
    
    QHash<Qt::HANDLE, TAGDB *> m_dbs;
    TAGDB *db();

protected:
    static bool setTagIconName(QVariantMap &item);

signals:
    void urlTagged(QString url, QString tag);
    void tagged(QVariantMap tag);
    void tagRemoved(QString tag);
    void urlTagRemoved(QString tag, QString url);
    void urlRemoved(QString url);
};

#endif // TAGGING_H
