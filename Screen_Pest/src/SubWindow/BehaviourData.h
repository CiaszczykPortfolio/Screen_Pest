// behaviourdata.h
#pragma once

#include <QString>
#include <QMap>
#include <QVector>
#include <QJsonObject>

struct Transition {
    QString to;
    double weight = 1.0;
};

struct ActionCall {
    QString name;
    QVariantMap params;
};

struct StateDef {
    QString animation;
    double durationMin = 2.0, durationMax = 5.0;
    QVector<Transition> transitions;

    // Optional actions triggered automatically
    ActionCall onEnterAction;
    ActionCall onExitAction;
    ActionCall perTickAction;      // called every update while in this state
    ActionCall onMousePress;        // Wydarzenie na triggerowane wciśnięciem myszki

    QVector<int> loops; //alternatywa dla duration
};

struct AnimationEvent {
    int frame;       // 0-indexed frame that triggers the action
    QString action;  // action name to dispatch
};

struct AnimationDef {
    QString prefix;
    int frameCount = 1;
    double fps = 4.0;
    bool loop = true;
    QVector<AnimationEvent> events;
};

class BehaviourData {
public:
    bool loadFromJson(const QString &jsonPath);

    QString name;
    double speed = 80.0;
    QString initialState;
    double globalFrameJitter = 0.0;
    int scaleFactor = 16;

    QMap<QString, AnimationDef> animations;
    QMap<QString, StateDef> states;
};
