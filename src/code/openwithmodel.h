#pragma once

#include <QObject>

#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
#include <MauiKit3/Core/mauilist.h>
#else
#include <MauiKit4/Core/mauilist.h>
#endif

class OpenWithModel : public MauiList
{
  Q_OBJECT
  Q_DISABLE_COPY(OpenWithModel)
  
  Q_PROPERTY(QStringList urls READ urls WRITE setUrls NOTIFY urlsChanged)
  
public:
    explicit OpenWithModel(QObject * parent = nullptr);
    
    const FMH::MODEL_LIST &items() const final override;
    void componentComplete() override final;
        
    void setUrls(const QStringList &urls);
    QStringList urls() const;
    
public Q_SLOTS:
    void openWith(const int &index);
    
private:
    void setList();
    
    FMH::MODEL_LIST m_list;
    QStringList m_urls;
    
Q_SIGNALS:
    void urlsChanged();
};

