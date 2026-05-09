#pragma once

#include <functional>
#include <QString>
#include <QMap>
#include <QVariantMap>

class SpriteWindow;

using ActionFunc = std::function<void(SpriteWindow&, const QVariantMap &params)>;

class ActionDispatcher {
public:
    explicit ActionDispatcher(SpriteWindow *window);

    static void registerDefault(const QString &name, ActionFunc func);
    static void executeDefault(const QString &name, SpriteWindow &w,
                               const QVariantMap &params = {});

    void setOverride(const QString &name, ActionFunc func);
    void setRemap(const QString &from, const QString &to);

    void execute(const QString &name, const QVariantMap &params = {});

private:
    SpriteWindow *m_window;
    QMap<QString, ActionFunc> m_localActions;
    QMap<QString, QString> m_remaps;
};