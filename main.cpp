#include "ratransfert.h"
#include <QApplication>
#include "wdrequest.h"

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    QApplication::setOrganizationName("TRAPTA");
    QApplication::setOrganizationDomain("trapta.eu");
    QApplication::setApplicationName("RA Transfer");

    RATransfert w(0);
    w.show();

    return app.exec();

}
