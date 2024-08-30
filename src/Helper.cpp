#include "Helper.h"
#include <QDir>
#include <QCoreApplication>

namespace {
#ifdef Q_OS_MAC
QString macOSXAppBundlePath() {
    QDir dir = QDir(QCoreApplication::applicationDirPath());
    dir.cdUp();
    dir.cdUp();
    QString absolutePath = dir.absolutePath();
    // absolutePath will contain a "/" at the end,
    // but we want the clean path to the .app bundle
    if (absolutePath.length() > 0 && absolutePath.right(1) == "/") {
        absolutePath.chop(1);
    }
    return absolutePath;
}

QString macOSXAppBundleName() {
    QString bundlePath = macOSXAppBundlePath();
    QFileInfo fileInfo(bundlePath);
    return fileInfo.baseName();
}
#endif  // Q_OS_MAC
}  // namespace

QSettings& GetSettings() {
    QString iniPath = QCoreApplication::applicationDirPath() + "/config.ini";
    static QSettings settings(iniPath, QSettings::IniFormat);
    return settings;
}

void SetStartWithOS(bool startAutomatically) {
#if defined(Q_OS_MAC)

    // Remove any existing login entry for this app first, in case there was one
    // from a previous installation, that may be under a different launch path.
    {
        QStringList args;
        args << "-e tell application \"System Events\" to delete login item\"" + macOSXAppBundleName() + "\"";

        QProcess::execute("osascript", args);
    }

    // Now install the login item, if needed.
    if (startAutomatically) {
        QStringList args;
        args << "-e tell application \"System Events\" to make login item at end " +
                    "with properties {path:\"" + macOSXAppBundlePath() + "\", hidden:false}";

        QProcess::execute("osascript", args);
    }

#elif defined(Q_OS_WINDOWS)

    QString key = "PortableStarter";

    QSettings registrySettings(
        "HKEY_CURRENT_USER\\SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run",
        QSettings::NativeFormat);

    registrySettings.remove(key);

    if (startAutomatically) {
        registrySettings.setValue(key, QString("\"" + QDir::toNativeSeparators(QCoreApplication::applicationFilePath()) + "\""));
    }

    registrySettings.sync();

#endif
}

bool IsUrl(const QString& str) {
    return str.startsWith("http://", Qt::CaseInsensitive) || str.startsWith("https://", Qt::CaseInsensitive);
}
