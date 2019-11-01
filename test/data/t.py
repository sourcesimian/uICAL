#!/usr/bin/env python
import json
import sys


with open('./test/data/rrule.dat') as fh:
    rrule = None
    dtstart = None
    extra = {}
    expected = []
    comments = []
    cases = []

    for line in fh:
        line = line.rstrip()
        if line.startswith('#'):
            comments.append(line)
        elif line.startswith('RRULE'):
            rrule = line
        elif line.startswith('DTSTART'):
            dtstart = line
        elif line.startswith(' - '):
            expected.append(line[3:])
        elif line:
            key, value = line.split(':', 1)
            extra['key'] = value
        elif not line and rrule and dtstart and expected:
            for c in comments:
                print(c)
            json.dump([
                rrule,
                dtstart,
                extra,
                expected
            ], sys.stdout, indent=4)
            sys.stdout.write(",\n")
            rrule = None
            dtstart = None
            extra = {}
            expected = []
            comments = []

