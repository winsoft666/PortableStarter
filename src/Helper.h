#ifndef HELPER_H_
#define HELPER_H_

#include <QSettings>
#include <QList>

QSettings& GetSettings();

void SetStartWithOS(bool startAutomatically);

bool IsUrl(const QString& str);

bool GetStringLetters(const QString& string, QList<QString>& allLetters);

#endif  // !HELPER_H_