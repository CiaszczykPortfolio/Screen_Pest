#include "subwindow.h"
#include <QPainter>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QFile>
#include <QDir>
#include <QRandomGenerator>
#include <QtMath>

SubWindow::SubWindow(const QString &resourcePath, QWidget *parent)
    : QWidget(parent, Qt::Window | Qt::WindowStaysOnTopHint) //Qt::FramelessWindowHint
{
    // Transparent background for a clean desktop pet look
    setAttribute(Qt::WA_TranslucentBackground);
    setStyleSheet("background: transparent;");

    loadJson(resourcePath + "/behaviour.json");
    loadSprites(resourcePath + "/sprites");

    // Window size = scaled sprite size (assume all frames same size)
    if (!sprites.isEmpty()) {
        QSize scaledSize = sprites.first().size();
        setFixedSize(scaledSize);
    } else {
        setFixedSize(32 * scaleFactor, 32 * scaleFactor);
    }

    // State machine
    currentState = initialState;
    resetState(currentState);

    // Timer ~60 FPS
    elapsed.start();
    lastElapsed = 0;
    connect(&updateTimer, &QTimer::timeout, this, &SubWindow::tick);
    updateTimer.start(16);
}

SubWindow::~SubWindow() {}

void SubWindow::loadJson(const QString &jsonPath)
{
    QFile file(jsonPath);
    if (!file.open(QIODevice::ReadOnly)) {
        qWarning("Cannot open behaviour.json: %s", qPrintable(jsonPath));
        return;
    }
    QJsonDocument doc = QJsonDocument::fromJson(file.readAll());
    QJsonObject root = doc.object();

    speed = root.value("speed").toDouble(80);
    initialState = root.value("initialState").toString("idle");
    globalFrameJitter = root.value("globalFrameJitter").toDouble(0.08);
    scaleFactor = root.value("scaleFactor").toInt(16);   // <<< NEW

    QJsonObject animObj = root.value("animations").toObject();
    for (auto it = animObj.begin(); it != animObj.end(); ++it) {
        QJsonObject obj = it.value().toObject();
        AnimationDef def;
        def.prefix     = obj.value("prefix").toString();
        def.frameCount = obj.value("frames").toInt(1);
        def.fps        = obj.value("fps").toDouble(4);
        def.loop       = obj.value("loop").toBool(true);
        animations.insert(it.key(), def);
    }

    QJsonObject stateObj = root.value("states").toObject();
    for (auto it = stateObj.begin(); it != stateObj.end(); ++it) {
        QJsonObject obj = it.value().toObject();
        StateDef def;
        def.animation   = obj.value("animation").toString();
        QJsonArray dur  = obj.value("duration").toArray();
        def.durationMin = dur.at(0).toDouble(2.0);
        def.durationMax = dur.at(1).toDouble(5.0);

        QJsonArray trans = obj.value("transitions").toArray();
        for (const auto &t : trans) {
            QJsonObject tObj = t.toObject();
            Transition tr;
            tr.to     = tObj.value("to").toString();
            tr.weight = tObj.value("weight").toDouble(1.0);
            def.transitions.append(tr);
        }
        states.insert(it.key(), def);
    }
}

void SubWindow::loadSprites(const QString &spritesFolder)
{
    QDir dir(spritesFolder);
    if (!dir.exists()) {
        qWarning("Sprites folder does not exist: %s", qPrintable(spritesFolder));
        return;
    }

    for (auto it = animations.begin(); it != animations.end(); ++it) {
        const AnimationDef &def = it.value();
        for (int i = 1; i <= def.frameCount; ++i) {
            QString fileName = def.prefix + QString::number(i) + ".png";
            QString fullPath = dir.filePath(fileName);
            QPixmap pix(fullPath);
            if (!pix.isNull()) {
                // Scale 16x with nearest neighbour (no blur)
                pix = pix.scaled(pix.size() * scaleFactor,
                                 Qt::IgnoreAspectRatio,
                                 Qt::FastTransformation);
                sprites.insert(fileName, pix);
            } else {
                qWarning("Missing sprite: %s", qPrintable(fullPath));
            }
        }
    }
}

void SubWindow::resetState(const QString &stateName)
{
    currentState = stateName;
    const StateDef &sd = states[stateName];
    double range = sd.durationMax - sd.durationMin;
    stateTimer = sd.durationMin + QRandomGenerator::global()->generateDouble() * range;

    currentFrameIndex = 0;
    frameTimer = 0;

    const AnimationDef &ad = animations[sd.animation];
    double base = 1.0 / ad.fps;
    currentFrameDuration = base + QRandomGenerator::global()->generateDouble() * globalFrameJitter;
}

QString SubWindow::currentSpriteName() const
{
    const AnimationDef &ad = animations[states[currentState].animation];
    int frameNum = currentFrameIndex + 1;
    return ad.prefix + QString::number(frameNum) + ".png";
}

void SubWindow::tick()
{
    qint64 now = elapsed.elapsed();
    double delta = (now - lastElapsed) / 1000.0;
    lastElapsed = now;
    if (delta > 0.1) delta = 0.1;

    updateAnimation(delta);
    update();
}

void SubWindow::updateAnimation(double deltaTime)
{
    const StateDef &stateDef = states[currentState];
    const AnimationDef &animDef = animations[stateDef.animation];

    // 1. State timer
    stateTimer -= deltaTime;
    if (stateTimer <= 0.0) {
        const QVector<Transition> &trans = stateDef.transitions;
        if (!trans.isEmpty()) {
            double totalWeight = 0;
            for (const auto &t : trans) totalWeight += t.weight;
            double r = QRandomGenerator::global()->generateDouble() * totalWeight;
            QString nextState;
            for (const auto &t : trans) {
                r -= t.weight;
                if (r <= 0.0) {
                    nextState = t.to;
                    break;
                }
            }
            if (nextState.isEmpty()) nextState = trans.first().to;
            resetState(nextState);
        } else {
            resetState(currentState);
        }
        updateAnimation(0);
        return;
    }

    // 2. Advance animation frame
    frameTimer += deltaTime;
    while (frameTimer >= currentFrameDuration) {
        frameTimer -= currentFrameDuration;
        currentFrameIndex++;
        if (currentFrameIndex >= animDef.frameCount) {
            if (animDef.loop) {
                currentFrameIndex = 0;
            } else {
                currentFrameIndex = animDef.frameCount - 1;
            }
        }
        double base = 1.0 / animDef.fps;
        currentFrameDuration = base + QRandomGenerator::global()->generateDouble() * globalFrameJitter;
    }

}

void SubWindow::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::SmoothPixmapTransform, false);
    QString name = currentSpriteName();
    if (sprites.contains(name)) {
        const QPixmap &pix = sprites[name];
        painter.drawPixmap(0, 0, pix);
    }
}
