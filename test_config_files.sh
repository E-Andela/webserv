#!/usr/bin/env bash
set -u  # keep unset-var checks
set -o pipefail

CONFIG_DIR="config"
SERVER_BINARY="./webserv"
PATTERN="${1:-*.conf}"

if [[ ! -x "$SERVER_BINARY" ]]; then
  echo "❌ Error: $SERVER_BINARY not found or not executable. Build the project first."
  exit 1
fi

tmpfile="$(mktemp)"
trap 'rm -f "$tmpfile" "$OUTFILE"' EXIT
OUTFILE="$(mktemp)"

# Build explicit sort order
while IFS= read -r -d '' path; do
  base="$(basename "$path")"
  if [[ "$base" == "default.conf" ]]; then
    key="000000-default"
  elif [[ "$base" == "minimal.conf" ]]; then
    key="000001-minimal"
  elif [[ "$base" =~ ^test([0-9]+)_ ]]; then
    num="${BASH_REMATCH[1]}"
    printf -v padnum "%06d" "$num"
    key="000002-${padnum}-${base}"
  else
    key="999999-${base}"
  fi
  printf "%s\t%s\n" "$key" "$path" >> "$tmpfile"
done < <(find "$CONFIG_DIR" -type f -name "$PATTERN" -print0)

if [[ ! -s "$tmpfile" ]]; then
  echo "❌ No config files matched: $CONFIG_DIR/$PATTERN"
  exit 1
fi

TOTAL=0; PASSED=0; FAILED=0

while IFS=$'\t' read -r _key conf; do
  (( TOTAL++ ))
  echo "=================================================================="
  echo "🧪 Testing config: $conf"
  echo "=================================================================="

  # Temporarily disable 'exit on error' so failures don't abort the loop
  set +e
  "$SERVER_BINARY" "$conf" >"$OUTFILE" 2>&1
  EXITCODE=$?
  set -e 2>/dev/null || true  # re-enable if available

  OUTPUT="$(cat "$OUTFILE")"
  printf "%s\n" "$OUTPUT"
  echo
  echo "------------------ Highlight Summary ------------------"

  if printf "%s\n" "$OUTPUT" | grep -E '\[ERROR \]|\[WARNING \]|Config Error:' >/dev/null; then
    printf "%s\n" "$OUTPUT" | grep -E '\[ERROR \]|\[WARNING \]|Config Error:'
  else
    echo "✅ No issues found."
  fi

  if [[ $EXITCODE -eq 0 ]]; then
    echo "✅ Test PASS (exit code 0)"
    (( PASSED++ ))
  else
    echo "❌ Test FAIL (exit code $EXITCODE)"
    (( FAILED++ ))
  fi

  echo
done < <(sort -k1,1 "$tmpfile")

echo "==================== Summary ===================="
echo "Total: $TOTAL | Passed: $PASSED | Failed: $FAILED"
[[ $FAILED -eq 0 ]]
