#include <QApplication>
#include <QStandardPaths>
#include <QDir>
#include <QDebug>
#include "ManagerWindow/MainWindow.h"
#include "SubWindow/SpriteWindow.h"
#include "SubWindow/ActionDispatcher.h"

/*
 * rejestruje bazowe akcje w dispatcherze
 */
void registerDefaultActions()
{
    using namespace std::placeholders;

    ActionDispatcher::registerDefault("move",
                                      [](SpriteWindow &w, const QVariantMap &params) {
                                          double dx = params.value("dx", 0.0).toDouble();
                                          double delta = params.value("delta", 0.0).toDouble();
                                          dx *= delta*100;
                                          double dy = params.value("dy", 0.0).toDouble();
                                          dy *= delta*100;
                                          w.moveBy(dx, -dy);
                                      });

    ActionDispatcher::registerDefault("grab_cursor",
                                      [](SpriteWindow &w, const QVariantMap &) {
                                          QApplication::setOverrideCursor(Qt::ClosedHandCursor);
                                          // Optionally store state to release later
                                      });

    ActionDispatcher::registerDefault("release_cursor",
                                      [](SpriteWindow &w, const QVariantMap &) {
                                          QApplication::restoreOverrideCursor();
                                      });

    ActionDispatcher::registerDefault("sit_down",
                                      [](SpriteWindow &w, const QVariantMap &) {
                                      });
    ActionDispatcher::registerDefault("stand_up",
                                      [](SpriteWindow &w, const QVariantMap &) {
                                      });
    ActionDispatcher::registerDefault("start_cleaning",
                                      [](SpriteWindow &w, const QVariantMap &) {});
    ActionDispatcher::registerDefault("finish_cleaning",
                                      [](SpriteWindow &w, const QVariantMap &) {});
    ActionDispatcher::registerDefault("jump_sound",
                                      [](SpriteWindow &w, const QVariantMap &) {});

    ActionDispatcher::registerDefault("play_animation",
                                      [](SpriteWindow &w, const QVariantMap &params) {
                                          QString anim = params.value("animation").toString();
                                          if (!anim.isEmpty())
                                              w.switchState(anim);
                                      });
}

/* findObjectsDir: Znajduje gdzie jest
 * folder assets w zależności
 * od metody uruchomienia programu i go
 * zwraca.
 */
static QString findAssetsDir(const QString &appName)
{
    // 1. lokalizacja instalacji
    const QStringList installDirs =
        QStandardPaths::standardLocations(QStandardPaths::GenericDataLocation);
    for (const QString &base : installDirs) {
        QDir candidate(base + QLatin1Char('/') + appName + QLatin1String("/assets"));
        if (candidate.exists())
            return candidate.absolutePath();
    }

    // 2. lokalizacja dev na podstawie macro
    #ifdef RESOURCE_DIR
        QDir devDir(QStringLiteral(RESOURCE_DIR) + QLatin1String("/assets"));
        if (devDir.exists())
            return devDir.absolutePath();
    #endif

    // 3. Warning - nie ma folderu z assetami
    qWarning() << "Could not find assets directory!";
    return {};
}


/*
 *  main: Tworzy okno głównego manadżera,
 * I przekazuje lokalizacje folderu assets
 * tylko jeśli istnieje.
 */
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    //fprintf(stderr, "=== stderr test ===\n");

    a.setApplicationName("ScreenPest");

    registerDefaultActions();

    const QString rootDir = findAssetsDir(a.applicationName());
    if (rootDir.isEmpty()) {
        qCritical("No objects directory found – aborting.");
        return 1;
    }

    MainWindow window(rootDir);
    window.show();
    return a.exec();
}

