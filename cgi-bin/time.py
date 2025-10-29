#!/usr/bin/env python3
from datetime import datetime

print("Content-Type: text/html")
print("Cache-Control: no-store, no-cache, must-revalidate")
print("Pragma: no-cache")
print("Expires: 0")
print()  # Blank line between headers and body
print(f"<html><body><h1>Time: {datetime.now()}</h1></body></html>")