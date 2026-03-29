#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <iostream>
#include <memory>
#include <QGuiApplication>
#include <QByteArray>

enum class Platform {
    Wayland,
    Xcb,
    Windows,
    Unknown
};

static const QHash<QString, Platform> platformMap = {
    {"wayland", Platform::Wayland},
    {"xcb",     Platform::Xcb},
    {"windows", Platform::Windows}
};

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}


std::unique_ptr<FloatingWindow> FloatingWindowFactory() {
    Platform p = platformMap.value(QGuiApplication::platformName(), Platform::Unknown);

    switch (p){
    case Platform::Wayland:
            if (qEnvironmentVariableIsSet("HYPRLAND_INSTANCE_SIGNATURE"))
                return std::make_unique<HyprLayerShellHybrid>();
            else if(qEnvironmentVariableIsSet("KDE_FULL_SESSION"))
                return std::make_unique<KWinLayerShellHybrid>();
            return nullptr;
        case Platform::Xcb:
            return std::make_unique<X11Window>();
        case Platform::Windows:
            return std::make_unique<WindowsWindow>();
        default:
            return nullptr;
    }
}
