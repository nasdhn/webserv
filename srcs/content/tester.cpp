#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <sstream>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <cstdlib> // pour exit

// --- 1. MOCK DE TES CLASSES (Pour que ça compile tout seul) ---
// (Dans ton vrai projet, c'est tes includes .hpp)

struct FakeRequest {
    std::string method;
    std::string path;
    std::string query;
    std::string body;
};

class Response {
public:
    int _fd;
    pid_t _pid;
    bool _is_fd;
    std::string _header_str;

    Response() : _fd(-1), _pid(-1), _is_fd(false) {}
    
    // Destructeur qui fait le ménage (Anti-Zombie)
    ~Response() {
        if (_fd != -1) close(_fd);
        if (_pid != -1) {
            int status;
            waitpid(_pid, &status, 0);
            std::cout << "\n[DEBUG] Processus CGI nettoyé (PID " << _pid << ")" << std::endl;
        }
    }

    void setStatus(int code) { std::cout << "[DEBUG] Status set to: " << code << std::endl; }
    void setBody(std::string b) { _is_fd = false; std::cout << "[DEBUG] Body set (string)" << std::endl; }
    void setBodyfd(int fd) { _fd = fd; _is_fd = true; }
    void setpid(pid_t pid) { _pid = pid; }
    void setHeader(std::string k, std::string v) { _header_str += k + ": " + v + "\r\n"; }
    
    int get_body_fd() { return _fd; }
};

// --- 2. TES FONCTIONS (J'ai copié ton code validé) ---

std::string getExtention(std::string path) {
    size_t pos = path.find_last_of(".");
    if (pos == std::string::npos) return "";
    return path.substr(pos);
}

int execCGI(FakeRequest req, pid_t &pid) {
    std::string path = req.path;
    int fd[2];
    if (pipe(fd) == -1) return (-1);
    
    pid = fork();
    if (pid < 0) return (-1);
    
    if (pid == 0) { // ENFANT
        close(fd[0]);
        dup2(fd[1], STDOUT_FILENO);
        close(fd[1]);

        std::string ext = getExtention(req.path);
        std::string bin;
        if (ext == ".py") bin = "/usr/bin/python3"; // Vérifie ce chemin !
        else if (ext == ".php") bin = "/usr/bin/php-cgi";
        else exit(1);

        char *arg[3];
        arg[0] = (char *)bin.c_str();
        arg[1] = (char *)req.path.c_str();
        arg[2] = NULL;

        std::vector<std::string> envs;
        envs.push_back("REQUEST_METHOD=" + req.method);
        envs.push_back("QUERY_STRING=" + req.query);
        envs.push_back("PATH_INFO=" + req.path);
        
        char **envp = new char*[envs.size() + 1];
        for (size_t i = 0; i < envs.size(); i++) envp[i] = (char*)envs[i].c_str();
        envp[envs.size()] = NULL;

        execve(bin.c_str(), arg, envp);
        perror("execve FAILED");
        exit(1);
    } else { // PARENT
        close(fd[1]);
        return (fd[0]);
    }
}

Response BuildResponse(FakeRequest req) {
    Response res;
    struct stat info;

    // Simulation simple pour le test (pas de delete ici)
    if (stat(req.path.c_str(), &info) != 0) {
        std::cerr << "Erreur: Fichier introuvable: " << req.path << std::endl;
        return res;
    }

    std::string ext = getExtention(req.path);
    if (ext == ".py" || ext == ".php") {
        pid_t pid;
        int fd = execCGI(req, pid);
        res.setStatus(200);
        res.setBodyfd(fd);
        res.setpid(pid);
        return res;
    }

    // Fichier normal
    int fd = open(req.path.c_str(), O_RDONLY);
    res.setStatus(200);
    res.setBodyfd(fd);
    return res;
}

// --- 3. LE MAIN DE TEST (SIMULATION NASIM) ---

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