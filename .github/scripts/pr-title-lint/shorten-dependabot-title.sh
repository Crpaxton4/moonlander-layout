#!/usr/bin/env bash
set -euo pipefail

: "${TITLE:?not set or empty}"
: "${PR_NUMBER:?not set or empty}"
: "${GH_TOKEN:?not set or empty}"
: "${GITHUB_REPOSITORY:?not set or empty}"
: "${GITHUB_OUTPUT:?not set or empty}"

# Dependabot names a pre-commit hook by its full GitHub URL, and it adds the group to
# the title of a single update. Both make the header longer than the 100 characters
# that commitlint allows. Thus the title keeps only the repository name of the hook
# and loses the group
short_title="$(sed -E \
    -e 's#https://github\.com/[^/ ]+/##g' \
    -e 's/ in the [^ ]+ group( across [0-9]+ director(y|ies))?$//' \
    <<<"$TITLE")"

if [[ "$short_title" != "$TITLE" ]]; then
    gh pr edit "$PR_NUMBER" --repo "$GITHUB_REPOSITORY" --title "$short_title"
fi
echo "title=$short_title" >>"$GITHUB_OUTPUT"
