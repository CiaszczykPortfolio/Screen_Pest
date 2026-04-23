// ------------------------- mainwindow.cpp -------------------------
#include "mainwindow.h"
#include "subwindow.h"

#include <QVBoxLayout>
#include <QLabel>
#include <QFile>
#include <QStyle>
#include <QApplication>

MainWindow::MainWindow(const QString &rootDir, QWidget *parent)
    : QWidget(parent)
{
    // ---- Normalise and verify the root path ----
    QDir dir(rootDir);
    if (!dir.exists()) {
        qWarning("MainWindow: root directory does not exist: %s", qPrintable(rootDir));
        dir.mkpath(".");
    }
    rootPath = dir.absolutePath();
    currentPath = rootPath;

    // ---- GUI layout ----
    auto *mainLayout = new QVBoxLayout(this);

    // Back button (arrow) at the top
    backButton = new QPushButton("← Back", this);
    backButton->setEnabled(false);  // at root, nothing to go back to
    connect(backButton, &QPushButton::clicked, this, &MainWindow::onBackClicked);
    mainLayout->addWidget(backButton);

    // Grid view of items
    listWidget = new QListWidget(this);
    listWidget->setViewMode(QListView::IconMode);
    listWidget->setIconSize(QSize(48, 48));
    listWidget->setResizeMode(QListView::Adjust);
    listWidget->setMovement(QListView::Static);
    listWidget->setSpacing(12);
    connect(listWidget, &QListWidget::itemDoubleClicked,
            this, &MainWindow::onItemDoubleClicked);
    mainLayout->addWidget(listWidget);

    setWindowTitle("NPC Spawner");
    resize(800, 600);

    // Load initial directory
    loadDirectory(currentPath);
}

// ----------------------------------------------------------------------
// Populate the list widget from the given absolute path
// ----------------------------------------------------------------------
void MainWindow::loadDirectory(const QString &path)
{
    listWidget->clear();
    QDir dir(path);
    if (!dir.exists()) return;

    // Get all sub‑directories (ignore . and ..)
    QFileInfoList entries = dir.entryInfoList(QDir::Dirs | QDir::NoDotAndDotDot);
    for (const QFileInfo &info : entries) {
        QString name = info.fileName();
        QString fullPath = info.absoluteFilePath();
        bool hasBehaviour = QFile::exists(fullPath + "/behaviour.json");

        QListWidgetItem *item = new QListWidgetItem();
        item->setText(name);
        // Use different icons for folders vs. NPCs
        if (hasBehaviour) {
            item->setIcon(qApp->style()->standardIcon(QStyle::SP_FileIcon));
            item->setData(Qt::UserRole, fullPath);  // store full path
            item->setData(Qt::UserRole + 1, true);  // flag: is NPC
        } else {
            item->setIcon(qApp->style()->standardIcon(QStyle::SP_DirIcon));
            item->setData(Qt::UserRole, fullPath);
            item->setData(Qt::UserRole + 1, false); // flag: is folder
        }
        listWidget->addItem(item);
    }

    // Enable / disable the back button
    backButton->setEnabled(currentPath != rootPath);
}

// ----------------------------------------------------------------------
// Double‑click handler
// ----------------------------------------------------------------------
void MainWindow::onItemDoubleClicked(QListWidgetItem *item)
{
    QString path = item->data(Qt::UserRole).toString();
    bool isNpc = item->data(Qt::UserRole + 1).toBool();

    if (isNpc) {
        // Spawn a new autonomous NPC window
        SubWindow *sub = new SubWindow(path);
        sub->setWindowTitle(item->text());
        sub->setAttribute(Qt::WA_DeleteOnClose);
        sub->show();
    } else {
        // Navigate into the folder
        currentPath = path;
        loadDirectory(currentPath);
    }
}

// ----------------------------------------------------------------------
// Back button – go up one level, but never above rootPath
// ----------------------------------------------------------------------
void MainWindow::onBackClicked()
{
    if (currentPath == rootPath) return;  // safety

    QDir dir(currentPath);
    dir.cdUp();
    QString parent = dir.absolutePath();

    // Prevent leaving the root directory
    if (!parent.startsWith(rootPath)) {
        currentPath = rootPath;
    } else {
        currentPath = parent;
    }
    loadDirectory(currentPath);
}
