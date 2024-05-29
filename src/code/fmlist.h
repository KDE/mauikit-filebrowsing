/*
 * <one line to give the program's name and a brief idea of what it does.>
 * Copyright (C) 2018  Camilo Higuita <email>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#pragma once

#include <QObject>
#include <QImage>
#include <QQmlEngine>

#include <MauiKit4/Core/mauilist.h>

#include "filebrowsing_export.h"

#include "fmstatic.h"

class FM;

/**
 * @brief Represents the status of a directory listing, be it non existence location, loading or empty.
 * 
 * The status object is divided into different properties for convenience, such as error label, message, icon, code, etc.
 */
struct FILEBROWSING_EXPORT PathStatus
{
    Q_GADGET
    
    /**
     * The status code. More details are exposed with the other properties.
     */
    Q_PROPERTY(PathStatus::STATUS_CODE code MEMBER m_code)
    
    /**
     * The status title.
     */
    Q_PROPERTY(QString title MEMBER m_title)
    
    /**
     * The message details of the status.
     */
    Q_PROPERTY(QString message MEMBER m_message)
    
    /**
     * An associated icon name for the status.
     */
    Q_PROPERTY(QString icon MEMBER m_icon)
    
    /**
     * Whether the location is empty and there is nothing for listing.
     */
    Q_PROPERTY(bool empty MEMBER m_empty)
    
    /**
     * Whether the location can be accessed and exists.
     */
    Q_PROPERTY(bool exists MEMBER m_exists)    
    
public:
    /**
     * @brief The different status that can occur.
     */
    enum STATUS_CODE : int 
    { 
        /**
         * The content is still loading its contents
         */
        LOADING, 
        
        /**
         * The listing of the contents has failed. For knowing the reason check the other properties, such as `title`, `exists`, etc.
         */
        ERROR,
        
        /**
         * The listing has finished successfully
         */
        READY 
        
    }; Q_ENUM(STATUS_CODE)
    
    STATUS_CODE m_code;
    QString m_title;
    QString m_message;
    QString m_icon;
    bool m_empty = false;
    bool m_exists = false;
};
Q_DECLARE_METATYPE(PathStatus)

/**
 * @private
 */
struct FILEBROWSING_EXPORT NavHistory {
    void appendPath(const QUrl &path)
    {
        this->prev_history.append(path);
    }
    
    QUrl getPosteriorPath()
    {
        if (this->post_history.isEmpty())
            return QUrl();
        
        return this->post_history.takeLast();
    }
    
    QUrl getPreviousPath()
    {
        if (this->prev_history.isEmpty())
            return QUrl();
        
        if (this->prev_history.length() < 2)
            return this->prev_history.at(0);
        
        this->post_history.append(this->prev_history.takeLast());
        return this->prev_history.takeLast();
    }
    
private:
    QVector<QUrl> prev_history;
    QVector<QUrl> post_history;
};

/**
 * @brief The FMList class
 * Model for listing the file system files and directories and perform relevant actions upon it
 */
class FILEBROWSING_EXPORT FMList : public MauiList
{
    Q_OBJECT
    QML_ELEMENT
    Q_DISABLE_COPY(FMList)
    
    // writable
    
    /**
     * Whether to auto load the content entries when the path property is modified. Otherwise explicitly call the load method.
     * @see search
     * @see fill
     */
    Q_PROPERTY(bool autoLoad READ getAutoLoad WRITE setAutoLoad NOTIFY autoLoadChanged)
    
    /**
     * The URL to location path to proceed listing all of its file entries.
     * There is support for multiple type of location depending on the scheme, for example local file system uses `file://`, while you can browser networks using `ftp://` or `fish://`. Support for those locations depends on KIO and its slaves - to know more about it read the KIO slaves documentation.
     */
    Q_PROPERTY(QString path READ getPath WRITE setPath NOTIFY pathChanged)
    
    /**
     * Whether to list the hidden entries.
     * By default this is set to `false`.
     */
    Q_PROPERTY(bool hidden READ getHidden WRITE setHidden NOTIFY hiddenChanged)
    
    /**
     * Whether only directories should be listed.
     * By default this is set to `false`.
     */
    Q_PROPERTY(bool onlyDirs READ getOnlyDirs WRITE setOnlyDirs NOTIFY onlyDirsChanged)
    
    /**
     * Whether the folders should be sorted first and then the files.
     * By default this is set to `true`.
     */
    Q_PROPERTY(bool foldersFirst READ getFoldersFirst WRITE setFoldersFirst NOTIFY foldersFirstChanged)
    
    /**
     * When the location if a remote cloud directory, this allows to define the depth of the levels for listing the contents.
     * By default this is set to `1`, which will only lists the entries in the current location, a bigger depth will start listing sub-directories too.
     * @deprecated
     */
    Q_PROPERTY(int cloudDepth READ getCloudDepth WRITE setCloudDepth NOTIFY cloudDepthChanged)
    
    /**
     * The list of string values to filter the listing. For example to only list PNG and JPG images: `filters: ["*.png", "*.jpg"]`.
     * To reset or clear the filters you can set the property to `undefined`
     */
    Q_PROPERTY(QStringList filters READ getFilters WRITE setFilters NOTIFY filtersChanged RESET resetFilters)
    
    /**
     * A convenient way to filter the location contents by a file type (mimetype). 
     * By default this is set to `FILTER_TYPE::NONE`.
     */
    Q_PROPERTY(FMList::FILTER filterType READ getFilterType WRITE setFilterType NOTIFY filterTypeChanged RESET resetFilterType)
    
    /**
     * The sorting value.
     * By default this is set to `SORTBY::MODIFIED`.
     */
    Q_PROPERTY(FMList::SORTBY sortBy READ getSortBy WRITE setSortBy NOTIFY sortByChanged)
    
    /**
     * Whether destructive actions or modifications can be done to the current location contents, such as deleting, renaming, pasting, adding, etc.
     * This only protects the location contents if using this API action methods.
     */
    Q_PROPERTY(bool readOnly READ readOnly WRITE setReadOnly NOTIFY readOnlyChanged)
    
    // readonly
    /**
     * The title name of the current location.
     */
    Q_PROPERTY(QString pathName READ getPathName NOTIFY pathNameChanged FINAL)
    
    /**
     * The known type of the current location.
     */
    Q_PROPERTY(FMList::PATHTYPE pathType READ getPathType NOTIFY pathTypeChanged FINAL)
    
    /**
     * The current status of the location contents listing. This is a group of properties.
     */
    Q_PROPERTY(PathStatus status READ getStatus NOTIFY statusChanged)
    
    /**
     * The location of the parent directory of this current location.
     */
    Q_PROPERTY(QUrl parentPath READ getParentPath NOTIFY pathChanged)
    
public:
    /**
     * @brief The possible values to sort the location contents
     */
    enum SORTBY : uint_fast8_t {
        /**
         * The size of the file entry
         */
        SIZE = FMH::MODEL_KEY::SIZE,
        
        /**
         * The last modified date of the entry file
         */
        MODIFIED = FMH::MODEL_KEY::MODIFIED,
        
        /**
         * The creation date of the file entry
         */
        DATE = FMH::MODEL_KEY::DATE,
        
        /**
         * The name or title of the file entry
         */
        LABEL = FMH::MODEL_KEY::LABEL,
        
        /**
         * The file type of the entry. Deduced from its file suffix name
         */
        MIME = FMH::MODEL_KEY::MIME,
        
        /**
         * The date when the file entry was added
         */
        ADDDATE = FMH::MODEL_KEY::ADDDATE
    };
    Q_ENUM(SORTBY)
    
    /**
     * @brief The possible values to filter the a location content by a mime-type.
     */
    enum FILTER : uint_fast8_t {
        /**
         * Audio file types. Such as MP3, WAV, FLAC, MP4, etc.
         */
        AUDIO = FMStatic::FILTER_TYPE::AUDIO,
        
        /**
         * Video file types
         */
        VIDEO = FMStatic::FILTER_TYPE::VIDEO,
        
        /**
         * Plain text file types
         */
        TEXT = FMStatic::FILTER_TYPE::TEXT,
        
        /**
         * Image file types
         */
        IMAGE = FMStatic::FILTER_TYPE::IMAGE,
        
        /**
         * PDF, EBooks and comic books file types
         */
        DOCUMENT = FMStatic::FILTER_TYPE::DOCUMENT,
        
        /**
         * Compressed archives
         */
        COMPRESSED = FMStatic::FILTER_TYPE::COMPRESSED,
        
        /**
         * Font file types
         */
        FONT = FMStatic::FILTER_TYPE::FONT,
        
        /**
         * Any file type
         */
        NONE = FMStatic::FILTER_TYPE::NONE
    };
    Q_ENUM(FILTER)
    
    /**
     * @brief The different location or places types.
     */
    enum PATHTYPE : uint_fast8_t {
        PLACES_PATH = FMStatic::PATHTYPE_KEY::PLACES_PATH,
        FISH_PATH = FMStatic::PATHTYPE_KEY::FISH_PATH,
        MTP_PATH = FMStatic::PATHTYPE_KEY::MTP_PATH,
        REMOTE_PATH = FMStatic::PATHTYPE_KEY::REMOTE_PATH,
        DRIVES_PATH = FMStatic::PATHTYPE_KEY::DRIVES_PATH,
        REMOVABLE_PATH = FMStatic::PATHTYPE_KEY::REMOVABLE_PATH,
        TAGS_PATH = FMStatic::PATHTYPE_KEY::TAGS_PATH,
        BOOKMARKS_PATH = FMStatic::PATHTYPE_KEY::BOOKMARKS_PATH,
        APPS_PATH = FMStatic::PATHTYPE_KEY::APPS_PATH,
        TRASH_PATH = FMStatic::PATHTYPE_KEY::TRASH_PATH,
        CLOUD_PATH = FMStatic::PATHTYPE_KEY::CLOUD_PATH,
        QUICK_PATH = FMStatic::PATHTYPE_KEY::QUICK_PATH,
        OTHER_PATH = FMStatic::PATHTYPE_KEY::OTHER_PATH
        
    };
    Q_ENUM(PATHTYPE)
    
    /**
     * @brief The possible view types for listing the entries in the FileBrowser visual control.
     */
    enum VIEW_TYPE : uint_fast8_t {
        /**
         * Display the file system entries in a grid view.
         */
        ICON_VIEW,
        
        /**
         * Display the file system entries in a list, with more information details visible.
         */
        LIST_VIEW
    };
    Q_ENUM(VIEW_TYPE)
    
    /**
     * @brief FMList
     * @param parent
     */
    FMList(QObject *parent = nullptr);
    
    /**
     * @brief items
     * @return
     */
    const FMH::MODEL_LIST &items() const final override;
    
    FMList::SORTBY getSortBy() const;
    void setSortBy(const FMList::SORTBY &key);
    
    /**
     * @private
     */
    void componentComplete() override final;
    
    bool getAutoLoad() const;    
    void setAutoLoad(bool value);    
    
    QString getPath() const;
    void setPath(const QString &path);
    
    QString getPathName() const;
    
    FMList::PATHTYPE getPathType() const;
    
    QStringList getFilters() const;
    void setFilters(const QStringList &filters);
    void resetFilters();    
    
    FMList::FILTER getFilterType() const;
    void setFilterType(const FMList::FILTER &type);    
    void resetFilterType();    
    
    bool getHidden() const;
    void setHidden(const bool &state);
    
    bool getOnlyDirs() const;
    void setOnlyDirs(const bool &state);
    
    const QUrl getParentPath();
    
    bool getFoldersFirst() const;
    void setFoldersFirst(const bool &value);
    
    int getCloudDepth() const;
    void setCloudDepth(const int &value);    
    
    PathStatus getStatus() const;
    
    void setReadOnly(bool value);
    bool readOnly() const;
    
private:
    FM *fm;
    void clear();
    void reset();
    void setList();
    void assignList(const FMH::MODEL_LIST &list);
    void appendToList(const FMH::MODEL_LIST &list);
    void sortList();
    void filterContent(const QString &query, const QUrl &path);
    void setStatus(const PathStatus &status);    
    
    bool saveImageFile(const QImage &image);
    bool saveTextFile(const QString &data, const QString &format);
    /**
     * @brief Gets a model of the files associated with a tag
     * @param tag The lookup tag
     * @param filters Filters as regular expression
     * @return Model of files associated
     */
    FMH::MODEL_LIST getTagContent(const QString &tag, const QStringList &filters = {});
    
    FMH::MODEL_LIST list = {{}};
    
    bool m_autoLoad = true;
    QUrl path;
    QString pathName = QString();
    QStringList filters = {};
    
    bool onlyDirs = false;
    bool hidden = false;
    
    bool foldersFirst = false;
    int cloudDepth = 1;
    
    PathStatus m_status;
    
    FMList::SORTBY sort = FMList::SORTBY::MODIFIED;
    FMList::FILTER filterType = FMList::FILTER::NONE;
    FMList::PATHTYPE pathType = FMList::PATHTYPE::PLACES_PATH;
    
    NavHistory m_navHistory;
    
    bool m_readOnly = false;
    
public Q_SLOTS:
    
    /**
     * @brief Refresh the model for new changes. h content listing ill be regenerated.
     */
    void refresh();
    
    /**
     * @brief Create a new directory within the current directory.
     * @param name the name of the directory
     */
    void createDir(const QString &name);
    
    /**
     * @brief Create a new file.
     * @note To create a custom file, please supply with the correct suffix.
     * @param name the name of the new file, for example `new.txt`
     */
    void createFile(const QString &name);
    
    /**
     * @brief Rename a file with from a given URL to the a new name provided
     * @param url the file URL to be renamed
     * @param newName the new name for the file
     */
    void renameFile(const QString &url, const QString &newName);
    
    /**
     * @brief Create a symbolic link to the given URL in the current location.
     * @param url the file URL to create the link from
     */
    void createSymlink(const QString &url);
    
    /**
     * @brief Completely remove the set of file URLs provided. This action can not be undone
     * @param urls the list of file URLS to be removed
     */
    void removeFiles(const QStringList &urls);
    
    /**
     * @brief Remove and move to the trash the provided set of file URLs
     * @param urls the list of file URLS to be removed
     */
    void moveToTrash(const QStringList &urls);
    
    /**
     * @brief Whether the clipboard has a supported type of content.
     * @return whether the clipboard content is a supported file URL or a text or image raw data.
     */
    bool clipboardHasContent() const;
    
    /**
     * @brief Copy a list of file URLs into the current directory
     * @param urls list of files
     */
    void copyInto(const QStringList &urls);
    
    /**
     * @brief Cut/move a list of file URLs to the current directory
     * @param urls list of files
     */
    void cutInto(const QStringList &urls);
    
    /**
     * @brief Handle the paste action.
     * This allows to quickly paste into the current location any file URL in the clipboard, and raw image data and text snippets into a new file.
     */
    void paste();
    
    /**
     * @brief Changes the icon of a directory by making use of the directory config file
     * @param index the index position of the directory in the model
     * @param iconName then name of the new icon
     */
    void setDirIcon(const int &index, const QString &iconName);
    
    /**
     * @brief Remove an item from the model, this does not remove the file from the file system
     * @param index the index position of the file entry
     */
    void remove(const int &index);
    
    /**
     * @brief Start a search - starting from the current location - for a given name query.
     * @param query the search query name
     * @param recursive whether the search should be recursive and look into the subsequent sub-directories structure. By default this is set to `false`.
     */
    void search(const QString &query, bool recursive = true);    
    
    /**
     * @brief The immediate previous path location that was navigated
     * @return the path URL location
     */
    const QUrl previousPath();
    
    /**
     * @brief The immediate posterior path location that was navigated
     * @return the path URL location
     */
    const QUrl posteriorPath();
    
    /**
     * @brief Given a file name query find if it exists in the current location 
     * @param index the index of the found file entry otherwise `-1`
     */
    int indexOfName(const QString &query);
    int indexOfFile(const QString &url);
    
Q_SIGNALS:
    void pathChanged();
    void pathNameChanged();
    void pathTypeChanged();
    void filtersChanged();
    void filterTypeChanged();
    void hiddenChanged();
    void onlyDirsChanged();
    void sortByChanged();
    void foldersFirstChanged();
    void statusChanged();
    void cloudDepthChanged();
    void autoLoadChanged();    
    void readOnlyChanged();
    
    /**
     * @brief Emitted when the listing process has any error message that needs to be notified.
     * @param message the warning message text
     */
    void warning(QString message);
    
    /**
     * @brief Emitted while the file listing is still in progress.
     * @param percent the loading progress - it goes from 0 to 100.
     */
    void progress(int percent);
};

