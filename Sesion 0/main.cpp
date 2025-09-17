#include <QtWidgets>
#include "ejemplo1.h"
#include<iostream>

int main(int argc, char** argv)
{
    std::cerr << "Pingocout";    //Patata
    QApplication app(argc, argv);
    ejemplo1 foo;
    foo.show();
    return app.exec();
}
