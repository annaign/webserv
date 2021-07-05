#ifndef CLIENT_HPP
#define CLIENT_HPP

#include "webserv.hpp"
#include "Config.hpp"

class ConfigServer;

class Client
{
private:
	std::string method_;
	std::string url_;
	std::string httpVersion_;
	std::map<std::string, std::string> headers_; // header -> content
	std::string body_;
	std::string queryString_;

	// Parsed field: url_
	std::string serverName_; // template: "localhost" or "example.com"
	std::string path_;		 // template: "/" or "/path/" or "/path". If empty host identifier: ""
	std::string fileName_;	 // template: "filename.ext" or "filename"
							 // queryString_ - see above

	// ----- part zdawnsta -----
	int sockClient_;
	int sockServer_;
	ConfigServer server_;
	size_t chunk_;
	bool headersReq_; // are headers full? (\r\n\r\n)
	std::string request_;
	// ----- end of part zdawnsta -----

	bool fihishReadingRequest_;
	std::time_t timeLastConn_;

	typedef std::map<std::string, std::string> headersMap;

public:
	Client();
	Client(int sockClient, int sockFd, ConfigServer const &server);
	~Client();

	void setPath(std::string const &path);
	void setFileName(std::string const &fileName);
	void setBody(std::string const &body);
	bool parseBody(std::string const &body);

	std::string const &getMethod() const;
	std::string const &getUrl() const;
	std::string const &getHttpVersion() const;
	std::map<std::string, std::string> const &getHeaders() const;
	std::string const &getBody() const;
	std::string const &getQueryString() const;

	std::string const &getServerName() const;
	std::string const &getPath() const;
	std::string const &getFileName() const;

	std::string getHeaderAcceptCharsets() const;
	std::string getHeaderAcceptLanguage() const;
	std::map<std::string, std::string> getHeaderAuthorization() const;
	std::string getHeaderHost() const;
	std::string getHeaderReferer() const;
	std::string getHeaderUserAgent() const;
	std::string getHeaderContentLanguage() const;
	std::string getHeaderContentType() const;
	size_t getHeaderContentLength() const;
	std::string getHeaderDate() const;
	std::string getHeaderTransferEncoding() const;

	void clearClient();

	// ----- part zdawnsta -----
	void setChunk(size_t num10);
	void setChunk(std::string const &num16);
	void setHeadersReq(bool result);
	void setRequest(std::string const &request);
	size_t getChunk() const;
	bool getHeadersReq() const;
	std::string const &getRequest() const;
	int getSocketClient() const;
	ConfigServer const &getServer() const;

	void addToRequest(std::string const &chunk);
	// ----- end of part zdawnsta -----

	void setFihishReadingRequest(bool isFihised);
	bool getFihishReadingRequest() const;

	void setTimeLastConn();
	std::time_t getTimeLastConnow() const;

	class ClientException : public std::exception
	{
	private:
		std::string msg_;

	public:
		ClientException() throw() {}
		ClientException(std::string const &msg) throw() : msg_(msg) {}
		virtual ~ClientException() throw() {}

		virtual const char *what() const throw() { return this->msg_.c_str(); }
	};

	void parseRequest(std::string const &request) throw(Client::ClientException);

protected:
	bool parseRequestLine(std::string const &request);
	bool parseHeaders(std::string const &headers);

	void parseUrl(std::string url);
};

std::ostream &operator<<(std::ostream &os, Client const &client);

#endif