#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include <iostream>
#include <memory>
#include <QGuiApplication>
#include <QByteArray>

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
    QString platform = QGuiApplication::platformName();

    if(platform == "wayland"){
        if (qEnvironmentVariableIsSet("HYPRLAND_INSTANCE_SIGNATURE")){
            return std::make_unique<HyprLayerShellHybrid>();
        }
        else if(qEnvironmentVariableIsSet("KDE_FULL_SESSION")){
            return std::make_unique<KWinLayerShellHybrid>();
        }
        return nullptr;
    }
    else if(platform == "xcb")
        return std::make_unique<X11Window>();
    else if(platform == "windows")
        return std::make_unique<WindowsWindow>();
    return nullptr;
}
