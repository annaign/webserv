#ifndef CGI_HPP
#define CGI_HPP

#include "webserv.hpp"
#include "Client.hpp"
#include "Config.hpp"

class Client;

class CGI
{
private:
	char *args_[4];
	char **env_;
	std::string *cgi_input_file_ptr_;
	std::string *cgi_output_file_ptr_;

	std::map<std::string, std::string> envMap_;
	std::string envir_;

	std::string headers_;
	std::string body_;
	std::string scrypt_;

	Client client_;
	ConfigLocation location_;

	// void initArgs();
	// void initEnv();

public:
	CGI(Client const &client, ConfigLocation location);
	~CGI();


	void run();
	std::string const &getHeaders() const;
	std::string const &getBody() const;
};

// class CGIException : public std::exception
// {
// private:
//     std::string _msg;
//
// public:
//     CGIException(std::string message = "Unable to execute CGI.")
//         : _msg(message){};
//     ~CGIException() throw(){};
//     const char *what() const throw()
//     {
//         return (_msg.c_str());
//     };
// };

// std::string uIntegerToString(size_t n)
// {
// 	std::ostringstream convert;

// 	convert << n;
// 	return (convert.str());
// }

// class ParsingException : public std::exception
// {
// 	private:
// 		std::string _msg;
// 	public:
// 		ParsingException(int line = 0, std::string msg = "Unable to parse the given config file.")
// 		: _msg("Line: " + uIntegerToString(line + 1) + ": " + msg)
// 		{};
// 		~ParsingException() throw()
// 		{};
// 		const char *what () const throw ()
// 		{
// 			return (_msg.c_str());
// 		};
// };

#endif