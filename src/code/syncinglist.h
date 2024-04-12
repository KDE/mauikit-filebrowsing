#ifndef SYNCINGLIST_H
#define SYNCINGLIST_H

#include "fmh.h"
#include <QObject>

class FM;
class SyncingList : public QObject
{
    Q_OBJECT
    Q_DISABLE_COPY(SyncingList)
    
public:
    explicit SyncingList(QObject *parent = nullptr);
    FMH::MODEL_LIST items() const;

private:
    FMH::MODEL_LIST list;
    void setList();
    FM *fm;

Q_SIGNALS:
    void preItemAppended();
    void postItemAppended();
    void preItemRemoved(int index);
    void postItemRemoved();
    void updateModel(int index, QVector<int> roles);
    void preListChanged();
    void postListChanged();

public Q_SLOTS:
    QVariantMap get(const int &index) const;
    void insert(const QVariantMap &data);
    void removeAccount(const QString &server, const QString &user);
    void removeAccountAndFiles(const QString &server, const QString &user);
    void refresh();
};

#endif // SYNCINGLIST_H
