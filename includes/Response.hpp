#ifndef RESPONSE_HPP
#define RESPONSE_HPP

#include "webserv.hpp"
#include "ConfigServer.hpp"
// #include "Client.hpp"
#include "DefaultPage.hpp"
#include "Autoindex.hpp"
#include "cgi.hpp"
#include "cgi2.hpp"
#include "base64.hpp"

class Client;
class ConfigServer;

class Response
{
private:
	std::string response_;
	std::string data_;
	std::string code_;
	static std::map<std::string, std::string> statusCodes_; // code => message
	static std::map<std::string, std::string> mimeTypes_;	 // extension => type

	std::string path_;
	std::string locationNew_;

	typedef std::pair<std::string, std::string> code;

	static std::map<std::string, std::string> initStatusCodes();
	static std::map<std::string, std::string> initMimeTypes();

protected:
	bool getPage(std::string const &path_file, std::string &str_file);

	void responseAddCode(std::string const &code);
	void responseAddDate();
	void responseAddLastModified(std::string const &file);
	void responseAddServer(std::string const &server = WEBSERV_NAME);
	void responseAddLocation(std::string const &location);
	void responseAddAllow(std::string const &methodsList);
	void responseAddContentType(std::string const &fileName);
	void responseAddContentLength(size_t length);
	void responseAddContentLanguage(std::string const &languages = "en");
	void responseAddContentLocation(std::string const &url);
	void responseAddRetryAfter(int seconds);
	void responseAddTransferEncoding(std::string const &encoding = "identity");
	void responseAddWWWAuthenticate(std::string const &auth);

	bool checkAllowes(Client const &client, ConfigLocation const &location);
	bool createUrl(ConfigServer const &server, ConfigLocation const &location, Client const &client);

	void returnErrorPage(Client const &client, std::string const &auth = "", std::string const &methodsList = "GET, HEAD");
	void returnPage(ConfigServer const &server, Client const &client);

	bool checkLocation(Client &client, ConfigLocation const &location);

public:
	Response();
	~Response();

	// void setGET(ConfigServer const &server, Client const &client);
	// void setHEAD(ConfigServer const &server, Client const &client);
	// void setPOST(ConfigServer const &server, Client const &client);
	// void setPUT(ConfigServer const &server, Client const &client);

	void setGET(Client &client);
	void setHEAD(Client &client);
	void setPOST(Client &client);
	void setPUT(Client &client);
	void setErrorCode(Client const &client, std::string const &code);

	void clearResponse();

	// ----- part zdawnsta -----
	std::string fullResponse_;
	// ----- end of part zdawnsta -----

	// ----- part zdawnsta -----
	// ----- end of part zdawnsta -----

	std::string getCode() const;
	std::string const &getResponse() const;
	std::string const &getData() const;
};

#endif