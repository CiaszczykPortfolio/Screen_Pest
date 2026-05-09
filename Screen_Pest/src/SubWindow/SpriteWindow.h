#pragma once

#include <QWidget>
#include <QTimer>
#include <QElapsedTimer>

class BehaviourData;
class SpriteAtlas;
class AnimationEngine;
class ActionDispatcher;

class SpriteWindow : public QWidget {
    Q_OBJECT
public:
    explicit SpriteWindow(const QString &resourcePath, QWidget *parent = nullptr);
    ~SpriteWindow();
    void moveBy(double dx, double dy);

protected:
    void paintEvent(QPaintEvent *event) override;
    void showEvent(QShowEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;

private slots:
    void tick();

private:
    QPoint m_screenPos;

    BehaviourData *m_data;       // or std::unique_ptr
    SpriteAtlas *m_atlas;
    AnimationEngine *m_engine;
    ActionDispatcher *m_dispatcher;

    QTimer m_timer;
    QElapsedTimer m_elapsed;
    qint64 m_lastElapsed;
};
