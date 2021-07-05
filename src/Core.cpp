#include "Core.hpp"

Core::Core(Config &config)
	: _servers(config.getServers()), _request_count(0), _maxSd(0)
{
}

Core::~Core()
{
}

bool Core::initServerSocket(size_t i)
{
	// Create a socket (IPv4, TCP)
	_sockfd[i] = socket(AF_INET, SOCK_STREAM, 0);

	if (_sockfd[i] == -1)
	{
		g_log.printErrorMessage("Can't create socket: " + std::string(strerror(errno)));
		return false;
	}
	// NO «address already in use»
	int yes = 1;
	setsockopt(_sockfd[i], SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int));
	fcntl(_sockfd[i], F_SETFL | O_NONBLOCK); // check after... < 0
	// Listen to address
	sockaddr_in sockaddr = _servers.at(i).getServAddr();
	if (bind(_sockfd[i], (struct sockaddr *)&sockaddr, sizeof(sockaddr)) != 0)
	{
		g_log.printErrorMessage("Can't bind address: " + _servers.at(i).getServerIp() + ":" + positiveNumberToString(_servers.at(i).getPort()) + " - " + std::string(strerror(errno)));
		return false;
	}

	// Start listening. Hold at most 10 connections in the queue
	if (listen(_sockfd[i], SOMAXCONN) != 0)
	{
		g_log.printErrorMessage("Can't configure listening port: " + std::string(strerror(errno)));
		return false;
	}
	return true;
}

void Core::startWebServer()
{
	for (size_t i = 0; i < _servers.size();)
	{
		if (!initServerSocket(i))
		{
			g_log.printErrorMessage("Socket init error: " + std::string(strerror(errno)));
			_servers.erase(_servers.begin() + i);
			continue;
		}
		g_log.printAccessMessage("Server was started: http://" + _servers.at(i).getServerIp() + ":" + positiveNumberToString(_servers.at(i).getPort()));
		i++;
	}
	size_t numberOfServers = _servers.size();
	if (numberOfServers < 1)
	{
		g_log.printErrorMessage("None socket is available: " + std::string(strerror(errno)));
		g_log.printErrorMessage("Server cannot be start.");
		return;
	}
	while (true)
		processingLoop(numberOfServers);
}

void Core::processingLoop(size_t numberOfServers)
{
	// int maxSd = 0;

	FD_ZERO(&readFds);
	FD_ZERO(&writeFds);
	for (size_t i = 0; i < numberOfServers; i++)
	{
		FD_SET(_sockfd[i], &readFds);
		_maxSd = std::max(_maxSd, _sockfd[i]);
	}

	std::list<Cl>::iterator it;
	for (it = _clients.begin(); it != _clients.end(); ++it)
	{

		if (it->client.getFihishReadingRequest())
		{
			if (std::difftime(std::time(NULL), it->client.getTimeLastConnow()) > TIMEOUT_SEC)
			{
				g_log.printErrorMessage("Close client " + positiveNumberToString(it->client.getSocketClient()) + " because of timeout > " + positiveNumberToString(TIMEOUT_SEC) + " sec");

				it = closeClient(it);
				continue;
			}

			FD_SET(it->client.getSocketClient(), &writeFds);
		}
		else
		{
			if (std::difftime(std::time(NULL), it->client.getTimeLastConnow()) > TIMEOUT_SEC)
			{
				g_log.printErrorMessage("Closing client " + positiveNumberToString(it->client.getSocketClient()) + " because of timeout > " + positiveNumberToString(TIMEOUT_SEC) + " sec. Creating error page.");

				it->client.setFihishReadingRequest(true);
				it->resp.setErrorCode(it->client, "408");
				it->resp.fullResponse_ = it->resp.getResponse() + it->resp.getData();
				it->client.setTimeLastConn();
				++it;
				continue;
			}

			FD_SET(it->client.getSocketClient(), &readFds);
		}
		_maxSd = std::max(_maxSd, it->client.getSocketClient());
	}
	// std::cout << "MaxSd = " << maxSd_ << std::endl;

	timeout_.tv_sec = 0;
	timeout_.tv_usec = 10;

	int sel = select(_maxSd + 1, &readFds, &writeFds, NULL, &timeout_);

	if (sel < 0)
		g_log.printErrorMessage("Select error: " + std::string(strerror(errno)));
	for (size_t i = 0; i < numberOfServers; i++)
	{
		if (FD_ISSET(_sockfd[i], &readFds))
			acceptConnection(_sockfd[i], _servers.at(i));
	}
	receiveRequests();
	sendResponses();
}

void Core::acceptConnection(int sockFd, ConfigServer server)
{

	sockaddr_in saClient;
	socklen_t clientLen = sizeof(saClient);
	int sockClient;

	sockClient = accept(sockFd, reinterpret_cast<sockaddr *>(&saClient), &clientLen);
	if (sockClient < 0)
	{
		g_log.printErrorMessage("Accept error: " + std::string(strerror(errno)));
		return;
	}
	fcntl(sockClient, F_SETFL | O_NONBLOCK);

#if DEBUG_SERV > 1
	g_log.printAccessMessage("Accept connection: " + server.getServerIpPort() + " sockServer: " + positiveNumberToString(sockFd) + ",\tClient: " + positiveNumberToString(sockClient));
#endif

	struct Cl cl;
	cl.client = Client(sockClient, sockFd, server);
	_clients.push_back(cl);
}

int Core::receiveData(int sockClient, std::string &str)
{
	char buffer[RECV_BUFFER] = {0};

	#if (MY_OS == 1)
		int recvBytes = recv(sockClient, buffer, RECV_BUFFER - 1, MSG_DONTWAIT);
	#else
		int recvBytes = recv(sockClient, buffer, RECV_BUFFER - 1, 0);
	#endif
	
	if (recvBytes > 0)
		str.append(buffer, recvBytes);
	return (recvBytes);
}

bool Core::accumHeaders(std::list<Cl>::iterator &it, std::string &req)
{
	if (!it->client.getHeadersReq())
	{
		it->client.addToRequest(req);
		size_t pos = (*it).client.getRequest().find("\r\n\r\n");
		if (pos == std::string::npos)
			return false;

		req = it->client.getRequest().substr(pos + 4);					   //Cat headers from req
		it->client.setRequest(it->client.getRequest().substr(0, pos + 4)); //Only headers in request
		it->client.setHeadersReq(true);
		// Read all Headers

		try
		{
#if DEBUG_SERV == 1
			std::cout << UNDERLINE_W << "Full request HEADERS before parsing:\n"
					  << BLUE_C << it->client.getRequest()
					  << RESET_C << std::endl;
#endif

			it->client.parseRequest(it->client.getRequest());
			it->client.setRequest("");

#if DEBUG_SERV == 1
			std::cout << UNDERLINE_W << "[" << _request_count << "] Parsed request HEADERS:\n"
					  << YELLOW_C << it->client << RESET_C << std::endl;
#endif
		}
		catch (const std::exception &e)
		{
			g_log.printErrorMessage(e);
			it->resp.setErrorCode(it->client, "400");
			it->resp.fullResponse_ = it->resp.getResponse() + it->resp.getData();
		}
	}

	return true;
}

bool Core::accumBody(std::list<Cl>::iterator &it)
{
	if (it->client.getRequest().length() < it->client.getHeaderContentLength())
		return false;
	else // check > length
	{
#if DEBUG_SERV == 1
		std::cout << UNDERLINE_W << "Full request BODY before parsing:\n"
				  << BLUE_C << it->client.getRequest()
				  << RESET_C << std::endl;
#endif

		it->client.parseBody(it->client.getRequest().substr(0, it->client.getHeaderContentLength()));

#if DEBUG_SERV == 1
		std::cout << UNDERLINE_W << "[" << _request_count << "] Parsed FULL request:\n"
				  << YELLOW_C << it->client << RESET_C << std::endl;
#endif

		return true;
	}
	return false;
}

bool Core::accumBodyChunked(std::list<Cl>::iterator &it)
{
	size_t pos = it->client.getRequest().find("\r\n");

	while (pos != std::string::npos)
	{
		std::string block = it->client.getRequest().substr(0, pos);

#if DEBUG_SERV == 1
		std::cout << UNDERLINE_W << "New chunked 'block' before parsing:\n"
				  << BLUE_C << block << "\nlen: " << block.length()
				  << RESET_C << std::endl;
#endif

		if (it->client.getChunk() == 0)
		{
			it->client.setChunk(block);

			if (it->client.getChunk() == 0)
			{
				if (it->client.getRequest().find("\r\n\r\n") == std::string::npos)
					return false;

#if DEBUG_SERV == 1
				std::cout << UNDERLINE_W << "[" << _request_count << "] Parsed FULL request:\n"
						  << YELLOW_C << it->client << RESET_C << std::endl;
#endif

				return true;
			}

			it->client.setRequest(it->client.getRequest().substr(pos + 2));
			pos = 0;

#if DEBUG_SERV == 1
			std::cout << UNDERLINE_W << "New chunked Request before parsing:\n"
					  << BLUE_C << it->client.getRequest()
					  << RESET_C << std::endl;
#endif
		}
		else
		{
			if (it->client.getRequest().length() >= it->client.getChunk() + 2)
			{
#if DEBUG_SERV == 1
				std::cout << UNDERLINE_W << "Part or chunked BODY request before parsing:\n"
						  << BLUE_C << it->client.getRequest().substr(0, it->client.getChunk())
						  << "\nlen: " << it->client.getRequest().substr(0, it->client.getChunk()).length()
						  << RESET_C << std::endl;
#endif

				it->client.parseBody(it->client.getRequest().substr(0, it->client.getChunk()));
				it->client.setRequest(it->client.getRequest().substr(it->client.getChunk() + 2));
				pos = 0;
				it->client.setChunk("0");

#if DEBUG_SERV == 1
				std::cout << UNDERLINE_W << "New chunked Request before parsing:\n"
						  << BLUE_C << it->client.getRequest()
						  << RESET_C << std::endl;
#endif
			}
		}

		if (pos > 0)
			pos += 2;
		pos = it->client.getRequest().find("\r\n", pos);
	}

	return false;
}

void Core::receiveRequests()
{
	std::list<Cl>::iterator it = _clients.begin();
	while (it != _clients.end())
	{
		std::string req;
		//check clients
		if (FD_ISSET(it->client.getSocketClient(), &readFds))
		{
			int ret = receiveData(it->client.getSocketClient(), req);
			if (ret > 0) //пришли данные
			{
#if DEBUG_SERV == 1
				std::cout << PURPLE_C;
				g_log.printAccessMessage("Client: " + positiveNumberToString((*it).client.getSocketClient()) + ". Part of request: \n" + req);
				std::cout << RESET_C;
#endif

				it->client.setTimeLastConn();

				if (!accumHeaders(it, req))
				{
					++it;
					continue;
				}
				it->client.addToRequest(req);

				if (it->resp.getCode().length() > 0)
					it->client.setFihishReadingRequest(true);
				else if (it->client.getHeaderTransferEncoding() == "chunked")
					it->client.setFihishReadingRequest(accumBodyChunked(it));
				else if (it->client.getHeaderContentLength() > 0)
					it->client.setFihishReadingRequest(accumBody(it));
				else // no body
					it->client.setFihishReadingRequest(true);

				if (it->client.getFihishReadingRequest())
				{
#if DEBUG_SERV > 1
					std::cout << UNDERLINE_W << "[" << _request_count << "] Parsed FULL request of client:"
							  << RESET_C << " " << it->client.getSocketClient() << "\n"
							  << YELLOW_C << it->client << RESET_C << std::endl;
#endif

					createResponse(it);
				}
				++it;
			} // ошибка (TCP return: RST(reset connection))
			else if (ret < 0)
			{
				g_log.printErrorMessage("Recieve error: " + it->client.getServer().getServerIp() + ":" + positiveNumberToString(it->client.getServer().getPort()) + " - " + std::string(strerror(errno)));
				it = closeClient(it);
			}
			else // нет данных (TCP return: FIN(sender is finished sending data))
			{
				g_log.printAccessMessage("Client " + positiveNumberToString(it->client.getSocketClient()) + " sent FIN.");
				if (!it->client.getFihishReadingRequest())
					it = closeClient(it);
				else
					++it;
			}
		}
		else
			++it;
	}
}

void Core::createResponse(std::list<Cl>::iterator &it)
{
#if DEBUG_SERV > 1
	g_log.printAccessMessage("Create response to Client: " + positiveNumberToString(it->client.getSocketClient()) + ", server: " + it->client.getServer().getServerIpPort());
#endif

	++_request_count;

#if DEBUG_SERV > 1

	std::cout << UNDERLINE_W << "Created Client Request real body size:"
			  << YELLOW_C << " " << it->client.getBody().length()
			  << RESET_C << std::endl;
#endif

	if (it->resp.getCode().length() == 0)
	{
		try
		{
			if (it->client.getMethod() == "HEAD")
				it->resp.setHEAD(it->client);
			else if (it->client.getMethod() == "GET")
				it->resp.setGET(it->client);
			else if (it->client.getMethod() == "POST")
				it->resp.setPOST(it->client);
			else if (it->client.getMethod() == "PUT")
				it->resp.setPUT(it->client);
			else
				it->resp.setErrorCode(it->client, "501");

#if DEBUG_SERV > 1

			std::cout << UNDERLINE_W << "Webserv response to client:"
					  << RESET_C << " " << it->client.getSocketClient() << "\n"
					  << YELLOW_C << it->resp.getResponse()
					  << GREEN_C << "Count 'Body' size: " << it->resp.getData().length()
					  << RESET_C << std::endl;
#endif
		}
		catch (const Client::ClientException &e)
		{
			g_log.printErrorMessage(e);
			it->resp.setErrorCode(it->client, "400");
		}
	}
	it->resp.fullResponse_ = it->resp.getResponse() + it->resp.getData();
}

void Core::sendResponses()
{
	std::list<Cl>::iterator it = _clients.begin();
	while (it != _clients.end())
	{
		int sended;
		if (FD_ISSET(it->client.getSocketClient(), &writeFds) && it->resp.fullResponse_.length() > 0) // TODO resp.isReady
		{
			size_t piece = std::min((size_t)SEND_BUFFER, it->resp.fullResponse_.size());

#if DEBUG_SERV > 1

			std::cout << UNDERLINE_W << "Send to client " << it->client.getSocketClient()
					  << " part of Response size:"
					  << YELLOW_C << " " << piece
					  << RESET_C << std::endl;
#endif
			if (it->resp.fullResponse_.length() > 0)
			{
				#if (MY_OS == 1)
					sended = send(it->client.getSocketClient(), it->resp.fullResponse_.data(), piece, MSG_DONTWAIT);
				#else
					sended = send(it->client.getSocketClient(), it->resp.fullResponse_.data(), piece, 0);
				#endif

				if (sended < 0)
				{
					g_log.printErrorMessage("sendResponses error. sended : " + positiveNumberToString(sended) + ", " + std::string(strerror(errno)));
					it = closeClient(it);
					continue;
				}
				if (sended == 0)
				{
					g_log.printErrorMessage("sendResponses. sended : " + positiveNumberToString(sended));
					if (it->resp.fullResponse_.length() != 0)
					{
						it = closeClient(it);
						continue;
					}
				}
				else
					it->resp.fullResponse_ = it->resp.fullResponse_.substr(sended);
				it->client.setTimeLastConn();
			}
			if (it->resp.fullResponse_.length() == 0)
			{
				if (it->client.getMethod() == "POST" || it->client.getMethod() == "PUT")
				{
					it->client.clearClient();
					it->resp.clearResponse();
					++it;
				}
				else
					it = closeClient(it);

				continue;
			}
		}
		++it;
	}
}

std::list<Core::Cl>::iterator Core::closeClient(std::list<Cl>::iterator it)
{
#if DEBUG_SERV > 1
	g_log.printAccessMessage("Client " + positiveNumberToString(it->client.getSocketClient()) + " disconnected");
#endif

	close(it->client.getSocketClient());

	return _clients.erase(it);
}

// bool Core::file_transfer(std::string path_file, std::string &str_file)
// {
// 	char buffer[BUFFER_SIZE] = {0};
// 	int fd, ret;
// 	if ((fd = open(path_file.c_str(), O_RDONLY)) == -1)
// 	{
// 		g_log.printErrorMessage("File did not open! Path - " + path_file + " : " + std::string(strerror(errno)));
// 		return false;
// 	}

// 	str_file = "";
// 	while ((ret = read(fd, buffer, BUFFER_SIZE - 1)) > 0)
// 		str_file.append(buffer, ret);

// 	if (ret == -1)
// 	{
// 		g_log.printErrorMessage("Error in file reading: " + std::string(strerror(errno)));
// 		return false;
// 	}
// 	close(fd);
// 	return true;
// }
