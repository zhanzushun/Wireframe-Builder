
#include <QApplication>
#include "mainwindow.hxx"

int main(int argc, char **argv)
{
    Q_INIT_RESOURCE(qmockups);

    QApplication app(argc, argv);
        app.setStyleSheet(
        "#documentTabs QFrame {background-color: qlineargradient(x1:0, y1:0, x2:0, y2:1, stop:0 rgba(0, 0, 0, 255), stop:0  #eef, stop: 1 #ccf);}"
        "#documentTabs QPushbutton {border: 2px solid #8f8f91;border-radius: 6px;color: rgb(255, 0, 0); padding: 3px 5px 3px 5px;}");

    MainWindow win;
    win.resize(800, 600);
    win.show();

    return app.exec();
};
