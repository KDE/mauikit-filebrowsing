#pragma once
#include <QObject>
#include <QStringList>
#include <QQmlEngine>

#include <MauiKit4/Core/mauilist.h>

/**
 * @brief The TagsList class
 * A model of the system tags, ready to be consumed by QML. This model has basic support for browsing, associating, adding and removing tags.
 * 
 * This is a basic model for most actions supported by the MauiKit File Browsing Tagging system. For more details on supported actions and complete API documentation refer to the Tagging page.
 * @see Tagging
 * 
 * @note This class is exposed as a QML type with the alias name of `TagsListModel`.
 */
class TagsList : public MauiList
{
    Q_OBJECT
    QML_ELEMENT
    QML_NAMED_ELEMENT(TagsListModel)
    Q_DISABLE_COPY(TagsList)
    
    /**
     * Whether the retrieved tags should be only associated to the current application or to any other app.
     * By default this is set to `true`, so the lookup will be only matching tags associated to the given URLs that were created by the current application.
     */
    Q_PROPERTY(bool strict READ getStrict WRITE setStrict NOTIFY strictChanged)
    
    /**
     * The list of file URLs to look for their tags.
     */
    Q_PROPERTY(QStringList urls READ getUrls WRITE setUrls NOTIFY urlsChanged)
    
    /**
     * The resulting list of tag names that were found.
     */
    Q_PROPERTY(QStringList tags READ getTags NOTIFY tagsChanged)

public:
    explicit TagsList(QObject *parent = nullptr);

    const FMH::MODEL_LIST &items() const override;

    bool getStrict() const;
    void setStrict(const bool &value);

    QStringList getUrls() const;
    void setUrls(const QStringList &value);

    QStringList getTags() const;

    void componentComplete() override final;

private:
    FMH::MODEL_LIST list;
    void setList();

    bool strict = true;
    QStringList m_urls;

    void append(const FMH::MODEL &tag);

Q_SIGNALS:
    void strictChanged();
    void urlsChanged();
    void tagsChanged();

public Q_SLOTS:

    /**
     * @brief Adds a given tag to the model, if the tag already exists in the model then nothing happens.
     * @note This operation does not inserts the tag to the tagging data base. To insert a new tag see the insert function.
     * @see insert
     * @param tag the tag to be added to the model
     */
    void append(const QString &tag);
    
    /**
     * @brief Adds a given tag map to the model, if the tag map already exists in the model then nothing happens.
     * @note This operation does not inserts the tag to the tagging data base.
     * @param tag the tag map to be added to the model. The supported key values are: `tag`, `color`, `date`
     */
    void appendItem(const QVariantMap &tag);
    
    /**
     * @brief Adds a given list of tags to the model. Tags that already exists in the model are ignored.
     * @param tags list of tags to be added to the model.
     * @see append
     */
    void append(const QStringList &tags);

    /**
     * @brief Inserts a tag to the tagging data base.
     * @param tag to be inserted
     * @return if the tag already exists in the data base then it return false, if the operation is successful returns true otherwise false
     */
    bool insert(const QString &tag);

    /**
     * @brief Associates a given tag to the current file URLs set to the URLs property
     * @param tag a tag to be associated, if the tag doesn't exists then it gets created
     */
    void insertToUrls(const QString &tag);

    /**
     * @brief Updates a list of tags associated to the current file URLs. All the previous tags associated to each file URL are removed and replaced by the new ones
     * @param tags tags to be updated
     */
    void updateToUrls(const QStringList &tags);

    /**
     * @brief Removes a tag from the model at a given index. The tag is removed from the model but not from the tagging data base
     * @param index index position of the tag in the model. If the model has been filtered or ordered using the MauiKit BaseModel then it should use the mapped index.
     * @return
     */
    bool remove(const int &index);

    /**
     * @brief Removes a tag at the given index in the model from the given file URL. This removes the associated file URL from the tagging data base and  the tag from the model
     * @param index index of the tag in the model
     * @param url file URL
     */
    void removeFrom(const int &index, const QString &url);

    /**
     * @brief Removes a tag at a given index in the model from the all the file URLs currently set
     * @param index index of the tag in the model.
     */
    void removeFromUrls(const int &index);

    /**
     * @brief Removes a given tag name from the current list of file URLs set
     * @param tag the tag name
     */
    void removeFromUrls(const QString &tag);

    /**
     * @brief Removes a tag from the tagging data base. This operation will remove the association of the tag to the current application making the request, meaning that if the tag is also associated to another application then the tag will be conserved.
     * @param index
     */
    void erase(const int &index);

    /**
     * @brief Reloads the model, checking the tags from the given list of file URLs
     */
    void refresh();

    /**
     * @brief Checks whether a given tag name is already in the model list
     * @param tag tag name to look up
     * @return whether the tag exists 
     */
    bool contains(const QString &tag);
};

