#ifndef YSPLITTER_H
#define YSPLITTER_H

#include <QObject>
#include <QSerialPort>
#include <QSerialPort>
#include <QDebug>
#include <QTimer>

#define SPL_CMD_RESET   1
#define SPL_CMD_POOL    2
#define SPL_CMD_FISCAL  4

#define PACKET_SIZE 30
#define PERIOD 300

class ysplitter : public QObject
{
	Q_OBJECT
	
	union translate {
		quint32 in;
		quint8 b[4];
	};


	struct splitter
	{
		quint16 eq			= 0;
		quint16 va			= 0;
		quint16 coin		= 0;
		quint16 jets		= 0;
		quint16 pulse       = 0;
		quint16 err_eq		= 0;
		quint16 err_va		= 0;
		quint16 err_coin	= 0;
		quint16 err_pulse   = 0;
		quint8 status_eq	= 0;
		quint8 status_va	= 0;
		quint8 status_coin  = 0;
		quint32 fisID		= 0;
	};

public:
	ysplitter(QObject *parent);
	~ysplitter();

	void init(QString portName);
	
	void start();
	void stop();

	void sendFiscal(quint32 fisID, quint16 fisSum, quint16 fisItemID);

	splitter currentState;

	quint32 timeouts;

private:
	QSerialPort * _serial;

	QByteArray _out_buffer;
	QByteArray _in_buffer;

	void _closePort();

	quint8  _calc_crc(QByteArray *buffer, quint8 cnt, quint8 fromPOS);

	void _send();

	void _sendReset();
	void _sendFiscal();
	void _sendPool();

	void _parse_answer();

	quint8 _state;

	quint16 _eq1;
	quint16 _eq2;
	quint16 _eq3;
	quint16 _eq4;
	quint16 _eq5;

	quint32 _fisID;
	quint16 _fisSum;
	quint16 _fisItemID;

	bool _fiscaling;
	bool _pooling;
	bool _running;
	bool _waitingRequest;
	
	void _append2Byte(quint16 value);
	void _append4Byte(quint32 value);
	quint16  _get2Byte(quint8 i);
	quint32  _get4Byte(quint8 i);


private slots:
	void _gotSerialData();
	void _timerSingleShot();

signals:
	void gotNewData();
	void fiscalOK();
	void crcError();
	void timeout();

public slots:
	void setEQ1(int eq_sum) { _eq1 = eq_sum; }
	void setEQ2(int eq_sum) { _eq2 = eq_sum; }
	void setEQ3(int eq_sum) { _eq3 = eq_sum; }
	void setEQ4(int eq_sum) { _eq4 = eq_sum; }
	void setEQ5(int eq_sum) { _eq5 = eq_sum; }

};

#endif // YSPLITTER_H
