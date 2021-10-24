#include "ClientCode.h"
#include <Windows.h>

std::mutex m;
std::queue<boost::array<unsigned char, 1>> bufferQueue;

void Execute(boost::asio::io_context* io)
{
	io->run();
}


int main()
{
	boost::signals2::signal<void()> recvSignal;

	recvSignal.connect([&] {
		std::lock_guard<std::mutex> lg(m);
		boost::array<unsigned char, 1> msg = bufferQueue.front();
		std::string data(bufferQueue.front().begin(), bufferQueue.front().end());
		std::cout << data;
		bufferQueue.pop();
		});

	int option;
	std::cout << "press 1 for server or 2 for client.\n";
	std::cin >> option;

	
	try
	{
		boost::asio::io_context io;
		ClientCode client(&recvSignal, io, &m, &bufferQueue);
		std::thread t(Execute, &io);
		t.detach();

		if (option == 1) {
			system("pause");
			t.~thread();
		}
		else {
			//send unlimited packets!!!
			int x = 0;
			/*while(true)
			{*/
				Sleep(1);
				x++;
				std::cout << "sent[" << x << "]\n" ;
				client.StartSend();
			/*}*/
			t.~thread();
		}
	}
	catch (std::exception& e)
	{
		std::cerr << e.what() << std::endl;
	}
}