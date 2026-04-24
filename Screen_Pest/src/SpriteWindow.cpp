#include "SpriteWindow.h"
#include "BehaviourData.h"
#include "SpriteAtlas.h"
#include "AnimationEngine.h"
#include "ActionDispatcher.h"
#include <QPainter>

SpriteWindow::SpriteWindow(const QString &resourcePath, QWidget *parent)
    : QWidget(parent, Qt::Window | Qt::WindowStaysOnTopHint)//| Qt::FramelessWindowHint
{
    setAttribute(Qt::WA_TranslucentBackground);
    setStyleSheet("background: transparent;");

    // Composition
    m_data = new BehaviourData;
    m_data->loadFromJson(resourcePath + "/behaviour.json");

    m_atlas = new SpriteAtlas;
    m_atlas->load(*m_data, resourcePath + "/sprites");
    setFixedSize(m_atlas->frameSize());

    m_dispatcher = new ActionDispatcher(this);
    // load per-NPC action overrides from JSON (if any) into m_dispatcher

    m_engine = new AnimationEngine(*m_data, *m_dispatcher);
    m_engine->reset();

    m_elapsed.start();
    m_lastElapsed = 0;
    connect(&m_timer, &QTimer::timeout, this, &SpriteWindow::tick);
    m_timer.start(16);
}

SpriteWindow::~SpriteWindow() {
    delete m_data;
    delete m_atlas;
    delete m_engine;
    delete m_dispatcher;
}

void SpriteWindow::tick() {
    qint64 now = m_elapsed.elapsed();
    double delta = (now - m_lastElapsed) / 1000.0;
    m_lastElapsed = now;
    if (delta > 0.1) delta = 0.1;
    m_engine->update(delta);
    update();
}

void SpriteWindow::paintEvent(QPaintEvent *) {
    QPainter painter(this);
    painter.setRenderHint(QPainter::SmoothPixmapTransform, false);

    // Get current frame name from engine + data
    const AnimationDef &ad = m_data->animations[m_data->states[m_engine->currentState].animation];
    int frameNum = m_engine->currentFrameIndex + 1;
    QString spriteName = ad.prefix + QString::number(frameNum) + ".png";
    const QPixmap *pix = m_atlas->get(spriteName);
    if (pix)
        painter.drawPixmap(0, 0, *pix);
}
