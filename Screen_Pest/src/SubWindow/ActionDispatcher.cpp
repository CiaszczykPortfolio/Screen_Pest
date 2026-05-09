#include "ActionDispatcher.h"
#include "SpriteWindow.h"
#include <unordered_map>

/*
 * Przechowuje globalne implementacje akcji
 */
static std::unordered_map<QString, ActionFunc>& defaults() {
    static std::unordered_map<QString, ActionFunc> reg;
    return reg;
}

/*
 * rejestruje globalne implementacje
 */
void ActionDispatcher::registerDefault(const QString &name, ActionFunc func) {
    defaults()[name] = std::move(func);
}

/*
 * Sprawdza nazwę akcji w globalnym rejestrze,
 * wywołuje jeśli istnieje
 */
void ActionDispatcher::executeDefault(const QString &name, SpriteWindow &w,
                                      const QVariantMap &params) {
    auto it = defaults().find(name);
    if (it != defaults().end())
        it->second(w, params);
}

/*
 * konstruktor instancjonowanego dispatchera,
 * przechowuje wskaźnik do okna, którego akcje wywołuje
 */
ActionDispatcher::ActionDispatcher(SpriteWindow *window)
    : m_window(window)
{
    Q_ASSERT(m_window);
}

/*
 * Przesłania globalną implementację lokalną
 */
void ActionDispatcher::setOverride(const QString &name, ActionFunc func) {
    m_localActions[name] = std::move(func);
}

/*
 * Przekazuje nazwę jednej akcji innej
 */
void ActionDispatcher::setRemap(const QString &from, const QString &to) {
    m_remaps[from] = to;
}

/*
 * Gdy jakakolwiek akcja jest wywołana,
 * jest egzekwowana przez tą metodę
 */
void ActionDispatcher::execute(const QString &name, const QVariantMap &params) {
    // 1. Local custom action
    auto localIt = m_localActions.find(name);
    if (localIt != m_localActions.end()) {
        localIt.value()(*m_window, params);
        return;
    }

    // 2. Remap to another action name
    auto remapIt = m_remaps.find(name);
    const QString &actualName = (remapIt != m_remaps.end()) ? remapIt.value() : name;

    // 3. Global default
    executeDefault(actualName, *m_window, params);
}