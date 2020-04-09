#include "sptest.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	sptest w;
	w.show();
	return a.exec();
}
