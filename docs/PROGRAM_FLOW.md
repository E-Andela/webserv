Start program

1. Parse configuration file
   - Load server blocks
   - Load route and CGI configurations

2. Initialize all server sockets
   - Create socket
   - Bind to port
   - Listen for connections

3. Enter event loop (poll/select/epoll)
   - Monitor all server and client sockets

4. On new connection
   - Accept client (accept())
   - Add to poll watch list

5. On client ready to read
   - Read HTTP request
   - Parse request (method, path, headers, body, etc)

6. Match request to route (route lookup to determine next step)
   - Determine handler: static file, CGI, upload, or redirect

7. Generate HTTP response
   - Serve file, run CGI, store upload, etc.
   - Set status code and headers

8. On client ready to write
   - Send response
   - Optionally close connection

9. Handle errors, timeouts, disconnects gracefully

Repeat loop

On shutdown
   - Close all sockets
   - Free resources

End program
