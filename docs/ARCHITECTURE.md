# Class & Object Overview 

This document outlines the major entities, classes, and their relationships

---

## Core Architecture

### Class: `Server`
Manages a single server instance and its socket.

Server
- port : int
- host: string
- socket_fd: int
+ start(): void
+ acceptclient(): int


### Class: `Connection`
represents a single client connection.

Connection
-fd_: int
- request_: Httprequest
- response_: HttpResponse
+ read(): void
+ write(): void


### Class: Poller
Event loop using poll() select() or similar.

Poller
- fds_:
- connections_: 
+ run():
+ updateFds(): void


## HTTP Handling

### Class: `HttpRequest`
parses incoming HTTP request.

HttpRequest
- method_: string
- uri_: string
- headers_: 
- body_: string
+ parse(): int(or bool)
+ isDone(): bool

### Class: `HttpResponse`
builds the response to send.

HttpResponse
- statusCode_: int
- headers_:
- body_:
+ build():
+ getStatus():

## Configuration


### Class `Config`
stores parsed config

Config
- lines_: vector<string>
+ addLines(): void
+ getLines(): vector


### Class `ConfigParser`
parses configuration files

ConfigParser
+ parse(path): Config

### struct `ServerConfig`

### struct `RouteConfig`


## Routing and Handlers

### Class: `Router`

### Interface: `IHandler`
base class for all route behavior

### Derived Handlers

#### Class `StaticFileHandler`
#### Class `CgiHandler`
#### Class `UploadHandler`


## Utilities

### Class `Logger`
Logger
+ info(): void
+ error(): void
+ debug(): void


### Class `ErorrPages`
ErrorPages
+ get(status): string
---

## Summary

| Category         | Classes                                     |
|------------------|---------------------------------------------|
| Core Server      | `Server`, `Connection`, `Poller`            |
| HTTP Logic       | `HttpRequest`, `HttpResponse`               |
| Configuration    | `Config`, `ConfigParser`, `ServerConfig`, `RouteConfig` |
| Routing          | `Router`, `IHandler`, + Handlers            |
| Utilities        | `Logger`, `ErrorPages`         |

---
