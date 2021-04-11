#ifndef OPENWITHMODEL_H
#define OPENWITHMODEL_H

#include <QObject>
#include <MauiKit/Core/mauilist.h>

class OpenWithModel : public MauiList
{
  Q_OBJECT
  Q_PROPERTY(QStringList urls READ urls WRITE setUrls NOTIFY urlsChanged)
  
public:
    explicit OpenWithModel(QObject * parent = nullptr);
    
    const FMH::MODEL_LIST &items() const final override;
    void componentComplete() override final;
        
    void setUrls(const QStringList &urls);
    QStringList urls() const;
    
public slots:
    void openWith(const int &index);
    
private:
    void setList();
    
    FMH::MODEL_LIST m_list;
    QStringList m_urls;
    
signals:
    void urlsChanged();
};

#endif
