// actiondispatcher.cpp
#include "ActionDispatcher.h"
#include "SpriteWindow.h"
#include <unordered_map>

// ---------- static defaults ----------
static std::unordered_map<QString, ActionFunc>& defaults() {
    static std::unordered_map<QString, ActionFunc> reg;
    return reg;
}

void ActionDispatcher::registerDefault(const QString &name, ActionFunc func) {
    defaults()[name] = std::move(func);
}

void ActionDispatcher::executeDefault(const QString &name, SpriteWindow &w) {
    auto it = defaults().find(name);
    if (it != defaults().end())
        it->second(w);
}

// ---------- instance ----------
ActionDispatcher::ActionDispatcher(SpriteWindow *window)
    : m_window(window)
{
    Q_ASSERT(m_window);
}

void ActionDispatcher::setOverride(const QString &name, ActionFunc func) {
    m_localActions[name] = std::move(func);
}

void ActionDispatcher::setRemap(const QString &from, const QString &to) {
    m_remaps[from] = to;
}

void ActionDispatcher::execute(const QString &name) {
    // 1. Local custom action
    auto localIt = m_localActions.find(name);
    if (localIt != m_localActions.end()) {
        localIt.value()(*m_window);
        return;
    }

    // 2. Remap to another action name
    auto remapIt = m_remaps.find(name);
    const QString &actualName = (remapIt != m_remaps.end()) ? remapIt.value() : name;

    // 3. Global default
    executeDefault(actualName, *m_window);
}
