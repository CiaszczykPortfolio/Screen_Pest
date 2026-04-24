// spritewindow.h
#ifndef SPRITEWINDOW_H
#define SPRITEWINDOW_H

#include <QWidget>
#include <QTimer>
#include <QElapsedTimer>
//#include <memory>  // if using unique_ptr

class BehaviourData;
class SpriteAtlas;
class AnimationEngine;
class ActionDispatcher;

class SpriteWindow : public QWidget {
    Q_OBJECT
public:
    explicit SpriteWindow(const QString &resourcePath, QWidget *parent = nullptr);
    ~SpriteWindow();

protected:
    void paintEvent(QPaintEvent *event) override;

private slots:
    void tick();

private:
    BehaviourData *m_data;       // or std::unique_ptr
    SpriteAtlas *m_atlas;
    AnimationEngine *m_engine;
    ActionDispatcher *m_dispatcher;

    QTimer m_timer;
    QElapsedTimer m_elapsed;
    qint64 m_lastElapsed;
};

#endif
