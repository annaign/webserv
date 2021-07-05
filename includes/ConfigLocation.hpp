#ifndef CONFIGLOCATION_HPP
#define CONFIGLOCATION_HPP

#include "webserv.hpp"

class ConfigLocation
{
private:
	std::string prefix_;
	std::map<std::string, bool> methods_;
	std::string methodsList_;
	std::string root_;
	std::string alias_;
	std::string index_;
	bool autoindex_;
	std::string authBasic_;
	std::string authBasicUserFile_;
	std::string extension_;
	std::string scrypt_;
	size_t clientBodyBufferSize_;
	std::map<std::string, bool> fields_;

public:
	ConfigLocation();
	ConfigLocation(ConfigLocation const &src);
	~ConfigLocation();

	ConfigLocation &operator=(ConfigLocation const &rhs);

	bool setPrefix(std::string const &prefix);
	bool setMethods(std::vector<std::string> const &methods);
	bool setRoot(std::string const &root);
	bool setAlias(std::string const &alias);
	bool setIndex(std::string const &index);
	bool setAutoindex(std::string const &autoindex);
	bool setAuthBasic(std::string const &file);
	bool setAuthBasicUserFile(std::string const &file);
	bool setExtension(std::string const &extension);
	bool setScrypt(std::string const &scrypt);
	bool setClientBodyBufferSize(std::string const &clientBodyBufferSize);

	std::string const &getPrefix() const;
	std::map<std::string, bool> const &getMethods() const;
	std::string const &getMethodsList() const;
	std::string const &getRoot() const;
	std::string const &getAlias() const;
	std::string const &getIndex() const;
	bool getAutoindex() const;
	std::string getAuthBasic() const;
	std::vector<std::string> getAuthBasicUserFile() const;
	std::string const &getExtension() const;
	std::string const &getScrypt() const;
	size_t getClientBodyBufferSize() const;
	std::map<std::string, bool> const &getFields() const;
};

#endif
