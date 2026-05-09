#include "MainWindow.h"
#include "../SubWindow/SpriteWindow.h"

#include <QVBoxLayout>
#include <QFile>
#include <QDir>
#include <QStyle>
#include <QApplication>
#include <QPointer>

/*
 * Konstruktor: Tworzy główne okno, z podstawowym
 * przeglądaniem assetów
 */
MainWindow::MainWindow(const QString &rootDir, QWidget *parent)
    : QWidget(parent)
{
    QDir dir(rootDir);
    if (!dir.exists()) {
        qWarning("MainWindow: root directory does not exist: %s", qPrintable(rootDir));
        dir.mkpath(".");
    }
    m_rootPath = dir.absolutePath();
    m_currentPath = m_rootPath;

    // Layout
    auto *lay = new QVBoxLayout(this);

    m_backButton = new QPushButton("← Back", this);
    m_backButton->setEnabled(false);
    connect(m_backButton, &QPushButton::clicked, this, &MainWindow::onBackClicked);
    lay->addWidget(m_backButton);

    m_listWidget = new QListWidget(this);
    m_listWidget->setViewMode(QListView::IconMode);
    m_listWidget->setIconSize(QSize(48, 48));
    m_listWidget->setResizeMode(QListView::Adjust);
    m_listWidget->setMovement(QListView::Static);
    m_listWidget->setSpacing(12);
    connect(m_listWidget, &QListWidget::itemDoubleClicked,
            this, &MainWindow::onItemDoubleClicked);
    lay->addWidget(m_listWidget);

    setWindowTitle("Menedżer assetów");
    resize(640, 480);

    loadDirectory(m_currentPath);
}

/*
 * Wczytuje zawartość folderu na podanej ścieżce,
 * klasyfikuje zawartość jako asset albo folder.
 */
void MainWindow::loadDirectory(const QString &path)
{
    m_listWidget->clear();
    QDir dir(path);
    if (!dir.exists()) return;

    const QFileInfoList entries = dir.entryInfoList(QDir::Dirs | QDir::NoDotAndDotDot);
    for (const QFileInfo &info : entries) {
        const QString name = info.fileName();

        // Ignorowanie folderu dev (aby nie zaśmiecać)
        if (name == QStringLiteral("dev"))
            continue;

        const QString fullPath = info.absoluteFilePath();
        const bool isNpc = QFile::exists(fullPath + "/behaviour.json");

        auto *item = new QListWidgetItem();
        item->setText(name);
        item->setIcon(qApp->style()->standardIcon(isNpc ? QStyle::SP_FileIcon : QStyle::SP_DirIcon));
        item->setData(Qt::UserRole, fullPath);
        item->setData(Qt::UserRole + 1, isNpc);   // flag: NPC or folder
        m_listWidget->addItem(item);
    }

    m_backButton->setEnabled(m_currentPath != m_rootPath);
}

/*
 * Obsługa wejścia w asset/folder
 */
void MainWindow::onItemDoubleClicked(QListWidgetItem *item)
{
    const QString path = item->data(Qt::UserRole).toString();
    const bool isNpc = item->data(Qt::UserRole + 1).toBool();

    if (isNpc) {
        spawnSpriteWindow(path, item->text());
    } else {
        m_currentPath = path;
        loadDirectory(m_currentPath);
    }
}

/*
 * Utworzenie okna assetu, dodanie wskaźnika do aktywnych okien
 */
void MainWindow::spawnSpriteWindow(const QString &path, const QString &title)
{
    auto *window = new SpriteWindow(path);
    window->setWindowTitle(title);
    window->setAttribute(Qt::WA_DeleteOnClose);
    window->show();

    m_activeWindows.append(QPointer<SpriteWindow>(window));
    connect(window, &QObject::destroyed, this, &MainWindow::onWindowDestroyed);
}

/*
 * Usunięcie nieważnych wskaźników
 */
void MainWindow::onWindowDestroyed(QObject *obj)
{
    // Usunięcie nieważnych wskaźników
    m_activeWindows.erase(
        std::remove_if(m_activeWindows.begin(), m_activeWindows.end(),
                       [obj](const QPointer<SpriteWindow>& ptr) {
                           return ptr.isNull() || ptr.data() == obj;
                       }),
        m_activeWindows.end());
}

/*
 * Cofnięcie w menu
 */
void MainWindow::onBackClicked()
{
    if (m_currentPath == m_rootPath) return;

    QDir dir(m_currentPath);
    dir.cdUp();
    QString parent = dir.absolutePath();

    // Blokada do niewychodzednia z /assets
    if (!parent.startsWith(m_rootPath))
        parent = m_rootPath;

    m_currentPath = parent;
    loadDirectory(m_currentPath);
}