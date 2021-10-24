#pragma once
#include <iostream>
#include <string>
#include <boost/array.hpp>
#include <boost/bind/bind.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/signals2.hpp>
#include <boost/asio.hpp>
#include <boost/thread.hpp>
#include <queue>

using boost::asio::ip::udp;

class ClientCode
{
public:
	ClientCode(boost::signals2::signal<void()>* bufferEvent, boost::asio::io_context& io_context, std::mutex *m, std::queue<boost::array<unsigned char, 1>>* bufferQueue)
		: socket(io_context, udp::endpoint(udp::v4(), 23001))
	{
		bufferQueues = bufferQueue;
		mMutex = m;
		bufferPointer = bufferEvent;
		start_receive();
	}
	
	void PushQueue(const boost::array<unsigned char, 1> bufferToPush);

	boost::array<unsigned char, 1> getRecv_Buffer();

	const boost::array<unsigned char, 1> BufferFactory();

	void StartSend();

private:
	void handle_receive(const boost::system::error_code& error,
		std::size_t /*bytes_transferred*/);

void HandleSend(boost::shared_ptr<boost::array<unsigned char, 1>> /*message*/,
		const boost::system::error_code& /*error*/,
		std::size_t /*bytes_transferred*/);

	void start_receive();

	udp::socket socket;
	udp::endpoint recvEP;
	udp::endpoint sendEP;

	std::string output;
	boost::array<unsigned char, 1> recvBuffer;
	boost::signals2::signal<void()>* bufferPointer;
	std::mutex *mMutex;
	std::queue<boost::array<unsigned char, 1>>* bufferQueues;
};