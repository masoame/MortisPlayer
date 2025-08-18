#include<QApplication>
#include "holomainwindow.hpp"
int main(int argc, char* args[])
{
    QApplication app(argc, args);
    HoloMainWindow w;
    w.show();
    return app.exec();
}
