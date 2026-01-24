#!/usr/bin/python3
import os
import sys

print("Content-Type: text/html\r\n\r\n")
print("<html><body><h1>CGI Python Works</h1>")
print("<h2>Method: " + os.environ.get('REQUEST_METHOD', 'Unknown') + "</h2>")
print("<h2>Body Content:</h2><pre>")
try:
    body = sys.stdin.read()
    print(body)
except:
    print("No body")
print("</pre></body></html>")
