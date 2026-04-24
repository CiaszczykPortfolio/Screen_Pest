// animationengine.cpp
#include "AnimationEngine.h"
#include "ActionDispatcher.h"

AnimationEngine::AnimationEngine(const BehaviourData &data, ActionDispatcher &dispatcher)
    : m_data(data), m_dispatcher(dispatcher) {}

void AnimationEngine::reset() {
    if (!m_data.states.isEmpty())
        resetState(m_data.initialState);
}

void AnimationEngine::resetState(const QString &name) {
    currentState = name;
    const StateDef &sd = m_data.states[name];
    m_stateTimer = sd.durationMin +
                   QRandomGenerator::global()->generateDouble() * (sd.durationMax - sd.durationMin);
    currentFrameIndex = 0;
    m_frameTimer = 0;
    const AnimationDef &ad = m_data.animations[sd.animation];
    m_currentFrameDuration = 1.0 / ad.fps +
                             QRandomGenerator::global()->generateDouble() * m_data.globalFrameJitter;
}

void AnimationEngine::update(double deltaTime)
{
    if (m_data.states.isEmpty())
        return;

    const StateDef &state = m_data.states[currentState];
    const AnimationDef &anim = m_data.animations[state.animation];

    m_stateTimer -= deltaTime;
    if (m_stateTimer <= 0.0) {
        if (!state.transitions.isEmpty()) {
            double totalWeight = 0.0;
            for (const auto &t : state.transitions)
                totalWeight += t.weight;

            double r = QRandomGenerator::global()->generateDouble() * totalWeight;
            QString nextState;
            for (const auto &t : state.transitions) {
                r -= t.weight;
                if (r <= 0.0) {
                    nextState = t.to;
                    break;
                }
            }
            if (nextState.isEmpty())
                nextState = state.transitions.first().to;

            resetState(nextState);

            update(0.0);
            return;
        } else {
            resetState(currentState);
        }
    }

    m_frameTimer += deltaTime;
    while (m_frameTimer >= m_currentFrameDuration) {
        m_frameTimer -= m_currentFrameDuration;

        currentFrameIndex++;
        if (currentFrameIndex >= anim.frameCount) {
            if (anim.loop)
                currentFrameIndex = 0;
            else
                currentFrameIndex = anim.frameCount - 1;
        }

        fireEvents(currentFrameIndex);

        double base = 1.0 / anim.fps;
        m_currentFrameDuration = base +
                                 QRandomGenerator::global()->generateDouble() * m_data.globalFrameJitter;
    }
}

void AnimationEngine::fireEvents(int frame) {
    const AnimationDef &ad = m_data.animations[m_data.states[currentState].animation];
    for (const auto &ev : ad.events) {
        if (ev.frame == frame)
            m_dispatcher.execute(ev.action);
    }
}
