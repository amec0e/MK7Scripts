#!/usr/bin/env python3
"""
Generate a WiFi Pineapple (MK7) recon OUI list from the maclookup.app JSON database.

Reads one or more maclookup.app JSON database files, keeps only MA-L (24-bit OUI)
entries, and writes a flat {"AABBCC": "Vendor Name", ...} JSON file for the Pineapple.

Input JSON is an array of objects shaped like:
    {"macPrefix": "00:00:00", "vendorName": "XEROX CORPORATION",
     "private": false, "blockType": "MA-L", "lastUpdate": "2015/11/17"}
"""

import sys
import os
import json
import argparse

# Deferred so a missing dependency produces a friendly message from the
# requirements check rather than a raw ImportError traceback on startup.
try:
    from unidecode import unidecode
except ImportError:
    unidecode = None

MIN_PYTHON = (3, 6)


def check_requirements():
    """Return a list of human-readable problems; empty list means all good."""
    problems = []

    if sys.version_info < MIN_PYTHON:
        have = ".".join(str(n) for n in sys.version_info[:3])
        need = ".".join(str(n) for n in MIN_PYTHON)
        problems.append(f"Python {need}+ is required (you have {have}).")

    if unidecode is None:
        problems.append(
            "Missing required package 'unidecode'. Install it with:\n"
            "    pip3 install unidecode"
        )

    return problems


def strip_non_alphanumeric(text):
    allowed_chars = set("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789.,& ")
    cleaned_text = ''.join(char if char in allowed_chars else ' ' for char in unidecode(text))
    cleaned_text = ' '.join(cleaned_text.split())  # Collapse runs of whitespace to a single space
    return cleaned_text


def load_entries(input_file):
    """Load and validate one maclookup.app JSON file. Exits with a clear message on failure."""
    if not os.path.isfile(input_file):
        sys.exit(f"Error: input file not found: {input_file}\n"
                 f"Check the path, or run 'ls' to see what's in the current folder.")

    try:
        with open(input_file, encoding='utf-8-sig') as jsonfile:
            entries = json.load(jsonfile)
    except json.JSONDecodeError:
        # Most common real-world cause: the maclookup download token expired and
        # the server returned an HTML page instead of the JSON database.
        sys.exit(f"Error: {input_file} is not valid JSON.\n"
                 f"If you just downloaded it, the download token may have expired and you\n"
                 f"got an HTML page instead. Check with:  head -c 100 {input_file}\n"
                 f"(a good file starts with '[{{\"macPrefix\"').\n"
                 f"If so, grab a fresh link from https://maclookup.app/downloads/json-database")

    if not isinstance(entries, list):
        sys.exit(f"Error: expected {input_file} to contain a JSON array of records, "
                 f"got {type(entries).__name__}.")

    return entries


def process_json(input_file, mac_dict):
    for entry in load_entries(input_file):
        if not isinstance(entry, dict):
            continue
        if entry.get('blockType', '').strip() != 'MA-L':
            continue

        mac_prefix_value = entry.get('macPrefix', '').replace(":", "").replace("-", "").strip().upper()
        if not mac_prefix_value:
            continue

        vendor_name = strip_non_alphanumeric(entry.get('vendorName', ''))
        mac_dict[mac_prefix_value] = vendor_name


def build_parser():
    parser = argparse.ArgumentParser(
        prog="Process_MAL_Only_JSON.py",
        description="Build a WiFi Pineapple (MK7) recon OUI list from the maclookup.app "
                    "JSON database. Keeps only MA-L entries and writes a flat "
                    "{prefix: vendor} JSON file.",
        epilog=(
            "examples:\n"
            "  # basic use: one input file, one output file\n"
            "  python3 Process_MAL_Only_JSON.py -i ouis_source.json -o ouis.txt\n\n"
            "  # merge several downloads into one list\n"
            "  python3 Process_MAL_Only_JSON.py -i jan.json feb.json -o ouis.txt\n\n"
            "getting the input file:\n"
            "  Download the JSON database from https://maclookup.app/downloads/json-database\n"
            "  (the download link carries a token that rotates daily).\n\n"
            "requirements:\n"
            "  Python 3.6+ and the 'unidecode' package (pip3 install unidecode)."
        ),
        formatter_class=argparse.RawDescriptionHelpFormatter,
    )
    parser.add_argument('-i', '--input', type=str, nargs='+', required=True, metavar='FILE',
                        help='One or more maclookup.app JSON database files to read.')
    parser.add_argument('-o', '--output', type=str, required=True, metavar='FILE',
                        help='Path of the OUI list to write (e.g. ouis.txt).')
    return parser


def main():
    problems = check_requirements()
    if problems:
        print("Requirement check failed:", file=sys.stderr)
        for p in problems:
            print("  - " + p, file=sys.stderr)
        sys.exit(1)

    args = build_parser().parse_args()

    # Accumulate across ALL input files into a single dict before writing.
    mac_dict = {}
    for input_file in args.input:
        process_json(input_file, mac_dict)

    if not mac_dict:
        sys.exit("Error: no MA-L entries were found in the input. "
                 "Is this really the maclookup.app JSON database?")

    # Sort by Mac Prefix length (smallest first), then lexicographically.
    sorted_mac_dict = {k: v for k, v in sorted(mac_dict.items(), key=lambda x: (len(x[0]), x[0]))}

    try:
        with open(args.output, 'w', encoding='utf-8') as out:
            json.dump(sorted_mac_dict, out, ensure_ascii=False)
    except OSError as e:
        sys.exit(f"Error: could not write output file {args.output}: {e}")

    print(f"Done: wrote {len(sorted_mac_dict)} MA-L entries to {args.output}")


if __name__ == "__main__":
    main()
