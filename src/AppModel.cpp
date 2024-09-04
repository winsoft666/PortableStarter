#include "AppModel.h"
#include <QFile>
#include <QBuffer>
#include <QThread>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonParseError>
#include <QCoreApplication>
#include "Helper.h"

AppModel::AppModel(QObject* parent /*= nullptr*/) :
    QAbstractListModel(parent) {
    beginResetModel();
    if (loadJSON()) {
        for (auto it = metas_.begin(); it != metas_.end(); ++it) {
            shownMetas_.append(*it);
        }
    }
    endResetModel();
}

void AppModel::setFilter(const QString& search, const QString& category) {
    filterSearch_ = search.toLower();
    filterCategory_ = category;

    beginResetModel();
    shownMetas_.clear();
    for (auto it = metas_.begin(); it != metas_.end(); ++it) {
        if (filterCategory_.isEmpty() || (*it)->category == filterCategory_) {
            if (filterSearch_.isEmpty() || (*it)->triggerKey.indexOf(filterSearch_, 0, Qt::CaseInsensitive) >= 0) {
                shownMetas_.append(*it);
            }
        }
    }
    endResetModel();
}

void AppModel::appendApp(const QSharedPointer<AppMeta>& app) {
    beginInsertRows(QModelIndex(), metas_.size(), metas_.size() + 1);
    metas_.append(app);
    saveJSON();
    if (filterCategory_.isEmpty() || app->category == filterCategory_) {
        if (filterSearch_.isEmpty() || app->triggerKey.indexOf(filterSearch_, 0, Qt::CaseInsensitive) >= 0) {
            shownMetas_.append(app);
        }
    }
    endInsertRows();
}

void AppModel::removeApp(const QSharedPointer<AppMeta>& app) {
    int shownIdx = -1;
    for (int i = 0; i < shownMetas_.size(); i++) {
        if (shownMetas_.at(i)->id == app->id) {
            shownIdx = i;
            break;
        }
    }

    if (shownIdx >= 0) {
        beginRemoveRows(QModelIndex(), shownIdx, shownIdx);
        int metasIdx = -1;
        for (int i = 0; i < metas_.size(); i++) {
            if (metas_.at(i)->id == app->id) {
                metasIdx = i;
                break;
            }
        }

        shownMetas_.removeAt(shownIdx);
        metas_.removeAt(metasIdx);

        saveJSON();

        endRemoveRows();
    }
}

void AppModel::flush() {
    saveJSON();
}

QSharedPointer<AppMeta> AppModel::getApp(int row) {
    if (row < shownMetas_.size())
        return shownMetas_[row];
    return nullptr;
}

bool AppModel::loadJSON() {
    metas_.clear();
    shownMetas_.clear();

    QString jsonPath = QCoreApplication::applicationDirPath() + "/app.json";
    QFile file(jsonPath);
    if (!file.open(QIODevice::ReadOnly))
        return false;

    QByteArray data = file.readAll();
    file.close();

    QJsonParseError jsonError;
    QJsonDocument doc = QJsonDocument::fromJson(data, &jsonError);
    if (doc.isNull() || jsonError.error != QJsonParseError::NoError)
        return false;

    if (!doc.isArray())
        return false;

    QJsonArray appArray = doc.array();
    for (int i = 0; i < appArray.size(); i++) {
        QJsonObject obj = appArray[i].toObject();
        if (obj.isEmpty())
            continue;

        QSharedPointer<AppMeta> meta = QSharedPointer<AppMeta>::create();
        meta->id = obj["id"].toString();
        meta->runAsAdmin = obj["runAsAdmin"].toBool();
        meta->cmdTool = obj["cmdTool"].toBool();
        meta->category = obj["category"].toString();
        meta->path = obj["path"].toString();
        meta->parameter = obj["parameter"].toString();
        meta->name = obj["name"].toString();
        meta->triggerKey = obj["triggerKey"].toString();
        meta->icon = ByteArrayToPixmap(QByteArray::fromBase64(obj["icon"].toString().toUtf8()));

        metas_.append(meta);
    }

    return true;
}

bool AppModel::saveJSON() {
    QJsonArray root;
    for (int i = 0; i < metas_.size(); i++) {
        QJsonObject obj;
        obj["id"] = metas_[i]->id;
        obj["runAsAdmin"] = metas_[i]->runAsAdmin;
        obj["cmdTool"] = metas_[i]->cmdTool;
        obj["category"] = metas_[i]->category;
        obj["path"] = metas_[i]->path;
        obj["parameter"] = metas_[i]->parameter;
        obj["name"] = metas_[i]->name;
        obj["triggerKey"] = metas_[i]->triggerKey;

        QByteArray iconByteArray = PixmapToByteArray(metas_[i]->icon);
        Q_ASSERT(!iconByteArray.isEmpty());
        obj["icon"] = QString::fromUtf8(iconByteArray.toBase64());

        root.append(obj);
    }

    QJsonDocument doc(root);
    QString strJSON = doc.toJson();

    QString jsonPath = QCoreApplication::applicationDirPath() + "/app.json";
    QFile file(jsonPath);
    if (!file.open(QIODevice::WriteOnly)) {
        return false;
    }

    QByteArray u8 = strJSON.toUtf8();
    if (file.write(u8) != u8.size()) {
        file.close();
        return false;
    }

    file.close();
    return true;
}

int AppModel::rowCount(const QModelIndex& parent) const {
    return shownMetas_.size();
}

int AppModel::columnCount(const QModelIndex& parent) const {
    return 1;
}

QVariant AppModel::data(const QModelIndex& index, int role) const {
    const int row = index.row();
    QVariant var;
    if (role == Qt::UserRole + 1) {  // icon
        if (row < shownMetas_.size()) {
            var = QVariant::fromValue<QPixmap>(shownMetas_[row]->icon);
        }
    }
    else if (role == Qt::UserRole + 2) {  // name
        var = QVariant(shownMetas_[row]->name);
    }
    else if (role == Qt::UserRole + 3) {  // path + parameter
        var = QVariant(QString("%1 %2").arg(shownMetas_[row]->path).arg(shownMetas_[row]->parameter));
    }
    return var;
}
