#include "MainWindow.h"

#include <QApplication>
#include <QLocale>
#include <QTranslator>
#include "Helper.h"

int main(int argc, char* argv[]) {
    QGuiApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QGuiApplication::setAttribute(Qt::AA_UseHighDpiPixmaps);
#if (QT_VERSION >= QT_VERSION_CHECK(5, 14, 0))
    QGuiApplication::setHighDpiScaleFactorRoundingPolicy(Qt::HighDpiScaleFactorRoundingPolicy::PassThrough);
#endif
    QApplication a(argc, argv);

    QTranslator translator;
    const QStringList uiLanguages = QLocale::system().uiLanguages();
    for (const QString& locale : uiLanguages) {
        const QString baseName = "PortableStarter_" + QLocale(locale).name();
        if (translator.load(":/build/" + baseName)) {
            a.installTranslator(&translator);
            break;
        }
    }

    a.setWindowIcon(QIcon(":/images/logo.png"));

    MainWindow w;
    QSettings& settings = GetSettings();
    QRect geo;
    if (settings.value("RememberWindowPosAndSize").toInt() == 1) {
        geo = settings.value("WindowGeometry").toRect();
    }

    if (geo.isNull()) {
        w.resize(600, 300);
    }
    else {
        w.setGeometry(geo);
    }
    w.show();

    return a.exec();
}
