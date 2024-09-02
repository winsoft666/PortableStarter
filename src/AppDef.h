#ifndef APP_DEF_H_
#define APP_DEF_H_

#include <QPixmap>
#include <QUuid>

struct AppMeta {
    QString id;
    bool runAsAdmin = false;
    bool cmdTool = false;
    QString path;
    QString parameter;
    QString name;
    QString triggerKey;
    QPixmap icon;

    AppMeta() {
        id = QUuid::createUuid().toString(QUuid::Id128);
    }
};
#endif  // !APP_DEF_H_
