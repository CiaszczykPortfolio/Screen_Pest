#ifndef ANIMATIONENGINE_H
#define ANIMATIONENGINE_H

#include "BehaviourData.h"   // needs struct definitions
#include <QString>
#include <QRandomGenerator>

class ActionDispatcher;  // forward

class AnimationEngine {
public:
    AnimationEngine(const BehaviourData &data, ActionDispatcher &dispatcher);

    void reset();
    void update(double deltaTime);

    QString currentState;
    int currentFrameIndex = 0;   // 0-based

private:
    const BehaviourData &m_data;
    ActionDispatcher &m_dispatcher;

    double m_stateTimer = 0;
    double m_frameTimer = 0;
    double m_currentFrameDuration = 0;

    void resetState(const QString &name);
    void advanceFrame();
    void fireEvents(int frame);
};

#endif
