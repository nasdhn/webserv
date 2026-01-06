#include "../../includes/Response.hpp"
#include <iostream>

int main() {
    Response res;
    res.setStatus(200);
    res.setBody("<html>\n<body>\n<h1>Test Webserv</h1>\n<p>Bravo Yann, la string est correcte !</p>\n</body>\n</html>");
    res.setHeader("Content-Type", "text/html");
    res.setHeader("Connection", "keep-alive");
    std::string finalResponse = res.toString();
    std::cout << "--- DEBUT DE LA REPONSE HTTP ---" << std::endl;
    std::cout << finalResponse << std::endl;
    std::cout << "--- FIN DE LA REPONSE HTTP ---" << std::endl;

    return 0;
}