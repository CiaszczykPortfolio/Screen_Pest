#pragma once

#include "BehaviourData.h"
#include <QString>
#include <QRandomGenerator>

class ActionDispatcher;

class AnimationEngine : public QObject
{
    Q_OBJECT
public:
    AnimationEngine(const BehaviourData &data, ActionDispatcher &dispatcher);
    void reset();
    void update(double deltaTime);

    void resetState(const QString &name);


    QString currentState;
    int currentFrameIndex = 0;

signals:
    void frameAdvanced();
    void nextFrameTimeout(int msec);

private:
    const BehaviourData &m_data;
    ActionDispatcher &m_dispatcher;
    double m_stateTimer = 0;
    double m_frameTimer = 0;
    double m_currentFrameDuration = 0;

    void fireEvents(int frame);
    void dispatchAction(const QString &action, const QVariantMap &params = {});
};
