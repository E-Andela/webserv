# C++ Naming Conventions – Webserv Project

This document defines the naming conventions we use throughout the Webserv project to keep our codebase clean, readable, and consistent.

---

## ✅ General Guidelines

- Use **clear, descriptive names** – avoid vague names like `temp`, `data`, `stuff`.
- Prefer **camelCase** for most things.
- Use **PascalCase** for types.
- Use **lines_** (trailing underscore) for private members.
- Be consistent within the whole project.

---

## 🔠 Variable Naming

| Kind             | Style         | Example              |
|------------------|---------------|----------------------|
| Local variables  | `camelCase`   | `requestBody`        |
| Function params  | `camelCase`   | `configPath`         |
| Private members  | `camelCase_`  | `socketFd_`, `lines_`|
| Constants        | `kCamelCase` or `ALL_CAPS` | `kMaxClients`, `MAX_BUFFER` |

---

## 🧱 Classes and Types

| Kind         | Style         | Example             |
|--------------|---------------|---------------------|
| Class names  | `PascalCase`  | `HttpRequest`, `ServerConfig` |
| Struct names | `PascalCase`  | `Route`, `ClientInfo` |
| Enum names   | `PascalCase`  | `ConnectionState`   |
| Enum values  | `ALL_CAPS`    | `READING`, `DONE`   |

---

## ⚙️ Functions and Methods

| Kind           | Style         | Example               |
|----------------|---------------|------------------------|
| Regular        | `camelCase()` | `parseHeader()`       |
| Boolean        | `isX()`/`hasX()` | `isAlive()`, `hasHeader()` |
| Getters        | `getX()`      | `getPort()`           |
| Setters        | `setX()`      | `setRootPath()`       |

---

## 🗂 File & Folder Names

| Kind            | Style         | Example               |
|-----------------|---------------|------------------------|
| Header files    | `PascalCase.hpp` or `snake_case.hpp` | `HttpRequest.hpp` or `http_request.hpp` |
| Source files    | Same as headers |                      |
| Directories     | `snake_case`   | `src/server`, `include/utils` |

---

## 📛 Naming Don’ts

- ❌ Don't use names like `a`, `b`, `thing`, `var`, etc.
- ❌ Don't start identifiers with `_` (reserved in global scope).
- ❌ Don't use Hungarian notation (`strName`, `iCount`, etc.).

---

## 🧰 Tools & Tips

- Use [`clang-format`](https://clang.llvm.org/docs/ClangFormat.html) for auto-formatting.
- Use consistent naming in **commits, comments, and test cases**.
- Document custom naming in code if unclear.

---

> When in doubt, **be descriptive** and **stay consistent**.
