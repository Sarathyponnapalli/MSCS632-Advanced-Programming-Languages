#include <QApplication>
#include <QFont>
#include "MainWindow.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    app.setApplicationName("RideShare System");
    app.setApplicationVersion("1.0");
    app.setOrganizationName("MSCS632-Advanced-Programming-Languages");

    // Use a clean system font as fallback
    QFont defaultFont("Segoe UI", 10);
    defaultFont.setStyleHint(QFont::SansSerif);
    app.setFont(defaultFont);

    MainWindow win;
    win.show();
    return app.exec();
}
