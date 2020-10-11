#ifndef SPTEST_H
#define SPTEST_H

#include <QtWidgets/QMainWindow>
#include "ui_sptest.h"
#include <QSerialPort>
#include <QSerialPortInfo>
#include <QDebug>

#include "ysplitter.h"

#define IND_EQ 0
#define VALID 1
#define COIN 2
#define JETS 3
#define PULSE 4
#define EQ_ERR 5
#define VALID_ERR 6
#define COIN_ERR 7
#define PULSE_ERR 8
#define EQ_STATUS 9
#define VALID_STATUS 10
#define COIN_STATUS 11
#define FIS_ID 12
#define COIN_PULSE 13

class sptest : public QMainWindow
{
	Q_OBJECT

public:
	sptest(QWidget *parent = 0);
	~sptest();

public slots:
	void gotNewData();
	void fiscalOK();
	void crcError();
	void timeout();

private:
	Ui::sptestClass ui;
	ysplitter *_splitter;
	bool _running;

private slots:
	void _getPortsInfo();
	void _start();
	void _stop();
	void _sendFiscal();
};

#endif // SPTEST_H
