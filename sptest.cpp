#include "sptest.h"

sptest::sptest(QWidget *parent)
	: QMainWindow(parent)
{
	ui.setupUi(this);

	_splitter = new ysplitter(this);

	QStringList __vheader;
	__vheader << "EQ, RUR" 
		      << "VALID, RUR" << "COIN, RUR" << "JETS" << "PULSE, RUR" 
			  << "EQ ERR" << "VALID ERR" << "COIN ERR" << "PULSE ERR" 
			  << "EQ STATUS" << "VALID STATUS" << "COIN STATUS" << "fisID";

	ui.tableWidget->setRowCount(13);
	ui.tableWidget->setVerticalHeaderLabels(__vheader);
	ui.tableWidget->setColumnCount(1);
	ui.tableWidget->horizontalHeader()->hide();
	
	_getPortsInfo();

	connect(ui.getPortsInfo, SIGNAL(clicked()), SLOT(_getPortsInfo()));

	connect(ui.startBtn, SIGNAL(clicked()), SLOT(_start()));
	connect(ui.stopBtn, SIGNAL(clicked()), SLOT(_stop()));
	connect(ui.sendFiscalBtn, SIGNAL(clicked()), SLOT(_sendFiscal()));
	connect(_splitter, SIGNAL(gotNewData()), SLOT(gotNewData()));
	connect(_splitter, SIGNAL(fiscalOK()), SLOT(fiscalOK()));
	connect(_splitter, SIGNAL(crcError()), SLOT(crcError()));
	connect(_splitter, SIGNAL(timeout()), SLOT(timeout()));

	connect(ui.eq1spin, SIGNAL(valueChanged(int)), _splitter, SLOT(setEQ1(int)));
	connect(ui.eq2spin, SIGNAL(valueChanged(int)), _splitter, SLOT(setEQ2(int)));
	connect(ui.eq3spin, SIGNAL(valueChanged(int)), _splitter, SLOT(setEQ3(int)));
	connect(ui.eq4spin, SIGNAL(valueChanged(int)), _splitter, SLOT(setEQ4(int)));
	connect(ui.eq5spin, SIGNAL(valueChanged(int)), _splitter, SLOT(setEQ5(int)));

	_splitter->setEQ1(ui.eq1spin->value());
	_splitter->setEQ2(ui.eq2spin->value());
	_splitter->setEQ3(ui.eq3spin->value());
	_splitter->setEQ4(ui.eq4spin->value());
	_splitter->setEQ5(ui.eq5spin->value());

	_running = false;
}

sptest::~sptest()
{
	_splitter->stop();
}

void sptest::gotNewData()
{
	ui.label->setText("ONLINE");

	ui.tableWidget->setItem(IND_EQ, 0, new QTableWidgetItem(QString::number(_splitter->currentState.eq)));
	ui.tableWidget->item(IND_EQ, 0)->setTextAlignment(Qt::AlignVCenter | Qt::AlignHCenter);;

	ui.tableWidget->setItem(VALID, 0, new QTableWidgetItem(QString::number(_splitter->currentState.va)));
	ui.tableWidget->item(VALID, 0)->setTextAlignment(Qt::AlignVCenter | Qt::AlignHCenter);;

	ui.tableWidget->setItem(COIN, 0, new QTableWidgetItem(QString::number(_splitter->currentState.coin)));
	ui.tableWidget->item(COIN, 0)->setTextAlignment(Qt::AlignVCenter | Qt::AlignHCenter);;

	ui.tableWidget->setItem(JETS, 0, new QTableWidgetItem(QString::number(_splitter->currentState.jets)));
	ui.tableWidget->item(JETS, 0)->setTextAlignment(Qt::AlignVCenter | Qt::AlignHCenter);;

	ui.tableWidget->setItem(PULSE, 0, new QTableWidgetItem(QString::number(_splitter->currentState.pulse)));
	ui.tableWidget->item(PULSE, 0)->setTextAlignment(Qt::AlignVCenter | Qt::AlignHCenter);;

	ui.tableWidget->setItem(EQ_ERR, 0, new QTableWidgetItem(QString::number(_splitter->currentState.err_eq)));
	ui.tableWidget->item(EQ_ERR, 0)->setTextAlignment(Qt::AlignVCenter | Qt::AlignHCenter);;

	ui.tableWidget->setItem(VALID_ERR, 0, new QTableWidgetItem(QString::number(_splitter->currentState.err_va)));
	ui.tableWidget->item(VALID_ERR, 0)->setTextAlignment(Qt::AlignVCenter | Qt::AlignHCenter);;

	ui.tableWidget->setItem(COIN_ERR, 0, new QTableWidgetItem(QString::number(_splitter->currentState.err_coin)));
	ui.tableWidget->item(COIN_ERR, 0)->setTextAlignment(Qt::AlignVCenter | Qt::AlignHCenter);;

	ui.tableWidget->setItem(PULSE_ERR, 0, new QTableWidgetItem(QString::number(_splitter->currentState.err_pulse)));
	ui.tableWidget->item(PULSE_ERR, 0)->setTextAlignment(Qt::AlignVCenter | Qt::AlignHCenter);;

	ui.tableWidget->setItem(EQ_STATUS, 0, new QTableWidgetItem(QString::number(_splitter->currentState.status_eq)));
	ui.tableWidget->item(EQ_STATUS, 0)->setTextAlignment(Qt::AlignVCenter | Qt::AlignHCenter);;

	ui.tableWidget->setItem(VALID_STATUS, 0, new QTableWidgetItem(QString::number(_splitter->currentState.status_va)));
	ui.tableWidget->item(VALID_STATUS, 0)->setTextAlignment(Qt::AlignVCenter | Qt::AlignHCenter);;

	ui.tableWidget->setItem(COIN_STATUS, 0, new QTableWidgetItem(QString::number(_splitter->currentState.status_coin)));
	ui.tableWidget->item(COIN_STATUS, 0)->setTextAlignment(Qt::AlignVCenter | Qt::AlignHCenter);;

	ui.tableWidget->setItem(FIS_ID, 0, new QTableWidgetItem(QString::number(_splitter->currentState.fisID)));
	ui.tableWidget->item(FIS_ID, 0)->setTextAlignment(Qt::AlignVCenter | Qt::AlignHCenter);;
}

void sptest::fiscalOK()
{
	ui.label->setText("fiscalOK");
}

void sptest::crcError()
{
	ui.label->setText("crcError");
}

void sptest::timeout()
{
	ui.label->setText("timeout");
}

void sptest::_getPortsInfo()
{
	QList<QSerialPortInfo> __portList = QSerialPortInfo::availablePorts();

	ui.comboBox->clear();

	for (int i = 0; i < __portList.count(); i++) {
		ui.comboBox->addItem(__portList.at(i).portName());
	}
}

void sptest::_start()
{
	if (_running) return;

	_splitter->init(ui.comboBox->currentText());
	_splitter->start();
	_running = true;
}

void sptest::_stop()
{
	_splitter->stop();
	_running = false;
}

void sptest::_sendFiscal()
{
	_splitter->sendFiscal((quint32)ui.fisIDspin->value(), (quint16)ui.sumSpin->value(), (quint16)ui.itemIDspin->value());
}
