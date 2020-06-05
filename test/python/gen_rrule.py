#!/usr/bin/env python3

import sys
import uICAL

rrule = sys.argv[1]
dtstart = sys.argv[2]

rr = uICAL.RRule(rrule, dtstart)

next = rr.next() 
print('%s' % rr)
while next:
    print("%04d%02d%02dT%02d%02d%02d" % rr.now())
    next = rr.next() 


