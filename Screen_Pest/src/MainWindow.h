// ------------------------- mainwindow.h -------------------------
#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QWidget>
#include <QPushButton>
#include <QListWidget>
#include <QStack>
#include <QDir>

class MainWindow : public QWidget
{
    Q_OBJECT

public:
    explicit MainWindow(const QString &rootDir = "objects", QWidget *parent = nullptr);

private slots:
    void onItemDoubleClicked(QListWidgetItem *item);
    void onBackClicked();

private:
    void loadDirectory(const QString &path);

    QString rootPath;
    QString currentPath;          // absolute path we are viewing
    QListWidget *listWidget;
    QPushButton *backButton;
};

#endif // MAINWINDOW_H
