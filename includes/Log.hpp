#ifndef LOG_HPP
#define LOG_HPP

#include "webserv.hpp"

class Log
{
private:
	bool saveLogError_; // default (save logs): true
	bool errorToFile_;	// default (write to std::cerr): false
	std::string errorFileName_;

	bool saveLogAccess_; // default (save logs): true
	bool accessToFile_;	 // default (write to std::cout): false
	std::string accessFileName_;

protected:
	void writeToFile(std::exception const &msg, std::string const &fileName) const;
	void writeToFile(std::string const &msg, std::string const &fileName) const;
	void writeToConsole(std::exception const &msg, bool error = false) const;
	void writeToConsole(std::string const &msg, bool error = false) const;

public:
	Log();
	~Log();

	void setLogError(bool toFile, bool saveLog = true);
	void setLogAccess(bool toFile, bool saveLog = true);
	bool getSaveLogError() const;
	bool getErrorToFile() const;
	bool getSaveLogAccess() const;
	bool getAccessToFile() const;
	std::string getTimeStamp() const;

	void printErrorMessage(std::exception const &msg) const;
	void printErrorMessage(std::string const &msg) const;

	void printAccessMessage(std::exception const &msg) const;
	void printAccessMessage(std::string const &msg) const;
};

#endif