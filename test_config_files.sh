#!/bin/bash

CONFIG_DIR="config"
SERVER_BINARY="./webserv"

if [ ! -f "$SERVER_BINARY" ]; then
    echo "❌ Error: $SERVER_BINARY not found. Build the project first."
    exit 1
fi

# Use version-aware sort (-V) for proper ordering
for conf in $(ls "$CONFIG_DIR"/*.conf | sort -V); do
    echo "=================================================================="
    echo "🧪 Testing config: $conf"
    echo "=================================================================="
    
    # Run the server with the config file and capture output
    OUTPUT=$($SERVER_BINARY "$conf" 2>&1)
    echo "$OUTPUT"

    echo
    echo "------------------ Highlight Summary ------------------"
    
    # Grep for errors/warnings in the output
    if echo "$OUTPUT" | grep -Eq "❌|⚠️"; then
        echo "$OUTPUT" | grep -E "❌|⚠️"
    else
        echo "✅ No issues found."
    fi

    echo -e "\n\n"
done
