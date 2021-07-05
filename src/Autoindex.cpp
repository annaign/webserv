#include "Autoindex.hpp"

Autoindex::Autoindex()
{
}

Autoindex::~Autoindex()
{
}

std::string Autoindex::getPage(std::string const &path, std::string const &sitePath)
{
	std::string page = "<!DOCTYPE html>\n";
	page.append("<html>\n");
	page.append("<head><title>Index of " + sitePath + "</title></head>\n");
	page.append("<body bgcolor=\"white\">\n");
	page.append("<h1>Index of " + sitePath + "</h1><hr><pre>\n");

	page.append("<table>\n");
	page.append("<thead>\n");
	page.append("<tr>\n");
	page.append("<th>Name</th>\n");
	page.append("<th>Size, byte</th>\n");
	page.append("<th>Last modification</th>\n");
	page.append("</tr>\n");
	page.append("</thead>\n");
	page.append("<tbody>\n");

	if (!this->addLinks(path, sitePath, page))
	{
		page = "";
		return page;
	}

	page.append("</tbody>\n");
	page.append("</table>\n");

	page.append("</pre><hr></body>\n");
	page.append("</html>\n");

	return page;
}

bool Autoindex::addLinks(std::string const &path, std::string sitePath, std::string &page)
{

	page.append("<tr>\n");
	page.append("<td><a href=\"../\">../</a></td>\n");
	page.append("<td></td>\n");
	page.append("<td></td>\n");
	page.append("</tr>\n");

	DIR *dir = opendir(path.c_str());

	if (dir == NULL)
		return false;

	dirent *line;
	std::string pathFull = path;
	if (pathFull[pathFull.length() - 1] != '/')
		pathFull += "/";

	if (sitePath[sitePath.length() - 1] != '/')
		sitePath += "/";

	while ((line = readdir(dir)) != NULL)
	{
		std::string name = std::string(line->d_name);

		if (name != "." && name != "..")
		{
			page.append("<tr>\n");

			struct stat buf;
			if (stat((pathFull + name).c_str(), &buf) == -1)
			{
				page.append("<td></td>\n");
				page.append("<td></td>\n");
			}
			else
			{
				if (line->d_type == DT_REG)
				{
					page.append("<td><a href=\"" + sitePath + name + "\">" + name + "</a></td>\n"); // file
					page.append("<td>" + positiveNumberToString(buf.st_size) + "</td>\n");
				}
				else
				{
					page.append("<td><a href=\"" + sitePath + name + "/\">" + name + "</a></td>\n"); //path
					page.append("<td></td>\n");
				}
				page.append("<td>" + std::string(ctime(&buf.st_mtime)) + "</td>\n");
			}
			page.append("</tr>\n");
		}
	}

	closedir(dir);

	return true;
}
