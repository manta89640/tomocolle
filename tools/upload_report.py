#!/usr/bin/env python3
"""Uploads an objdiff progress report to ByteMatch, for the commit that is checked out.

For when CI has no base ROM to make the report with. The token is a project's own, from its Manage page on ByteMatch:
put it in the environment as BYTEMATCH_TOKEN, or in the file .bytematch-token, which git ignores.

The report is uploaded as the progress of HEAD, so HEAD has to be what the report was built from and has to be on
GitHub, where ByteMatch reads the source it links to: the working tree must be clean and the commit pushed.
"""
import argparse
import gzip
import json
import os
import re
import subprocess
import sys
import urllib.error
import urllib.parse
import urllib.request
from pathlib import Path

ROOT = Path(__file__).parent.parent.resolve()
HOST = "https://bytematch.dev"
TOKEN_FILE = ROOT / ".bytematch-token"


def git(*args: str) -> str:
    return subprocess.run(["git", *args], cwd=ROOT, capture_output=True, text=True, check=True).stdout.strip()


def project_of_remote(remote: str) -> str:
    """Returns owner/repo of a GitHub remote, such as "https://github.com/owner/repo.git"."""
    match = re.search(r"github\.com[:/]([^/]+/[^/]+?)(?:\.git)?$", git("remote", "get-url", remote))
    if not match:
        sys.exit(f"The remote {remote} is not a GitHub repository: pass --project owner/repo")
    return match[1]


def read_token() -> str:
    token = os.environ.get("BYTEMATCH_TOKEN", "").strip()
    if not token and TOKEN_FILE.exists():
        token = TOKEN_FILE.read_text(encoding="utf-8").strip()
    if not token:
        sys.exit(f"No token: set BYTEMATCH_TOKEN, or put the token in {TOKEN_FILE.name}")
    return token


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__.splitlines()[0])
    parser.add_argument("report", type=Path, help="report.json from `ninja report`")
    parser.add_argument("--version", required=True, help="game version the report is for")
    parser.add_argument("--project", help="owner/repo on ByteMatch (default: that of the remote)")
    parser.add_argument("--remote", default="origin", help="git remote the commit has to be on (default: origin)")
    parser.add_argument("--dry-run", action="store_true", help="check everything and print the request, without sending")
    args = parser.parse_args()

    commit = git("rev-parse", "HEAD")
    # Untracked files are not part of the build unless a delinks.txt names them, and then configure.py would say so
    if git("status", "--porcelain", "--untracked-files=no"):
        sys.exit("The working tree has changes that are not committed, so the report is not that of HEAD")
    if not git("branch", "--remotes", "--contains", commit, "--list", f"{args.remote}/*"):
        sys.exit(f"{commit[:7]} is not on {args.remote}: push it first")

    project = args.project or project_of_remote(args.remote)
    report = args.report.read_bytes()
    measures = json.loads(report).get("measures", {})
    query = urllib.parse.urlencode({
        "version": args.version,
        "commit": commit,
        "commit_message": git("log", "-1", "--format=%s"),
        "commit_timestamp": git("log", "-1", "--format=%cI"),
    })
    url = f"{HOST}/api/v1/projects/{project}/reports?{query}"
    print(f"{args.report}: {measures.get('matched_functions', 0)} of {measures.get('total_functions')} functions, "
          f"{measures.get('matched_code', 0)} of {measures.get('total_code')} bytes of code, at {commit[:7]}")
    if args.dry_run:
        print(f"Would POST {len(report)} bytes to {url}")
        return 0

    request = urllib.request.Request(url, data=gzip.compress(report), method="POST", headers={
        "Authorization": f"Bearer {read_token()}",
        "Content-Type": "application/json",
        "Content-Encoding": "gzip",
    })
    try:
        with urllib.request.urlopen(request) as response:
            print(response.read().decode("utf-8", errors="replace"))
    except urllib.error.HTTPError as error:
        print(f"ByteMatch answered {error.code}: {error.read().decode('utf-8', errors='replace')}", file=sys.stderr)
        return 1
    return 0


if __name__ == "__main__":
    sys.exit(main())
