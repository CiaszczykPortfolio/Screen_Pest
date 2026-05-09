#include "SpriteWindow.h"
#include "BehaviourData.h"
#include "SpriteAtlas.h"
#include "AnimationEngine.h"
#include "ActionDispatcher.h"

#include <QPainter>
#include <LayerShellQt/window.h>
#include <QMouseEvent>

/*
 *Konstruktor: Tworzy okno (z layershellqt dla Waylanda)
 * wczytuje schemat, Sprite'y, akcje, rozpoczyna timer.
 */
SpriteWindow::SpriteWindow(const QString &resourcePath, QWidget *parent)
    : QWidget(parent, Qt::Window | Qt::WindowStaysOnTopHint)//| Qt::FramelessWindowHint
{
    setAttribute(Qt::WA_TranslucentBackground);
    setStyleSheet("background: transparent;");

    // Wczytanie schematu
    m_data = new BehaviourData;
    m_data->loadFromJson(resourcePath + "/behaviour.json");

    m_atlas = new SpriteAtlas;
    m_atlas->load(*m_data, resourcePath + "/sprites");
    setFixedSize(m_atlas->frameSize());

    m_dispatcher = new ActionDispatcher(this);
    // Wczytanie akcji do Dispatchera

    m_engine = new AnimationEngine(*m_data, *m_dispatcher);
    connect(m_engine, &AnimationEngine::frameAdvanced, this, qOverload<>(&QWidget::update));
    connect(m_engine, &AnimationEngine::nextFrameTimeout, this, [this](int msec) {
        m_timer.start(msec);
    });
    m_engine->reset();

    m_timer.setSingleShot(true);                     // timer na jeden update
    connect(&m_timer, &QTimer::timeout, this, &SpriteWindow::tick);

    //Połączenie z sygnałami od Animacji, update co klatkę
    connect(m_engine, &AnimationEngine::frameAdvanced,
            this, qOverload<>(&QWidget::update));
    connect(m_engine, &AnimationEngine::nextFrameTimeout,
            this, [this](int msec) { m_timer.start(msec); });

    m_screenPos = QPoint(500, 500);   // or any initial position you like

    m_elapsed.start();
    m_lastElapsed = 0;
    m_engine->update(0.0);   // pierwszy trigger
}

/*
 * przesłania pokazanie okna z
 * dodaniem obsługi layer shell
 */
void SpriteWindow::showEvent(QShowEvent *event) {
    QWidget::showEvent(event);   // call base class first

    // Configure layer-shell now that the native window is shown
    if (auto *shellWin = LayerShellQt::Window::get(windowHandle())) {
        shellWin->setLayer(LayerShellQt::Window::LayerOverlay);
        shellWin->setKeyboardInteractivity(
            LayerShellQt::Window::KeyboardInteractivityNone);
        shellWin->setAnchors(static_cast<LayerShellQt::Window::Anchors>(
            LayerShellQt::Window::AnchorTop | LayerShellQt::Window::AnchorLeft));
            shellWin->setMargins(QMargins(m_screenPos.x(), m_screenPos.y(), 0, 0));
    }
}

void SpriteWindow::mousePressEvent(QMouseEvent *event) {
    if (event->button() == Qt::RightButton) {
        close();               // self‑destruct (WA_DeleteOnClose is set)
        return;
    }
    QWidget::mousePressEvent(event);
}

/*
 * Destruktor
 */
SpriteWindow::~SpriteWindow() {
    delete m_data;
    delete m_atlas;
    delete m_engine;
    delete m_dispatcher;
}

/*
 * Wysyła silnikowi animacji czasy wywołań.
 */
void SpriteWindow::tick() {
    qint64 now = m_elapsed.elapsed();
    double delta = (now - m_lastElapsed) / 1000.0;
    m_lastElapsed = now;
    if (delta > 0.1) delta = 0.1;
    m_engine->update(delta);
}

/*
 * Przemalowanie okna
 */
void SpriteWindow::paintEvent(QPaintEvent *) {
    QPainter painter(this);
    painter.setRenderHint(QPainter::SmoothPixmapTransform, false);

    // Pobranie nazwy aktualnej klatki
    const AnimationDef &ad = m_data->animations[m_data->states[m_engine->currentState].animation];
    int frameNum = m_engine->currentFrameIndex + 1;
    QString spriteName = ad.prefix + QString::number(frameNum) + ".png";
    const QPixmap *pix = m_atlas->get(spriteName);
    if (pix)
        painter.drawPixmap(0, 0, *pix);
}

/*
 * przesuwa okno o x, y
 * wykorzystuje layer shell jeśli dostępny
 */
void SpriteWindow::moveBy(double dx, double dy)
{
    // Calculate new absolute position
    m_screenPos += QPoint(static_cast<int>(dx), static_cast<int>(dy));

    fprintf( stderr, "moveBy called: %f %f, newpos: %i %i \n", dx, dy, (pos().x() + static_cast<int>(dx)), pos().y() + static_cast<int>(dy));


    // Try LayerShell first (Wayland)
    if (auto *shellWin = LayerShellQt::Window::get(windowHandle())) {
        fprintf(stderr, "Using layer-shell margins \n");
        shellWin->setMargins(QMargins(m_screenPos.x(), m_screenPos.y(), 0, 0));
    } else {
        // Fallback for X11 / Windows / macOS
        move(m_screenPos);
    }
}