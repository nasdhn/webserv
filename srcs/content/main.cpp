#include "../../includes/Response.hpp"
#include <string>
#include <unistd.h>
#include <map>
#include <vector>
#include <iostream>
#include <sstream>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

Response BuildResponse(FakeRequest req);

int main() {
    std::cout << "=== TEST 1 : FICHIER STATIC ===" << std::endl;
    // On crée un faux fichier html
    system("echo '<h1>Hello World</h1>' > index.html");
    
    FakeRequest req1;
    req1.method = "GET";
    req1.path = "index.html";
    
    // On appelle ta fonction
    {
        Response res1 = BuildResponse(req1);
        
        // Simulation Nasim : Lecture du FD
        char buffer[1024];
        int ret;
        std::cout << "--- Contenu recu par Nasim ---" << std::endl;
        while ((ret = read(res1.get_body_fd(), buffer, 1023)) > 0) {
            buffer[ret] = 0;
            std::cout << buffer;
        }
        std::cout << "\n------------------------------" << std::endl;
    } // Ici le destructeur de res1 est appelé (ferme le fd)

    std::cout << "\n\n=== TEST 2 : CGI PYTHON ===" << std::endl;
    
    FakeRequest req2;
    req2.method = "GET";
    req2.path = "script.py"; // Ton script python
    req2.query = "user=yann";

    {
        Response res2 = BuildResponse(req2);
        
        // Simulation Nasim : Lecture du Pipe
        char buffer[1024];
        int ret;
        std::cout << "--- Contenu recu par Nasim (Sortie du Script) ---" << std::endl;
        while ((ret = read(res2.get_body_fd(), buffer, 1023)) > 0) {
            buffer[ret] = 0;
            std::cout << buffer;
        }
        std::cout << "\n-------------------------------------------------" << std::endl;
    } // Ici le destructeur appelle waitpid() -> Pas de Zombie !

    // Nettoyage
    system("rm index.html");
    return 0;
}