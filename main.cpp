#include "UI/MainWindow.h"
#include <QApplication>
#include <QSplashScreen>

#include "core.h"
#include "engineinit.h"

int main(int argc, char *argv[])
{
    gea::EngineInit::PreInitializeEngine();

    QApplication a(argc, argv);
    QSplashScreen *mSplash = new QSplashScreen;
    mSplash->setPixmap(QPixmap(QString::fromStdString(gea::EngineInit::splashArtPath.string())));
    mSplash->show();

    gea::EngineInit::InitializeEngine();

    MainWindow w;
    w.move(200, 100);
    QLog("Cool log message from the main.cpp", "blue"); //NOTE: For testing
    QLog("Even cooler log from the main.cpp", "white"); //NOTE: For testing


    w.show();
    w.start();

    mSplash->hide();
    //mSplash->finish(&w); //Splashscreen closes itself once window is up


    return a.exec();
}
