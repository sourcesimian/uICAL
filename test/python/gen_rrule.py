#!/usr/bin/env python3

import sys
import uICAL

def attrs_from_str(str):
    attrs = {}
    for kv in str.split(';'):
        k, v = kv.split('=', 1)
        attrs[k] = v
    return attrs

def attrs_as_str(attrs):
    if not attrs:
        return ''
    return ';' + ';'.join(['%s:%s' % (k, v) for k, v in attrs.items()])


for arg in sys.argv[1:]:
    key, value = arg.split(':', 1)
    if ';' in key:
        key, attrs = key.split(';', 1)
    if key == 'RRULE':
        rrule = value
    if key == 'DTSTART':
        dtstart = value

rr = uICAL.RRule(rrule, dtstart)

next = rr.next() 
print()
print('%s' % rr)
print()
print('DTSTART:%s' % dtstart)
print('RRULE:%s' % rrule)
while next:
    print(" - %04d%02d%02dT%02d%02d%02d" % rr.now())
    next = rr.next() 


