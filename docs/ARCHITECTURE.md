# Class & Object Overview 

This document outlines the major entities, classes, and their relationships

---

## Core Architecture
<!-- 
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
+ getStatus(): -->

---

## Configuration


### Class `Tokenizer`
Breaks down configuration file into tokens.

Tokenizer:
- input: `std::istream&`
- currentChar: `char`

+ `next(); -> TokenType`
+ `peek(); -> TokenType`
+ `advance();`
+ `skipWhiteSpace();`

### Struct: `Directive`
Represents a single directive line in the config.

Directive
- name: `std::string'
- args: `std::vector<std::string>`

### Struct `Block`
Represents a block with nested directives or blocks.

Block
- name: `std::string`
- args: `std::vector<std::string>`
- directives: `std::vector<Directive>`
- children: `std::vector<Block>`

### Struct `Config`
top AST node containing all parsed blocks.

Config
- blocks: `std::vector<Block>`


### Class `ConfigParser`
parses the configuration file into a `Config` AST.

ConfigParser
+ `parse(path: string) -> Config`
+ `parseBlock(...) -> Block`

### Class `ConfigValidator`
Validates the AST and prints warnings/errors for faulty configs.

ConfigValidator
+ `validate(Config&) -> void`


### Class `ConfigMapper`
Maps the validated `Config` AST into real config objects.

ConfigMapper:
+ `map(Config&) -> std::vector<ServerConfig>`


### struct `ServerConfig`
Represents a server config.

ServerConfig:
- port: `int`
- serverName: `std::string`
- root: `std::string`
- index: `std::string`
- bodyLimit: `std::string`
- errorPages: `std::map<int, std::string>`
- routes: `std::vector<RouteConfig>`


### struct `RouteConfig`
Represents configuration for a `location` block.

RouteConfig:
- path: `std::string`
- methods: `std::vector<std::string>`
- uploadPath: `std::string`
- cgiPath: `std::string`
- cgiExtension: `std::string`
- redirectTo: `std::string`

### Class `ConfigError`
custom error for config subsystem

<!-- ## Routing and Handlers

### Class: `Router`

### Interface: `IHandler`
base class for all route behavior

### Derived Handlers

#### Class `StaticFileHandler`
#### Class `CgiHandler`
#### Class `UploadHandler` -->


## Utilities

### Class `Logger`
Logger
+ info(): void
+ error(): void
+ debug(): void


<!-- ### Class `ErorrPages`
ErrorPages
+ get(status): string -->
---
<!-- 
## Summary

| Category         | Classes                                     |
|------------------|---------------------------------------------|
| Core Server      | `Server`, `Connection`, `Poller`            |
| HTTP Logic       | `HttpRequest`, `HttpResponse`               |
| Configuration    | `Config`, `ConfigParser`, `ServerConfig`, `RouteConfig` |
| Routing          | `Router`, `IHandler`, + Handlers            |
| Utilities        | `Logger`, `ErrorPages`         |

--- -->
