#include "ClientCode.h"


void ClientCode::start_receive()
{
	socket.async_receive_from(
		boost::asio::buffer(recvBuffer), recvEP,
		boost::bind(&ClientCode::handle_receive, this,
			boost::asio::placeholders::error,
			boost::asio::placeholders::bytes_transferred));
}

const boost::array<unsigned char, 1> ClientCode::BufferFactory()
{
	const boost::array<unsigned char, 1> immuBuffer = recvBuffer;
	return immuBuffer;
}

void ClientCode::PushQueue(const boost::array<unsigned char, 1> bufferToPush)
{
	std::lock_guard<std::mutex> lg(*mMutex);
	bufferQueues->push(bufferToPush);
}

void ClientCode::handle_receive(const boost::system::error_code& error,
	std::size_t /*bytes_transferred*/)
{
	if (!error)
	{
		PushQueue(BufferFactory());
		(bufferPointer->operator())();
		start_receive();
	}
}

void ClientCode::HandleSend(boost::shared_ptr<boost::array<unsigned char, 1>> /*message*/,
	const boost::system::error_code& /*error*/,
	std::size_t /*bytes_transferred*/)
{
}

void ClientCode::StartSend()
{
	sendEP.address(boost::asio::ip::address::from_string("127.0.0.1"));
	sendEP.port(23000);

	boost::shared_ptr<boost::array<unsigned char, 1>> msg(new boost::array<unsigned char, 1>);
	msg.get()->elems[0] = 254;

	socket.async_send_to(boost::asio::buffer(msg.get(), 1), sendEP,
		boost::bind(&ClientCode::HandleSend, this, msg,
			boost::asio::placeholders::error,
			boost::asio::placeholders::bytes_transferred));
}

boost::array<unsigned char, 1> ClientCode::getRecv_Buffer()
{
	return recvBuffer;
}