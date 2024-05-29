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

#pragma once

#include <QObject>
#include <QQmlEngine>

#include "filebrowsing_export.h"

#include <MauiKit4/Core/fmh.h>

#define MAX_LIMIT 9999

class TAGDB;

/**
 * @brief The Tagging class provides quick methods to access and modify the tags associated to the files.
 *
 * @note This class follows a singleton pattern and it is thread safe, by creating a new instance for each new thread that request access to the singleton. All of the internal instances are self-handled and destroyed when the application quits.
 *
 * Graphical interfaces are provided which implement most of this class functionality and can be quickly used:
 * - TagsBar
 * - TagsDialog
 */
class FILEBROWSING_EXPORT Tagging : public QObject
{
    Q_OBJECT
    QML_ELEMENT
    QML_SINGLETON
    Q_DISABLE_COPY_MOVE(Tagging)

public:
    /**
     * @brief Returns an instance to the tagging object.
     * @return
     */
    static Tagging *getInstance();
    static QObject * qmlInstance(QQmlEngine *engine, QJSEngine *scriptEngine) {
        Q_UNUSED(scriptEngine)
        auto obj = Tagging::getInstance();
        engine->setObjectOwnership(obj, QQmlEngine::CppOwnership);
        return obj;
    }

    /**
     * @private
     */
    Tagging();

    /**
     * @private
     */
    ~Tagging();

public Q_SLOTS:

    /**
     * @brief Retrieve the information into a model, optionally you can pass a modifier callback function to manipulate or discard items in the model
     * @param query the query to be retrieved
     * @param modifier a callback function that sends as an argument a reference to the current item being retrieved, which can be modified in-place, and expects a boolean value to be returned to decide if such item should de added to the model or not
     * @return the resulting model
     */
    const QVariantList get(const QString &query, std::function<bool(QVariantMap &item)> modifier = nullptr);

    /**
     * @brief Checks if a given tag exists, it can be strictly enforced, meaning it is checked if the tag was created by the application making the request
     * @param tag the tag to search
     * @param strict whether the search should be strictly enforced. If strict is true then the tag should have been created by the app making the request, otherwise checks if the tag exists and could have been created by any other application.
     * @return
     */
    bool tagExists(const QString &tag, const bool &strict = false);

    /**
     * @brief Checks if a given tag is associated to a give file URL. The check can be strictly enforced, meaning the given URL was tagged by the application making the request
     * @param url the file URL
     * @param tag the tag to perform the check
     * @param strict strictly enforced check
     * @return
     */
    bool urlTagExists(const QString &url, const QString &tag);

    // INSERTIONS
    /**
     * @brief Adds a new tag, the newly created tag gets associated to the app making the call. If the tag already exists nothing is changed. If the tag exists the app making the request will get associated to the tag too
     * @param tag the name of the tag
     * @param color optional color for the tag
     * @param comment optional comment for the tag @deprecated
     * @return whether the operation was successful, meaning the tag was created
     */
    bool tag(const QString &tag, const QString &color = QString(), const QString &comment = QString());

    /**
     * @brief Adds a tag to a given file URL, if the given tag doesn't exists then it gets created
     * @param url the file URL to be tagged
     * @param tag the tag to be added to the file URL
     * @param color @deprecated Optional color
     * @param comment optional comment
     * @return whether the operation was successful
     */
    bool tagUrl(const QString &url, const QString &tag, const QString &color = QString(), const QString &comment = QString());

    // UPDATES
    /**
     * @brief Updates the tags associated to a file URL. If any of the given tags doesn't exists then they get created, if a tag associated to the current file URL is missing in the new passed tags then those get removed
     * @param url the file URL
     * @param tags the new set of tags to be associated to the file URL
     * @return whether the operation was successful
     */
    bool updateUrlTags(const QString &url, const QStringList &tags, const bool &strict = false);

    /**
     * @brief Updates a file URL to a new URL, preserving all associated tags. This is useful if a file is rename or moved to a new location
     * @param url previous file URL
     * @param newUrl new file URL
     * @return whether the operation was successful
     */
    bool updateUrl(const QString &url, const QString &newUrl);

    // QUERIES

    /**
     * @brief Give a list of all tags associated to files @deprecated
     * @param strict
     * @return whether the operation was successful
     */
    QVariantList getUrlsTags(const bool &strict = false);

    /**
     * @brief Returns a list model of all the tags. The model can be strictly enforced to only tags that were created by the application making the call
     * @param strict if true returns only tags created by the application making the request
     * @return the model with the info of all the requested tags
     */
    QVariantList getAllTags(const bool &strict = false);

    /**
     * @brief Returns a model of all the file URLs associated to a tag, the result can be strictly enforced to only file URLs associated to a tag created by the application making the request, restrict it to a maximum limit, filter by the mime-type or just add a modifier function
     * @param tag the tag name to perform the search
     * @param strict strictly enforced to only file URLs associated to the given tag created by the application making the request
     * @param limit maximum limit of results
     * @param mimeType filter by mime-type, for example: `"image/\*"` or `"image/png"`
     * @param modifier a callback function that sends as an argument a reference to the current item being retrieved, which can be modified, and expects a boolean value to be returned to decide if such item should be added to the model or not
     * @return the result model
     */
    QVariantList getUrls(const QString &tag, const bool &strict = false, const int &limit = MAX_LIMIT, const QString &mimeType = QStringLiteral(""), std::function<bool(QVariantMap &item)> modifier = nullptr);

    /**
     * @brief Returns a model list of all the tags associated to a file URL. The result can be strictly enforced to only tags created by the application making the call
     * @param url the file URL
     * @param strict strictly enforced to only tags created by the application making the request
     * @return the result model
     */
    QVariantList getUrlTags(const QString &url, const bool &strict = false);

    // DELETE
    /**
     * @brief Given a file URL remove all the tags associated to it
     * @param url the file URL
     * @return whether the operation was successful
     */
    bool removeUrlTags(const QString &url, const bool &strict = false);

    /**
     * @brief Removes a given tag associated to a given file URL
     * @param url file URL
     * @param tag tag associated to file URL to be removed
     * @return whether  the operation was successful
     */
    bool removeUrlTag(const QString &url, const QString &tag);

    /**
     * @brief Removes a URL with its associated tags
     * @param url the file URL
     * @return whether the operation was successful
     */
    bool removeUrl(const QString &url);

    /**
     * @brief Remove a tag
     */
    bool removeTag(const QString &tag, const bool &strict = false);

    /**
     * @brief Checks if a file URL has been marked as favorite. This works if the tagging component has been enabled, otherwise returns false as default.
     * @param url the file URL to be checked
     * @param strict strictly check if the file has been marked as favorite by the app making the request or not
     * @return
     */
    bool isFav(const QUrl &url, const bool &strict = false);

    /**
     * @brief If the given file has been marked as favorite then the tag is removed. This works if the tagging component has been enabled, otherwise returns false as default.
     * @param url the file URL
     * @return whether the operation has been successful
     */
    bool unFav(const QUrl &url);

    /**
     * @brief Marks a file URL as favorite.  This works if the tagging component has been enabled, otherwise returns false as default.
     * @param url the file URL
     * @return whether the operation has been successful
     */
    bool fav(const QUrl &url);

    /**
     * @brief Toggle the fav tag of a given file, meaning, if a file is marked as fav then the tag gets removed and if it is not marked then the fav tag gets added.
     * @param url the file URL
     * @return whether  the operation has been successful
     */
    bool toggleFav(const QUrl &url);

    /**
     * @brief Shortcut for getting a list of file URLs associated to a tag, the resulting list of URLs can be filtered by regular expression or by mime-type and limited
     * @param tag the tag to look up
     * @param filters the regular expressions list
     * @param strict if strict then the URLs returned are only associated to the application making the call, meaning, that such tag was added by such application only.
     * @param limit the maximum limit number of URLs to be returned
     * @param mime the mime-type filtering, for example, `"image/\*"` or `"image/png"`, `"audio/mp4"`
     * @return the list of file URLs
     */
    QList<QUrl> getTagUrls(const QString &tag, const QStringList &filters, const bool &strict = false, const int &limit = 9999, const QString &mime = QStringLiteral(""));

    /**
     * @brief Get all the tags available with detailed information packaged as a FMH::MODEL_LIST
     * @param limit the maximum numbers of tags
     * @return the model of tags
     */
    FMH::MODEL_LIST getTags(const int &limit = 5);

    /**
     * @brief Returns a model of tags associated to a file URL
     * @param url the file URL
     * @return model of the tags
     */
    FMH::MODEL_LIST getUrlTags(const QUrl &url);

    /**
     * @brief Adds a tag to a given file URL
     * @param tag the wanted tag, if the tag doesn't exists it is created
     * @param url the file URL
     * @return whether the operation has been successful
     */
    bool addTagToUrl(const QString tag, const QUrl &url);

    /**
     * @brief Removes a tag from a file URL if the tags exists
     * @param tag the lookup tag
     * @param url the file URL
     * @return whether the operation has been successful
     */
    bool removeTagToUrl(const QString tag, const QUrl &url);

private:
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

Q_SIGNALS:
    void urlTagged(QString url, QString tag);
    void tagged(QVariantMap tag);
    void tagRemoved(QString tag);
    void urlTagRemoved(QString tag, QString url);
    void urlRemoved(QString url);
};

