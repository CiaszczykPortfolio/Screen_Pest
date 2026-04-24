// behaviourdata.h
#ifndef BEHAVIOURDATA_H
#define BEHAVIOURDATA_H

#include <QString>
#include <QMap>
#include <QVector>
#include <QJsonObject>

struct Transition {
    QString to;
    double weight = 1.0;
};

struct StateDef {
    QString animation;
    double durationMin = 2.0, durationMax = 5.0;
    QVector<Transition> transitions;
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
    double globalFrameJitter = 0.08;
    int scaleFactor = 16;

    QMap<QString, AnimationDef> animations;
    QMap<QString, StateDef> states;
};

#endif
