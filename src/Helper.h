#ifndef HELPER_H_
#define HELPER_H_

#include <QSettings>
#include <QList>
#include <QByteArray>
#include <QPixmap>

QSettings& GetSettings();

void SetStartWithOS(bool startAutomatically);

bool IsUrl(const QString& str);

bool GetStringLetters(const QString& string, QList<QString>& allLetters);

QByteArray PixmapToByteArray(const QPixmap& pix);

QPixmap ByteArrayToPixmap(const QByteArray& arr);

#endif  // !HELPER_H_