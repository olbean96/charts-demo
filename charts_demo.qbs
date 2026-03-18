import qbs

Project {
    CppApplication {
        name: "charts_demo"
        consoleApplication: false

        Depends { name: "cpp" }
        Depends { name: "Qt"; submodules: ["core", "gui", "qml", "quick"] }

        cpp.cxxLanguageVersion: "c++17"
        cpp.defines: [
            "CHARTS_DEMO_QML_DIR=" + sourceDirectory + "/qml",
        ]

        files: [
            "src/axisconfig.h",
            "src/graphmanager.cpp",
            "src/graphmanager.h",
            "src/graphpoint.h",
            "src/graphrenderer.cpp",
            "src/graphrenderer.h",
            "src/graphseries.cpp",
            "src/graphseries.h",
            "src/graphseriesmodel.cpp",
            "src/graphseriesmodel.h",
            "src/graphview.cpp",
            "src/graphview.h",
            "src/legendmodel.cpp",
            "src/legendmodel.h",
            "src/main.cpp",
            "qml/ChartsWidget.qml",
            "qml/main.qml",
        ]
    }
}
