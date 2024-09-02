#ifndef APP_MODEL_H_
#define APP_MODEL_H_

#include <QList>
#include <QAbstractListModel>
#include "AppDef.h"

class AppModel : public QAbstractListModel {
    Q_OBJECT
   public:
    AppModel(QObject* parent = nullptr);

   public slots:
    void setFilter(const QString& filter);
    void appendApp(const QSharedPointer<AppMeta>& app);
    void removeApp(const QSharedPointer<AppMeta>& app);
    void flush();

   public:
    QSharedPointer<AppMeta> getApp(int row);

   protected:
    bool loadJSON();
    bool saveJSON();

   protected:
    int rowCount(const QModelIndex& parent) const override;
    int columnCount(const QModelIndex& parent) const override;
    QVariant data(const QModelIndex& index, int role) const override;

   protected:
    QString filter_;
    QList<QSharedPointer<AppMeta>> metas_;
    QList<QSharedPointer<AppMeta>> shownMetas_;
};
#endif  // !APP_MODEL_H_
