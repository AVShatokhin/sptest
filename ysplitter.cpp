#include "ysplitter.h"

ysplitter::ysplitter(QObject *parent)
	: QObject(parent)
{
	_serial = new QSerialPort(this);
	_fiscaling = false;
	_pooling = false;
	_running = false;
	timeouts = 0;
	connect(_serial, SIGNAL(readyRead()), SLOT(_gotSerialData()));
	_eq1 = 0;
	_eq2 = 0;
	_eq3 = 0;
	_eq4 = 0;
	_eq5 = 0;
}

ysplitter::~ysplitter()
{

}

void ysplitter::start()
{
	_sendReset();
	_pooling = false;
	_running = true;
	QTimer::singleShot(PERIOD, this, SLOT(_timerSingleShot()));
}

void ysplitter::stop()
{
	_closePort();
	_running = false;
}

void ysplitter::sendFiscal(quint32 fisID, quint16 fisSum, quint16 fisItemID)
{
	if (!_fiscaling) {
		_fisID = fisID;
		_fisSum = fisSum;
		_fisItemID = fisItemID;
		_fiscaling = true;
	}
}

void ysplitter::init(QString PortName)
{
	if (_serial->isOpen()) {
		_serial->close();
	}

	_serial->setPortName(PortName);
	_serial->open(QIODevice::ReadWrite);
	_serial->setBaudRate(QSerialPort::Baud19200);
	_serial->setDataBits(QSerialPort::Data8);
	_serial->setStopBits(QSerialPort::OneStop);
	_serial->setParity(QSerialPort::NoParity);
	_serial->setFlowControl(QSerialPort::NoFlowControl);
}


void ysplitter::_sendFiscal()
{
	_out_buffer.clear();
	_out_buffer.append(SPL_CMD_FISCAL);
	
	_append2Byte(_fisSum);
	_append2Byte(_fisItemID);
	_append4Byte(_fisID);

	_out_buffer.append(_calc_crc(&_out_buffer, _out_buffer.count(), 0));
	_send();

}

void ysplitter::_sendPool()
{

	_out_buffer.clear();
	_out_buffer.append(SPL_CMD_POOL);
	
	_append2Byte(_eq1);
	_append2Byte(_eq2);
	_append2Byte(_eq3);
	_append2Byte(_eq4);
	_append2Byte(_eq5);

	_out_buffer.append(_calc_crc(&_out_buffer, _out_buffer.count(), 0));
	_send();
}

void ysplitter::_append2Byte(quint16 value)
{
	translate __temp;
	__temp.in = value;
	_out_buffer.append(__temp.b[1]);
	_out_buffer.append(__temp.b[0]);
}

void ysplitter::_append4Byte(quint32 value)
{
	translate __temp;
	__temp.in = value;
	_out_buffer.append(__temp.b[3]);
	_out_buffer.append(__temp.b[2]);
	_out_buffer.append(__temp.b[1]);
	_out_buffer.append(__temp.b[0]);
}

quint16 ysplitter::_get2Byte(quint8 i)
{
	translate __temp;
	__temp.b[0] = (quint8)_in_buffer.at(i);
	__temp.b[1] = (quint8)_in_buffer.at(i - 1);
	__temp.b[2] = 0;
	__temp.b[3] = 0;

	return __temp.in;
}

quint32 ysplitter::_get4Byte(quint8 i)
{
	translate __temp;
	__temp.b[0] = (quint8)_in_buffer.at(i);
	__temp.b[1] = (quint8)_in_buffer.at(i - 1);
	__temp.b[2] = (quint8)_in_buffer.at(i - 2);
	__temp.b[3] = (quint8)_in_buffer.at(i - 3);

	return __temp.in;
}

void ysplitter::_gotSerialData()
{
	while (_serial->bytesAvailable() > 0) {
		_in_buffer.append(_serial->read(PACKET_SIZE));
	}

	
	if (_in_buffer.count() == PACKET_SIZE) {

		//quint8 __tmp = _calc_crc(&_in_buffer, PACKET_SIZE - (2 + 1), 2 + 1);

		quint8 __tmp = _calc_crc(&_in_buffer, PACKET_SIZE - 1, 2 + 1);

		if ((_in_buffer.at(0) == 0x0d) && (_in_buffer.at(1) == 0x0a))
		{
			if ((quint8)_in_buffer.at(29) == _calc_crc(&_in_buffer, PACKET_SIZE - 1, 2 + 1)) {
//			if ((quint8)_in_buffer.at(29) == (quint8)_in_buffer.at(29)) {
				currentState.eq			= _get2Byte(5);
				currentState.va			= _get2Byte(7);
				currentState.coin		= _get2Byte(9);
				currentState.jets		= _get2Byte(11);
				currentState.pulse		= _get2Byte(13);
				currentState.err_eq		= _get2Byte(15);
				currentState.err_va		= _get2Byte(17);
				currentState.err_coin	= _get2Byte(19);
				currentState.err_pulse	= _get2Byte(21);

				currentState.status_eq   = _in_buffer.at(22);
				currentState.status_va   = _in_buffer.at(23);
				currentState.status_coin = _in_buffer.at(24);

				currentState.fisID = _get4Byte(28);

				if (currentState.fisID == _fisID) _fiscaling = false;
				

				emit gotNewData();
				_pooling = true;
				_waitingRequest = false;
			}
			else {
				qDebug() << __tmp << "calc" ;
				qDebug() << (quint8)_in_buffer.at(29) << "crc";

				emit crcError();
			}
		}
	}
}

void ysplitter::_timerSingleShot()
{
	if (_running) {

		if (_waitingRequest) {
			timeouts++;
			emit timeout();
		}

		if (!_pooling) {
			start();
			return;
		}

		if (_fiscaling) {
			_sendFiscal();
		}
		else {
			_sendPool();
		}

		QTimer::singleShot(PERIOD, this, SLOT(_timerSingleShot()));
	}
}

void ysplitter::_closePort()
{
	if (_serial->isOpen()) {
		_serial->close();
	}
}

void ysplitter::_sendReset()
{
	_out_buffer.clear();
	_out_buffer.append(SPL_CMD_RESET);
	_out_buffer.append(_calc_crc(&_out_buffer, _out_buffer.count(), 0));
	_send();
}

void ysplitter::_send()
{
	_out_buffer.prepend((quint8)_out_buffer.count() - 1);
	_out_buffer.prepend(0xa);
	_out_buffer.prepend(0xd);
	_serial->write(_out_buffer, _out_buffer.count());
	_serial->waitForBytesWritten(200);
	_serial->flush();
	_in_buffer.clear();
	_waitingRequest = true;
}

quint8 ysplitter::_calc_crc(QByteArray *buffer, quint8 cnt, quint8 fromPOS)
{
	quint8 __crc = 0;
	for (int i = fromPOS; i < cnt; i++) {
		__crc += (quint8)buffer->at(i);
	}
	return (quint8)__crc;
}
