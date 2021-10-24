#include "ServerCode.h"

void ServerCode::start_receive()
{
	socket.async_receive_from(
		boost::asio::buffer(recvBuffer), recvEP,
		boost::bind(&ServerCode::handle_receive, this,
			boost::asio::placeholders::error,
			boost::asio::placeholders::bytes_transferred));
}

const boost::array<unsigned char, 1> ServerCode::BufferFactory()
{
	const boost::array<unsigned char, 1> immuBuffer = recvBuffer;
	return immuBuffer;
}

void ServerCode::PushQueue(const boost::array<unsigned char, 1> bufferToPush)
{
	std::lock_guard<std::mutex> lg(*mMutex);
	bufferQueues->push(bufferToPush);
}

void ServerCode::handle_receive(const boost::system::error_code& error,
	std::size_t /*bytes_transferred*/)
{
	if (!error)
	{
		PushQueue(BufferFactory());
		(bufferPointer->operator())();
		start_receive();
	}
}

void ServerCode::HandleSend(boost::shared_ptr<boost::array<unsigned char, 1>> /*message*/,
	const boost::system::error_code& /*error*/,
	std::size_t /*bytes_transferred*/)
{
}

void ServerCode::StartSend()
{
	sendEP.address(boost::asio::ip::address::from_string("127.0.0.1"));
	sendEP.port(23001);

	boost::shared_ptr<boost::array<unsigned char, 1>> msg(new boost::array<unsigned char, 1>);
	msg.get()->elems[0] = 254;

	socket.async_send_to(boost::asio::buffer(msg.get(), 1), sendEP,
		boost::bind(&ServerCode::HandleSend, this, msg,
			boost::asio::placeholders::error,
			boost::asio::placeholders::bytes_transferred));
}

boost::array<unsigned char, 1> ServerCode::getRecv_Buffer()
{
	return recvBuffer;
}