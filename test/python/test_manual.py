#!/usr/bin/env python

def test_friday_thirteenth():
    import uICAL

    dtstart = uICAL.DateTime("19700101T090000")
    begin = uICAL.DateTime("21051113T090000")
    rr = uICAL.RRule("FREQ=MONTHLY;BYDAY=FR;BYMONTHDAY=13", dtstart, begin=begin)
    
    year = None
    now = None
    for i in range(10000):
        assert rr.next() == True
        dt = rr.now()
        now = dt.as_tuple()
        if year:
            assert year <= now[0], "Year decreased after %d occureences" % i
        year = now[0]
        assert now[2] == 13
    print("%04d-%02d-%02dT%02d:%02d:%02d%s" % now)


test_friday_thirteenth()
