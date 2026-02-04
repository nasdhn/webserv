# webserv

This project has been created as part of the 42 curriculum by Cyril Goldenschue (cgoldens), Yann Oberson (yaoberso) and Nasim Dahman (nadahman).

Description:

Webserv is a fully functional HTTP/1.1 server written in C++98. The primary goal of this project is to understand the internal mechanisms of the HTTP protocol, network programming using sockets, and I/O multiplexing.
Unlike a traditional blocking server, Webserv uses poll() to handle multiple client connections simultaneously without blocking the execution flow. It is built to be robust, compliant with strictly defined HTTP standards, and capable of handling static content as well as dynamic content via CGI.


Instructions:

Compilation:
To compile the project, simply run make at the root of the repository:
make

Execution:
Run the server by providing a configuration file. If no file is provided, the server may default to a standard configuration or exit depending on implementation.
./webserv [configuration_file.conf]

Example:
./webserv l.conf

Once the server is running (default port is usually 8080), you can access it via your web browser: http://localhost:8080


Features
This server implements the following mandatory features:

HTTP Methods: Support for GET, POST, and DELETE requests.

I/O Multiplexing: Non-blocking I/O operations using poll().

CGI Support: Execution of .py and .php scripts via CGI (Common Gateway Interface).

File Upload: Robust handling of multipart/form-data for file uploads via POST.

Configuration: Custom .conf file parsing (setup ports, hostnames, default error pages, client body size limits, routes, etc.).

Static Content: Serving HTML, CSS, JS, and images.

Directory Listing: Autoindex feature when directory listing is enabled.

Error Handling: Custom error pages (404, 403, 500, 413, etc.).


Resources & AI Usage:

Ressources
https://www.geeksforgeeks.org/cpp/socket-programming-in-cpp/
https://blog.stephane-robert.info/docs/admin-serveurs/linux/curl/
https://medium.com/@avocadi/what-is-epoll-9bbc74272f7c
https://www.scaffoldgainford.com/blog/what-is-the-poll-function-for-sockets-311835.html

AI Usage
As required by the subject, we explicitly state that Artificial Intelligence tools (ChatGPT / Gemini) were used during the development of this project. They were utilized as a support tool for the following specific tasks:

Concept Explanation: Clarifying complex topics such as non-blocking I/O, the lifecycle of poll(), and the structure of HTTP requests/responses.

Debugging: Assisting in identifying the cause of complex errors (e.g., parsing logic issues, memory management in C++98) that were difficult to trace manually.

Test Generation: Generating Python scripts for CGI testing and complex curl commands to verify edge cases (like multipart uploads).


