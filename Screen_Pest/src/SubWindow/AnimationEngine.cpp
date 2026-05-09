#include "AnimationEngine.h"
#include "ActionDispatcher.h"
#include <QRandomGenerator>
#include <QVariantMap>

/*
 * konstruktor:
 * przechowuje referencje do dispatchera i danych zachowań
 */
AnimationEngine::AnimationEngine(const BehaviourData &data, ActionDispatcher &dispatcher)
    : m_data(data), m_dispatcher(dispatcher) {}

/*
 * rozpoczyna maszynę stanów od 1 zdefiniowanego zachowania
 * emituje onEnter.
 */
void AnimationEngine::reset() {
    if (!m_data.states.isEmpty())
        resetState(m_data.initialState);
}

/*
 * Wykonuje Exit action, rozpoczyna nowy stan, emituje
 * wywołuje akcję startową nowego stanu
 */
void AnimationEngine::resetState(const QString &name) {
    if (!currentState.isEmpty()) {
        const auto &oldState = m_data.states[currentState];
        if (!oldState.onExitAction.isEmpty())
            m_dispatcher.execute(oldState.onExitAction);
    }

    currentState = name;
    const StateDef &sd = m_data.states[name];
    m_stateTimer = sd.durationMin +
                   QRandomGenerator::global()->generateDouble() * (sd.durationMax - sd.durationMin);
    currentFrameIndex = 0;
    m_frameTimer = 0;

    const AnimationDef &ad = m_data.animations[sd.animation];
    m_currentFrameDuration = 1.0 / ad.fps +
                             QRandomGenerator::global()->generateDouble() * m_data.globalFrameJitter;

    // Akcja wejściowa nowego stanu
    if (!sd.onEnterAction.isEmpty())
        m_dispatcher.execute(sd.onEnterAction);

    emit frameAdvanced();
}

/*
 * Tick na zmianę klatki, jeśli istnieje,
 * wywołuje per-tick action
 * rozpoczyna nowy timer stanu
 */
void AnimationEngine::update(double deltaTime) {
    if (m_data.states.isEmpty()) return;

    const StateDef &state = m_data.states[currentState];
    const AnimationDef &anim = m_data.animations[state.animation];

    // akcja na tick
    if (!state.perTickAction.isEmpty()) {
        QVariantMap params;
        params["delta"] = deltaTime;
        m_dispatcher.execute(state.perTickAction, params);
    }

    // timer stanu i przejścia
    m_stateTimer -= deltaTime;
    if (m_stateTimer <= 0.0) {
        if (!state.transitions.isEmpty()) {
            double totalWeight = 0.0;
            for (const auto &t : state.transitions) totalWeight += t.weight;
            double r = QRandomGenerator::global()->generateDouble() * totalWeight;
            QString nextState;
            for (const auto &t : state.transitions) {
                r -= t.weight;
                if (r <= 0.0) { nextState = t.to; break; }
            }
            if (nextState.isEmpty()) nextState = state.transitions.first().to;
            resetState(nextState);
            // After transition, we need to emit the next timeout for the new state.
            // The frame timer for the new state is fresh; we can compute remaining time.
            double remaining = m_currentFrameDuration - m_frameTimer;
            if (remaining < 0.001) remaining = 0.001;
            emit nextFrameTimeout(static_cast<int>(remaining * 1000));
            return;
        } else {
            // Stay in same state, just reset timer
            // (re‑enter same state; trigger exit/enter actions? Usually no.)
            m_stateTimer = state.durationMin +
                           QRandomGenerator::global()->generateDouble() * (state.durationMax - state.durationMin);
            // No need to reset animation; state continues.
        }
    }

    // kolejna klatka
    m_frameTimer += deltaTime;
    while (m_frameTimer >= m_currentFrameDuration) {
        m_frameTimer -= m_currentFrameDuration;

        currentFrameIndex++;
        if (currentFrameIndex >= anim.frameCount) {
            if (anim.loop) currentFrameIndex = 0;
            else currentFrameIndex = anim.frameCount - 1;
        }

        fireEvents(currentFrameIndex);
        emit frameAdvanced();

        double base = 1.0 / anim.fps;
        m_currentFrameDuration = base +
                                 QRandomGenerator::global()->generateDouble() * m_data.globalFrameJitter;
    }

    // zaplanuj kolejny tick
    double remaining = m_currentFrameDuration - m_frameTimer;
    if (remaining < 0.001) remaining = 0.001;
    emit nextFrameTimeout(static_cast<int>(remaining * 1000));
}

/*
 * Wywołuje eventy dla konretnych klatek
 */
void AnimationEngine::fireEvents(int frame) {
    const AnimationDef &ad = m_data.animations[m_data.states[currentState].animation];
    for (const auto &ev : ad.events) {
        if (ev.frame == frame)
            m_dispatcher.execute(ev.action);
    }
}