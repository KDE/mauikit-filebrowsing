#pragma once

#include <QObject>

#include <MauiKit4/Core/mauilist.h>

/**
 * @brief A model of services associated to a list of file URLs, that can handle its file content.
 *
 * This model is exposed adn used by the OpenWithDialog control.
 */
class OpenWithModel : public MauiList
{
    Q_OBJECT
    Q_DISABLE_COPY(OpenWithModel)

    /**
   * The list of file URLs. The model will try to look for all the possible services that can handle the given files.
   */
    Q_PROPERTY(QStringList urls READ urls WRITE setUrls NOTIFY urlsChanged)

public:
    explicit OpenWithModel(QObject * parent = nullptr);
    
    /**
     * @private
     */
    const FMH::MODEL_LIST &items() const final override;
    
    /**
     * @private
     */
    void componentComplete() override final;

    void setUrls(const QStringList &urls);
    QStringList urls() const;
    
public Q_SLOTS:
    /**
     * @brief Launch a service at the given index with the URLs provided.
     * @param index the index position of the service to launch
     */
    void openWith(const int &index);
    
private:
    void setList();
    
    FMH::MODEL_LIST m_list;
    QStringList m_urls;
    
Q_SIGNALS:
    void urlsChanged();
};

