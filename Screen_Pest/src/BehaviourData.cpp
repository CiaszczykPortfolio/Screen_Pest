#include "BehaviourData.h"
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

template<typename T>
using JsonFieldSetter = std::function<void(T&, const QJsonValue&)>;

template<typename T>
struct FieldMap {
    QHash<QString, JsonFieldSetter<T>> fields;

    void load(T& obj, const QJsonObject &src) const {
        for (auto it = src.begin(); it != src.end(); ++it) {
            auto setter = fields.find(it.key());
            if (setter != fields.end())
                (*setter)(obj, it.value());
        }
    }
};

static const FieldMap<AnimationEvent> eventMap {
    {{"frame",  [](auto& ev, auto v){ ev.frame = v.toInt(); }},
     {"action", [](auto& ev, auto v){ ev.action = v.toString(); }}}
};

static const FieldMap<AnimationDef> animMap {
    {{"prefix", [](auto& def, auto v){ def.prefix = v.toString(); }},
     {"frames", [](auto& def, auto v){ def.frameCount = v.toInt(); }},
     {"fps",    [](auto& def, auto v){ def.fps = v.toDouble(); }},
     {"loop",   [](auto& def, auto v){ def.loop = v.toBool(); }}}
};

static const FieldMap<Transition> transMap {
    {{"to",     [](auto& tr, auto v){ tr.to = v.toString(); }},
     {"weight", [](auto& tr, auto v){ tr.weight = v.toDouble(); }}}
};

static const FieldMap<StateDef> stateMap {
    {{"animation", [](auto& sd, auto v){ sd.animation = v.toString(); }}}
};

bool BehaviourData::loadFromJson(const QString &jsonPath)
{
    QFile file(jsonPath);
    if (!file.open(QIODevice::ReadOnly))
        return false;

    QJsonParseError err;
    QJsonDocument doc = QJsonDocument::fromJson(file.readAll(), &err);
    if (err.error != QJsonParseError::NoError)
        return false;

    QJsonObject root = doc.object();

    name            = root.value("name").toString();
    speed           = root.value("speed").toDouble(80);
    initialState    = root.value("initialState").toString("idle");
    globalFrameJitter = root.value("globalFrameJitter").toDouble(0.08);
    scaleFactor     = root.value("scaleFactor").toInt(16);

    // Load animations
    animations.clear();
    QJsonObject animObj = root.value("animations").toObject();
    for (auto it = animObj.begin(); it != animObj.end(); ++it) {
        AnimationDef def;
        QJsonObject obj = it.value().toObject();
        animMap.load(def, obj);

        // Parse events array manually
        QJsonArray evArr = obj.value("events").toArray();
        for (auto evVal : evArr) {
            AnimationEvent ev;
            eventMap.load(ev, evVal.toObject());
            def.events.append(ev);
        }
        animations.insert(it.key(), def);
    }

    // Load states
    states.clear();
    QJsonObject stObj = root.value("states").toObject();
    for (auto it = stObj.begin(); it != stObj.end(); ++it) {
        StateDef sd;
        QJsonObject obj = it.value().toObject();
        stateMap.load(sd, obj);

        // Duration array
        QJsonArray dur = obj.value("duration").toArray();
        if (dur.size() >= 2) {
            sd.durationMin = dur[0].toDouble();
            sd.durationMax = dur[1].toDouble();
        }

        // Transitions array
        QJsonArray trans = obj.value("transitions").toArray();
        for (auto tv : trans) {
            Transition t;
            transMap.load(t, tv.toObject());
            sd.transitions.append(t);
        }
        states.insert(it.key(), sd);
    }

    return true;
}
