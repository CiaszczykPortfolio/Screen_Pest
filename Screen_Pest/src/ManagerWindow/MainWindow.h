#pragma once

#include <QWidget>
#include <QPushButton>
#include <QListWidget>
#include <QPointer>
#include <QVector>

class SpriteWindow;

class MainWindow : public QWidget
{
    Q_OBJECT

public:
    explicit MainWindow(const QString &rootDir = "objects", QWidget *parent = nullptr);

    const QVector<QPointer<SpriteWindow>>& activeWindows() const { return m_activeWindows; }

private slots:
    void onItemDoubleClicked(QListWidgetItem *item);
    void onBackClicked();
    void onWindowDestroyed(QObject *obj);

private:
    void loadDirectory(const QString &path);
    void spawnSpriteWindow(const QString &path, const QString &title);

    QString m_rootPath;
    QString m_currentPath;
    QListWidget *m_listWidget = nullptr;
    QPushButton *m_backButton = nullptr;
    QVector<QPointer<SpriteWindow>> m_activeWindows;
};

