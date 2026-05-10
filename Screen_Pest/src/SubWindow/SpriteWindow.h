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
    void switchState(const QString &stateName);

protected:
    void paintEvent(QPaintEvent *event) override;
    void showEvent(QShowEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    //void moveEvent(QMoveEvent *event) override;   //przesunięcie okna
    //bool event(QEvent *event) override;           // zmiana wymiarów ekranu

private slots:
    void tick();

private:
    void updateScreenGeometry();

    QRect m_screenGeometry;    // wymiary ekranu
    QPoint m_screenPos;        // pozycja na ekranie

    BehaviourData *m_data;       // or std::unique_ptr
    SpriteAtlas *m_atlas;
    AnimationEngine *m_engine;
    ActionDispatcher *m_dispatcher;

    QTimer m_timer;
    QElapsedTimer m_elapsed;
    qint64 m_lastElapsed;
};
