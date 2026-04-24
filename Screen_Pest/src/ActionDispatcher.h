#ifndef ACTIONDISPATCHER_H
#define ACTIONDISPATCHER_H

#include <functional>
#include <QString>
#include <QMap>

class SpriteWindow;

using ActionFunc = std::function<void(SpriteWindow&)>;

class ActionDispatcher {
public:
    // Construction with the window this dispatcher belongs to
    explicit ActionDispatcher(SpriteWindow *window);

    // Global defaults (static)
    static void registerDefault(const QString &name, ActionFunc func);
    static void executeDefault(const QString &name, SpriteWindow &w);

    // Per-instance overrides
    void setOverride(const QString &name, ActionFunc func);
    void setRemap(const QString &from, const QString &to);

    // Execute an action by name, using the stored window
    void execute(const QString &name);

private:
    SpriteWindow *m_window; // non-owning
    QMap<QString, ActionFunc> m_localActions;
    QMap<QString, QString> m_remaps;
};

#endif
