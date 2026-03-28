#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QLabel>
#include <memory>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    Ui::MainWindow *ui;
};

class FloatingWindow
{
public:
    virtual ~FloatingWindow() = default;

    virtual void show() = 0;
    virtual void hide() = 0;

    virtual void move(int x, int y) = 0;

    virtual void startInteractiveDrag() = 0;

    virtual void onPointerPress(int x, int y) = 0;
};

class WindowsWindow : public FloatingWindow
{
public:
    WindowsWindow() {
        window = new QMainWindow();
        auto *label = new QLabel("This is a Windows window");
        label->setAlignment(Qt::AlignCenter);
        window->setCentralWidget(label);
        window->resize(400, 200);
    }

    ~WindowsWindow() override {
        delete window;
    }

    void show() override { window->show(); }
    void hide() override { window->hide(); }

    void move(int x, int y) override{}
    void startInteractiveDrag() override{}
    void onPointerPress(int x, int y) override{}


    private:
        QMainWindow *window;
};

class KWinLayerShellHybrid : public FloatingWindow
{
public:
    KWinLayerShellHybrid() {
        window = new QMainWindow();
        auto *label = new QLabel("This is a Wayland (KWin) window");
        label->setAlignment(Qt::AlignCenter);
        window->setCentralWidget(label);
        window->resize(400, 200);
    }

    ~KWinLayerShellHybrid() override {
        delete window;
    }

    void show() override { window->show(); }
    void hide() override { window->hide(); }

    void move(int x, int y) override{}
    void startInteractiveDrag() override{}
    void onPointerPress(int x, int y) override{}
private:
    void createLayerSurface();
    void createXdgShellSurface();
    void switchToInteractiveSurface();
    void updateMarginsFromPosition(const QPoint &pos);

    QMainWindow *window;
};

class HyprLayerShellHybrid : public FloatingWindow
{
public:
    HyprLayerShellHybrid() {
        window = new QMainWindow();
        auto *label = new QLabel("This is a Wayland (Hyprland) window");
        label->setAlignment(Qt::AlignCenter);
        window->setCentralWidget(label);
        window->resize(400, 200);
    }

    ~HyprLayerShellHybrid() override {
        delete window;
    }

    void show() override { window->show(); }
    void hide() override { window->hide(); }

    void move(int x, int y) override{}
    void startInteractiveDrag() override{}
    void onPointerPress(int x, int y) override{}
private:
    void createLayerSurface();
    void createXdgShellSurface();
    void switchToInteractiveSurface();
    void updateMarginsFromPosition(const QPoint &pos);

    QMainWindow *window;
};

class X11Window : public FloatingWindow
{
public:
    X11Window() {
        window = new QMainWindow();
        auto *label = new QLabel("This is an X11/XWayland window");
        label->setAlignment(Qt::AlignCenter);
        window->setCentralWidget(label);
        window->resize(400, 200);
    }

    ~X11Window() override {
        delete window;
    }

    void show() override { window->show(); }
    void hide() override { window->hide(); }

    void move(int x, int y) override{}
    void startInteractiveDrag() override{}
    void onPointerPress(int x, int y) override{}

    private:
        QMainWindow *window;
};

std::unique_ptr<FloatingWindow> FloatingWindowFactory();
#endif // MAINWINDOW_H
