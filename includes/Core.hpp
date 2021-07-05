#ifndef CORE_HPP
#define CORE_HPP

#include "webserv.hpp"
#include "Config.hpp"
#include "Client.hpp"
#include "Response.hpp"

class Config;

class Core
{
private:
	struct Cl
	{
		Client client;
		Response resp;
	};

	int _sockfd[10]; //TODO
	std::vector<ConfigServer> _servers;
	std::list<Cl> _clients;
	size_t _request_count;

	int _maxSd;
	fd_set readFds;
	fd_set writeFds;

	timeval timeout_;

public:
	Core(Config &config);
	~Core();

	void startWebServer();

private:
	bool initServerSocket(size_t i); //init all servers from config
	void processingLoop(size_t numberOfServers);
	void acceptConnection(int sockFd, ConfigServer server);

	int receiveData(int sockClient, std::string &str);
	bool accumHeaders(std::list<Cl>::iterator &it, std::string &req);
	bool accumBody(std::list<Cl>::iterator &it);
	bool accumBodyChunked(std::list<Cl>::iterator &it);
	void receiveRequests();

	void createResponse(std::list<Cl>::iterator &it);
	void sendResponses();
	std::list<Cl>::iterator closeClient(std::list<Cl>::iterator it);

	// bool file_transfer(std::string, std::string &);
};

#endif
