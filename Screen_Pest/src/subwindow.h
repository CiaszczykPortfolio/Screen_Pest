#ifndef SUBWINDOW_H
#define SUBWINDOW_H

#include <QWidget>
#include <QElapsedTimer>
#include <QTimer>
#include <QMap>
#include <QString>
#include <QPixmap>

class SubWindow : public QWidget
{
    Q_OBJECT

public:
    explicit SubWindow(const QString &resourcePath, QWidget *parent = nullptr);
    ~SubWindow();

protected:
    void paintEvent(QPaintEvent *event) override;

private slots:
    void tick();

private:
    struct Transition {
        QString to;
        double weight;
    };

    struct StateDef {
        QString animation;
        double durationMin;
        double durationMax;
        QVector<Transition> transitions;
    };

    struct AnimationDef {
        QString prefix;
        int frameCount;
        double fps;
        bool loop;
    };

    QMap<QString, AnimationDef> animations;
    QMap<QString, StateDef> states;
    QString initialState;
    double speed;              // kept for compatibility, unused now
    double globalFrameJitter;
    int scaleFactor = 16;      // default if not in JSON

    QString currentState;
    double stateTimer;
    int currentFrameIndex;
    double frameTimer;
    double currentFrameDuration;

    QMap<QString, QPixmap> sprites;
    QTimer updateTimer;
    QElapsedTimer elapsed;
    qint64 lastElapsed;

    void loadJson(const QString &jsonPath);
    void loadSprites(const QString &spritesFolder);
    void resetState(const QString &stateName);
    void updateAnimation(double deltaTime);
    QString currentSpriteName() const;
};

#endif // SUBWINDOW_H
