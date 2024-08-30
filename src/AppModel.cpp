#include "AppModel.h"
#include <QFile>
#include <QBuffer>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonParseError>
#include <QCoreApplication>

AppModel::AppModel(QObject* parent /*= nullptr*/) :
    QAbstractListModel(parent) {
    loadJSON();
}

void AppModel::setFilter(const QString& filter) {
    filter_ = filter;
}

void AppModel::addApp(const AppMeta& app) {
    metas_.append(app);

    saveJSON();
}

AppMeta AppModel::getApp(int row) const {
    AppMeta app;
    if (row < metas_.size())
        app = metas_[row];
    return app;
}

bool AppModel::loadJSON() {
    metas_.clear();

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

        AppMeta meta;
        meta.runAsAdmin = obj["runAsAdmin"].toBool();
        meta.path = obj["path"].toString();
        meta.parameter = obj["parameter"].toString();
        meta.name = obj["name"].toString();
        meta.triggerKey = obj["triggerKey"].toString();
        meta.icon.loadFromData(QByteArray::fromBase64(obj["icon"].toString().toUtf8()), "PNG");

        metas_.append(meta);
    }

    return true;
}

bool AppModel::saveJSON() {
    QJsonArray root;
    for (int i = 0; i < metas_.size(); i++) {
        QJsonObject obj;
        obj["runAsAdmin"] = metas_[i].runAsAdmin;
        obj["path"] = metas_[i].path;
        obj["parameter"] = metas_[i].parameter;
        obj["name"] = metas_[i].name;
        obj["triggerKey"] = metas_[i].triggerKey;

        QByteArray iconByteArray;
        if (!metas_[i].icon.isNull()) {
            QBuffer iconBuffer(&iconByteArray);
            iconBuffer.open(QIODevice::WriteOnly);
            metas_[i].icon.save(&iconBuffer, "PNG");
        }

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
    return metas_.size();
}

int AppModel::columnCount(const QModelIndex& parent) const {
    return 1;
}

QVariant AppModel::data(const QModelIndex& index, int role) const {
    const int row = index.row();
    QVariant var;
    if (role == Qt::UserRole + 1) {  // icon
        if (row < metas_.size()) {
            var = QVariant::fromValue<QPixmap>(metas_[row].icon);
        }
    }
    else if (role == Qt::UserRole + 2) {  // text
        var = QVariant(QString("%1 %2 %3").arg(metas_[row].name).arg(metas_[row].path).arg(metas_[row].parameter));
    }
    return var;
}
