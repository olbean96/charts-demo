#include "graphmanager.h"
#include "graphview.h"

#include <QGuiApplication>
#include <QDir>
#include <QQmlApplicationEngine>
#include <QQmlContext>

#define CHARTS_DEMO_STRINGIFY_DETAIL(value) #value
#define CHARTS_DEMO_STRINGIFY(value) CHARTS_DEMO_STRINGIFY_DETAIL(value)

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);

    qmlRegisterType<GraphView>("ChartsDemo", 1, 0, "GraphView");

    GraphManager manager;
    manager.createDemoData();

    QQmlApplicationEngine engine;
    engine.rootContext()->setContextProperty("graphManager", &manager);
    const QString qmlPath =
        QDir(QStringLiteral(CHARTS_DEMO_STRINGIFY(CHARTS_DEMO_QML_DIR))).absoluteFilePath(QStringLiteral("main.qml"));
    engine.load(QUrl::fromLocalFile(qmlPath));

    if (engine.rootObjects().isEmpty()) {
        return -1;
    }

    return app.exec();
}
