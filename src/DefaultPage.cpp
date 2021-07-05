#include "DefaultPage.hpp"

DefaultPage::DefaultPage()
{
}

DefaultPage::~DefaultPage()
{
}

std::string DefaultPage::getPage(std::string error, std::string const &text, std::string const &servVersion)
{
	std::string page = "<!DOCTYPE html>\n";
	page.append("<html lang=\"en\">\n");
	page.append("<head>\n");
	page.append("<meta charset=\"UTF-8\">\n");
	page.append("<meta http-equiv=\"X-UA-Compatible\" content=\"IE=edge\">\n");
	page.append("<meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">\n");
	page.append("<title>" + error + " " + text + "</title>\n");
	page.append("</head>\n");
	page.append("<body bgcolor=\"white\">\n");
	page.append("<center>\n");
	page.append("<h1>" + error + " " + text + "</h1>\n");
	page.append("</center>\n");
	page.append("<hr>\n");
	page.append("<center>" + servVersion + "</center>\n");
	page.append("</body>\n");
	page.append("</html>\n");

	return page;
}
