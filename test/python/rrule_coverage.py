#!/usr/bin/env python3

# +----------+--------+--------+-------+-------+------+-------+------+
# |          |SECONDLY|MINUTELY|HOURLY |DAILY  |WEEKLY|MONTHLY|YEARLY|
# +----------+--------+--------+-------+-------+------+-------+------+
# |BYMONTH   |Limit   |Limit   |Limit  |Limit  |Limit |Limit  |Expand|
# |BYWEEKNO  |N/A     |N/A     |N/A    |N/A    |N/A   |N/A    |Expand|
# |BYYEARDAY |Limit   |Limit   |Limit  |N/A    |N/A   |N/A    |Expand|
# |BYMONTHDAY|Limit   |Limit   |Limit  |Limit  |N/A   |Expand |Expand|
# |BYDAY     |Limit   |Limit   |Limit  |Limit  |Expand|Note 1 |Note 2|
# |BYHOUR    |Limit   |Limit   |Limit  |Expand |Expand|Expand |Expand|
# |BYMINUTE  |Limit   |Limit   |Expand |Expand |Expand|Expand |Expand|
# |BYSECOND  |Limit   |Expand  |Expand |Expand |Expand|Expand |Expand|
# |BYSETPOS  |Limit   |Limit   |Limit  |Limit  |Limit |Limit  |Limit |
# +----------+--------+--------+-------+-------+------+-------+------+
# ref: https://icalendar.org/iCalendar-RFC-5545/3-3-10-recurrence-rule.html

import sys
from collections import namedtuple
import itertools

KEYS = ('FREQ', 'BYMONTH', 'BYWEEKNO', 'BYYEARDAY', 'BYMONTHDAY', 'BYDAY', 'BYHOUR', 'BYMINUTE', 'BYSECOND', 'BYSETPOS')

Rule = namedtuple('Rule', ','.join(KEYS))

SPEC = [
    Rule('SECONDLY', 'Limit',  'N/A',    'Limit',  'Limit',  'Limit',  'Limit',  'Limit',  'Limit',  'Limit' ),
    Rule('MINUTELY', 'Limit',  'N/A',    'Limit',  'Limit',  'Limit',  'Limit',  'Limit',  'Expand', 'Limit' ),
    Rule('HOURLY',   'Limit',  'N/A',    'Limit',  'Limit',  'Limit',  'Limit',  'Expand', 'Expand', 'Limit' ),
    Rule('DAILY',    'Limit',  'N/A',    'N/A',    'Limit',  'Limit',  'Expand', 'Expand', 'Expand', 'Limit' ),
    Rule('WEEKLY',   'Limit',  'N/A',    'N/A',    'N/A',    'Expand', 'Expand', 'Expand', 'Expand', 'Limit' ),
    Rule('MONTHLY',  'Limit',  'N/A',    'N/A',    'Expand', 'Note1',  'Expand', 'Expand', 'Expand', 'Limit' ),
    Rule('YEARLY',   'Expand', 'Expand', 'Expand', 'Expand', 'Note2',  'Expand', 'Expand', 'Expand', 'Limit' ),
]

def write_table(out):
    sep = '+----------' * (len(SPEC) + 1) + '+\n'
    out.write(sep)
    for attr in KEYS:
        out.write('|%-10s|' % attr)
        for r in SPEC:
            out.write('%-10s|' % getattr(r, attr))
        out.write('\n')
        if attr == 'FREQ':
            out.write(sep)
    out.write(sep)


def iter_combinations():
    for r in SPEC:
        freq = r.FREQ
        opts = [k for k in KEYS[1:] if getattr(r, k) in ('Expand', 'Limit', 'Note1', 'Note2')]

        for r in range(len(opts)):
            for c in itertools.combinations(opts, r):
                yield tuple([freq] + sorted(c))

def iter_existing(file):
    other = ('COUNT', 'UNTIL', 'WKST', 'INTERVAL',)
    ret = []
    with open(file, 'rt') as fh:
        for line in fh:
            if line.startswith('RRULE:'):
                line = line.rstrip()
                _,fields = line.split(':', 1)
                attribs = {}
                for kv in fields.split(';'):
                    k, v = kv.split('=', 1)
                    attribs[k] = v
                freq = attribs['FREQ']
                del attribs['FREQ']
                c = set(attribs.keys()) - set(other)

                yield tuple([freq] + sorted(c))


def coverage(file):
    existing = set(iter_existing(file))
    all = set(iter_combinations())

    testing = 0
    for a in sorted(all):
        check = ' '
        if a in existing:
            check = '*'
            testing +=1
        print('%s %s' % (check, ' '.join(a)))
    print('Testing: %d of %d' % (testing, len(all)))

file = './test/data/rrule.txt'
write_table(sys.stdout)
print(file)
coverage(file)
