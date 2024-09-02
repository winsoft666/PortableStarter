#ifndef APP_MODEL_H_
#define APP_MODEL_H_

#include <QList>
#include <QAbstractListModel>
#include "AppDef.h"

class AppModel : public QAbstractListModel {
    Q_OBJECT
   public:
    AppModel(QObject* parent = nullptr);

    void setFilter(const QString& filter);
    void appendApp(const AppMeta& app);
    void updateApp(const AppMeta& app);
    void removeApp(const AppMeta& app);
    AppMeta getApp(int row) const;

   protected:
    bool loadJSON();
    bool saveJSON();

   protected:
    int rowCount(const QModelIndex& parent) const override;
    int columnCount(const QModelIndex& parent) const override;
    QVariant data(const QModelIndex& index, int role) const override;

   protected:
    QString filter_;
    QList<AppMeta> metas_;
    QList<const AppMeta*> shownMetas_;
};
#endif // !APP_MODEL_H_
