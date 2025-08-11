# Config file grammar and examples

### notes:
#### server block:
each server block defines one server instance; a socket the program will bind to and handle request on.
we can have multiple server blocks.
Webserv must create one server object per server block.
listen localhost:9090; ==> IP and port to bind to 
    must parse
    validate port is in range
    validate address

root /data/server_a;  ==> root directory to serve files from(for static routes)
    must parse
    used as a base path when handling URL-to-file lookups.

server_name webserver-a.org;  ==> domain name usesd to distinguish virual hosts
    optional for parsing

allow_methods GET POST DELETE;  ==> which HTTP methods are allowed globally
    must parse
    should validate incopming requestst against this list

client_max_body_size 1612340999; ==> max size in bytes for incoming request bodies
    must parse
    enforce during POST PUT to avoid memory abuse

error+page <status> <path>; ==> custom HTML pages for error codes
    must parse
    use this to serve an error page isntead of default text

location /path {} ==> each location is a route handler
    patser must collect location blocks
    match incoming URIs against the closest matching location path
    overridde serverlevel config when applicable

inside Location blocks
    allow_methods ==> same as in server block, but only applies to this path - overrides serverwide method list
    index.html ==> default file to serve when URI is a directory
                    must parse
    autoindex on; ==> parse as bool on off
                        optional
                        whether to show a filder listing if no index file exists
    return 302 /; ==> HTTP redirect
                    must parse
                    if route matches, send 302 with a Location header

#### Example:

server {
    listen localhost:9090;
    root /data/server_a;
    server_name webserver-a.org;
    allow_methods GET POST DELETE;
    client_max_body_size 1612340999;
    error_page 402 /html/error/402.html;
    error_page 404 /html/error/404.html;
    error_page 405 /html/error/405.html;
    location / {
    	allow_methods GET DELETE GET GET GET;
        index html/index.html;
    }
    location /uploads {
        allow_methods GET POST DELETE;
		autoindex on;
    }
    location /cgi-bin {
		allow_methods POST GET;
    }
    location /redirectme {
        allow_methods GET;
        return 302 /;
    }
	location /forbidden {
		allow_methods ;
	}
}

server {
    listen localhost:9090;
    root /data/server_a;
    server_name webserver-a.org;
    allow_methods GET POST DELETE;
    client_max_body_size 1612340999;
    error_page 402 /html/error/402.html;
    error_page 404 /html/error/404.html;
    error_page 405 /html/error/405.html;
    location / {
    	allow_methods GET DELETE GET GET GET;
        index html/index.html;
    }
    location /uploads {
        allow_methods GET POST DELETE;
		autoindex on;
    }
    location /cgi-bin {
		allow_methods POST GET;
    }
    location /redirectme {
        allow_methods GET;
        return 302 /;
    }
	location /forbidden {
		allow_methods ;
	}
}


#### stupid/edge cases:
multiple identical server blocks
empty allow_methods
duplicate methods
whitespace formatting




## Config grammar:

config        ::= { block } ;

block         ::= identifier { args } "{" { directive | block } "}" ;

directive     ::= identifier { args } ";" ;

args          ::= identifier { identifier } ;

identifier    ::= string (any word, number, or path) ;




## Example AST:



server {
    listen 8080;
    server_name localhost;

    location / {
        methods GET POST;
        root /var/www/html;
    }

    location /upload {
        methods POST;
        upload_path /var/www/uploads;
    }
}



Config
└── Block: "server"
    ├── Directive: "listen" ["8080"]
    ├── Directive: "server_name" ["localhost"]
    ├── Block: "location" ["/"]
    │   ├── Directive: "methods" ["GET", "POST"]
    │   └── Directive: "root" ["/var/www/html"]
    └── Block: "location" ["/upload"]
        ├── Directive: "methods" ["POST"]
        └── Directive: "upload_path" ["/var/www/uploads"]
