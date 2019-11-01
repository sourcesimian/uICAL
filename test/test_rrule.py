import pytest 

def names(argvalue):
    if isinstance(argvalue, (list,)):
        return '-'
    if argvalue.startswith("RRULE"):
        return argvalue[6:]
    return '-'

params = (
    ('rrule', 'dtstart', 'exdates', 'expected'),
    [
        ################################################################################
        # Derived from:                                                                #
        #     https://icalendar.org/iCalendar-RFC-5545/3-8-5-3-recurrence-rule.html    #
        ################################################################################
        [
            "RRULE:FREQ=DAILY;COUNT=4",
            "DTSTART;TZID=America/New_York:19970902T090000",
            [],
            [
                "19970902T090000",
                "19970903T090000",
                "19970904T090000",
                "19970905T090000",
            ]
        ],
        [
            "RRULE:FREQ=DAILY;UNTIL=19970907T090000",
            "DTSTART;TZID=America/New_York:19970902T090000",
            [],
            [
                "19970902T090000",
                "19970903T090000",
                "19970904T090000",
                "19970905T090000",
                "19970906T090000",
                "19970907T090000",
            ]
        ],
        [
            "RRULE:FREQ=DAILY;INTERVAL=2",
            "DTSTART;TZID=America/New_York:19970902T090000",
            [],
            [
                "19970902T090000",
                "19970904T090000",
                "19970906T090000",
                "19970908T090000",
                "19970910T090000",
                "19970912T090000",
                "19970914T090000",
                "19970916T090000",
                "19970918T090000",
                "19970920T090000",
                "19970922T090000",
                "...",
            ]
        ],
        [
            "RRULE:FREQ=DAILY;INTERVAL=10;COUNT=5",
            "DTSTART;TZID=America/New_York:19970902T090000",
            [],
            [
                "19970902T090000",
                "19970912T090000",
                "19970922T090000",
                "19971002T090000",
                "19971012T090000",
            ]
        ],
        [
            "RRULE:FREQ=YEARLY;UNTIL=20000131T140000Z;BYMONTH=1;BYDAY=TH,SA",
            "DTSTART;TZID=America/New_York:19980101T090000",
            [],
            [
                "19980101T090000",
                "19980103T090000",
                "19980108T090000",
                "19980110T090000",
                "19980115T090000",
                "19980117T090000",
                "19980122T090000",
                "19980124T090000",
                "19980129T090000",
                "19980131T090000",
                "19990102T090000",
                "...",
            ]
        ],
        [
            "RRULE:FREQ=DAILY;UNTIL=20000131T140000Z;BYMONTH=1;BYDAY=MO,TH",
            "DTSTART;TZID=America/New_York:19980101T090000",
            [],
            [
                "19980101T090000",
                "19980105T090000",
                "19980108T090000",
                "19980112T090000",
                "19980115T090000",
                "19980119T090000",
                "19980122T090000",
                "19980126T090000",
                "19980129T090000",
                "19990104T090000",
                "19990107T090000",
                "...",
            ]
        ],
        [
            "RRULE:FREQ=WEEKLY;COUNT=10",
            "DTSTART;TZID=America/New_York:19970902T090000",
            [],
            [
                "19970902T090000",
                "19970909T090000",
                "19970916T090000",
                "19970923T090000",
                "19970930T090000",
                "19971007T090000",
                "19971014T090000",
                "19971021T090000",
                "19971028T090000",
                "19971104T090000",
            ]
        ],
        [
            "RRULE:FREQ=WEEKLY;UNTIL=19971004T090000Z",
            "DTSTART;TZID=America/New_York:19970902T090000",
            [],
            [
                "19970902T090000",
                "19970909T090000",
                "19970916T090000",
                "19970923T090000",
                "19970930T090000",
            ]
        ],
        [
            "RRULE:FREQ=WEEKLY;INTERVAL=2;WKST=SU",
            "DTSTART;TZID=America/New_York:19970902T090000",
            [],
            [
                "19970902T090000",
                "19970916T090000",
                "19970930T090000",
                "19971014T090000",
                "19971028T090000",
                "19971111T090000",
                "19971125T090000",
                "19971209T090000",
                "19971223T090000",
                "19980106T090000",
                "19980120T090000",
                "...",
            ]
        ],
        [
            "RRULE:FREQ=WEEKLY;UNTIL=19971007T000000Z;WKST=SU;BYDAY=TU,TH",
            "DTSTART;TZID=America/New_York:19970902T090000",
            [],
            [
                "19970902T090000",
                "19970904T090000",
                "19970909T090000",
                "19970911T090000",
                "19970916T090000",
                "19970918T090000",
                "19970923T090000",
                "19970925T090000",
                "19970930T090000",
                "19971002T090000",
            ]
        ],
        [
            "RRULE:FREQ=WEEKLY;COUNT=10;WKST=SU;BYDAY=TU,TH",
            "DTSTART;TZID=America/New_York:19970902T090000",
            [],
            [
                "19970902T090000",
                "19970904T090000",
                "19970909T090000",
                "19970911T090000",
                "19970916T090000",
                "19970918T090000",
                "19970923T090000",
                "19970925T090000",
                "19970930T090000",
                "19971002T090000",
            ]
        ],
        [
            "RRULE:FREQ=WEEKLY;INTERVAL=2;UNTIL=19971224T000000Z;WKST=SU;BYDAY=MO,WE,FR",
            "DTSTART;TZID=America/New_York:19970901T090000",
            [],
            [
                "19970901T090000",
                "19970903T090000",
                "19970905T090000",
                "19970915T090000",
                "19970917T090000",
                "19970919T090000",
                "19970929T090000",
                "19971001T090000",
                "19971003T090000",
                "19971013T090000",
                "19971015T090000",
                "...",
            ]
        ],
        [
            "RRULE:FREQ=WEEKLY;INTERVAL=2;COUNT=8;WKST=SU;BYDAY=TU,TH",
            "DTSTART;TZID=America/New_York:19970902T090000",
            [],
            [
                "19970902T090000",
                "19970904T090000",
                "19970916T090000",
                "19970918T090000",
                "19970930T090000",
                "19971002T090000",
                "19971014T090000",
                "19971016T090000",
            ]
        ],
        [
            "RRULE:FREQ=MONTHLY;COUNT=10;BYDAY=1FR",
            "DTSTART;TZID=America/New_York:19970905T090000",
            [],
            [
                "19970905T090000",
                "19971003T090000",
                "19971107T090000",
                "19971205T090000",
                "19980102T090000",
                "19980206T090000",
                "19980306T090000",
                "19980403T090000",
                "19980501T090000",
                "19980605T090000",
            ]
        ],
        [
            "RRULE:FREQ=MONTHLY;UNTIL=19971224T000000Z;BYDAY=1FR",
            "DTSTART;TZID=America/New_York:19970905T090000",
            [],
            [
                "19970905T090000",
                "19971003T090000",
                "19971107T090000",
                "19971205T090000",
            ]
        ],
        [
            "RRULE:FREQ=MONTHLY;INTERVAL=2;COUNT=10;BYDAY=1SU,-1SU",
            "DTSTART;TZID=America/New_York:19970907T090000",
            [],
            [
                "19970907T090000",
                "19970928T090000",
                "19971102T090000",
                "19971130T090000",
                "19980104T090000",
                "19980125T090000",
                "19980301T090000",
                "19980329T090000",
                "19980503T090000",
                "19980531T090000",
            ]
        ],
        [
            "RRULE:FREQ=MONTHLY;COUNT=6;BYDAY=-2MO",
            "DTSTART;TZID=America/New_York:19970922T090000",
            [],
            [
                "19970922T090000",
                "19971020T090000",
                "19971117T090000",
                "19971222T090000",
                "19980119T090000",
                "19980216T090000",
            ]
        ],
        [
            "RRULE:FREQ=MONTHLY;BYMONTHDAY=-3",
            "DTSTART;TZID=America/New_York:19970928T090000",
            [],
            [
                "19970928T090000",
                "19971029T090000",
                "19971128T090000",
                "19971229T090000",
                "19980129T090000",
                "19980226T090000",
                "19980329T090000",
                "19980428T090000",
                "19980529T090000",
                "19980628T090000",
                "19980729T090000",
                "...",
            ]
        ],
        [
            "RRULE:FREQ=MONTHLY;COUNT=10;BYMONTHDAY=2,15",
            "DTSTART;TZID=America/New_York:19970902T090000",
            [],
            [
                "19970902T090000",
                "19970915T090000",
                "19971002T090000",
                "19971015T090000",
                "19971102T090000",
                "19971115T090000",
                "19971202T090000",
                "19971215T090000",
                "19980102T090000",
                "19980115T090000",
            ]
        ],
        [
            "RRULE:FREQ=MONTHLY;COUNT=10;BYMONTHDAY=1,-1",
            "DTSTART;TZID=America/New_York:19971031T090000",
            [],
            [
                "19971031T090000",
                "19971101T090000",
                "19971130T090000",
                "19971201T090000",
                "19971231T090000",
                "19980101T090000",
                "19980131T090000",
                "19980201T090000",
                "19980228T090000",
                "19980301T090000",
            ]
        ],
        [
            "RRULE:FREQ=MONTHLY;INTERVAL=18;COUNT=10;BYMONTHDAY=10,11,14,15",
            "DTSTART;TZID=America/New_York:19970910T090000",
            [],
            [
                "19970910T090000",
                "19970911T090000",
                "19970914T090000",
                "19970915T090000",
                "19990310T090000",
                "19990311T090000",
                "19990314T090000",
                "19990315T090000",
                "20000910T090000",
                "20000911T090000",
            ]
        ],
        [
            "RRULE:FREQ=MONTHLY;INTERVAL=2;BYDAY=TU",
            "DTSTART;TZID=America/New_York:19970902T090000",
            [],
            [
                "19970902T090000",
                "19970909T090000",
                "19970916T090000",
                "19970923T090000",
                "19970930T090000",
                "19971104T090000",
                "19971111T090000",
                "19971118T090000",
                "19971125T090000",
                "19980106T090000",
                "19980113T090000",
                "...",
            ]
        ],
        [
            "RRULE:FREQ=YEARLY;COUNT=10;BYMONTH=6,7",
            "DTSTART;TZID=America/New_York:19970610T090000",
            [],
            [
                "19970610T090000",
                "19970710T090000",
                "19980610T090000",
                "19980710T090000",
                "19990610T090000",
                "19990710T090000",
                "20000610T090000",
                "20000710T090000",
                "20010610T090000",
                "20010710T090000",
            ]
        ],
        [
            "RRULE:FREQ=YEARLY;INTERVAL=2;COUNT=10;BYMONTH=1,2,3",
            "DTSTART;TZID=America/New_York:19970310T090000",
            [],
            [
                "19970310T090000",
                "19990110T090000",
                "19990210T090000",
                "19990310T090000",
                "20010110T090000",
                "20010210T090000",
                "20010310T090000",
                "20030110T090000",
                "20030210T090000",
                "20030310T090000",
            ]
        ],
        [
            "RRULE:FREQ=YEARLY;INTERVAL=3;COUNT=10;BYYEARDAY=1,100,200",
            "DTSTART;TZID=America/New_York:19970101T090000",
            [],
            [
                "19970101T090000",
                "19970410T090000",
                "19970719T090000",
                "20000101T090000",
                "20000409T090000",
                "20000718T090000",
                "20030101T090000",
                "20030410T090000",
                "20030719T090000",
                "20060101T090000",
            ]
        ],
        [
            "RRULE:FREQ=YEARLY;BYDAY=20MO",
            "DTSTART;TZID=America/New_York:19970519T090000",
            [],
            [
                "19970519T090000",
                "19980518T090000",
                "19990517T090000",
                "20000515T090000",
                "20010514T090000",
                "20020520T090000",
                "20030519T090000",
                "20040517T090000",
                "20050516T090000",
                "20060515T090000",
                "20070514T090000",
                "...",
            ]
        ],
        [
            "RRULE:FREQ=YEARLY;BYWEEKNO=20;BYDAY=MO",
            "DTSTART;TZID=America/New_York:19970512T090000",
            [],
            [
                "19970512T090000",
                "19980511T090000",
                "19990517T090000",
                "20000515T090000",
                "20010514T090000",
                "20020513T090000",
                "20030512T090000",
                "20040510T090000",
                "20050516T090000",
                "20060515T090000",
                "20070514T090000",
                "...",
            ]
        ],
        [
            "RRULE:FREQ=YEARLY;BYMONTH=3;BYDAY=TH",
            "DTSTART;TZID=America/New_York:19970313T090000",
            [],
            [
                "19970313T090000",
                "19970320T090000",
                "19970327T090000",
                "19980305T090000",
                "19980312T090000",
                "19980319T090000",
                "19980326T090000",
                "19990304T090000",
                "19990311T090000",
                "19990318T090000",
                "19990325T090000",
                "...",
            ]
        ],
        [
            "RRULE:FREQ=YEARLY;BYDAY=TH;BYMONTH=6,7,8",
            "DTSTART;TZID=America/New_York:19970605T090000",
            [],
            [
                "19970605T090000",
                "19970612T090000",
                "19970619T090000",
                "19970626T090000",
                "19970703T090000",
                "19970710T090000",
                "19970717T090000",
                "19970724T090000",
                "19970731T090000",
                "19970807T090000",
                "19970814T090000",
                "...",
            ]
        ],
        [
            "RRULE:FREQ=MONTHLY;BYDAY=FR;BYMONTHDAY=13",
            "DTSTART;TZID=America/New_York:19970902T090000",
            [
                "EXDATE;TZID=America/New_York:19970902T090000",
            ],
            [
                "19980213T090000",
                "19980313T090000",
                "19981113T090000",
                "19990813T090000",
                "20001013T090000",
                "20010413T090000",
                "20010713T090000",
                "20020913T090000",
                "20021213T090000",
                "20030613T090000",
                "20040213T090000",
                "...",
            ]
        ],
        [
            "RRULE:FREQ=MONTHLY;BYDAY=SA;BYMONTHDAY=7,8,9,10,11,12,13",
            "DTSTART;TZID=America/New_York:19970913T090000",
            [],
            [
                "19970913T090000",
                "19971011T090000",
                "19971108T090000",
                "19971213T090000",
                "19980110T090000",
                "19980207T090000",
                "19980307T090000",
                "19980411T090000",
                "19980509T090000",
                "19980613T090000",
                "19980711T090000",
                "...",
            ]
        ],
        [
            "RRULE:FREQ=YEARLY;INTERVAL=4;BYMONTH=11;BYDAY=TU;BYMONTHDAY=2,3,4,5,6,7,8",
            "DTSTART;TZID=America/New_York:19961105T090000",
            [],
            [
                "19961105T090000",
                "20001107T090000",
                "20041102T090000",
                "20081104T090000",
                "20121106T090000",
                "20161108T090000",
                "20201103T090000",
                "20241105T090000",
                "20281107T090000",
                "20321102T090000",
                "20361104T090000",
                "...",
            ]
        ],
        # The third instance into the month of one of Tuesday, Wednesday, or Thursday, for the next 3 months:
        [
            "RRULE:FREQ=MONTHLY;COUNT=3;BYDAY=TU,WE,TH;BYSETPOS=3",
            "DTSTART;TZID=America/New_York:19970904T090000",
            [],
            [
                "19970904T090000",
                "19971007T090000",
                "19971106T090000",
            ]
        ],
        # The second-to-last weekday of the month
        [
            "RRULE:FREQ=MONTHLY;BYDAY=MO,TU,WE,TH,FR;BYSETPOS=-2",
            "DTSTART;TZID=America/New_York:19970929T090000",
            [],
            [
                "19970929T090000",
                "19971030T090000",
                "19971127T090000",
                "19971230T090000",
                "19980129T090000",
                "19980226T090000",
                "19980330T090000",
                "...",
            ]
        ],
        [
            "RRULE:FREQ=HOURLY;INTERVAL=3;UNTIL=19970902T170000Z",
            "DTSTART;TZID=America/New_York:19970902T090000",
            [],
            [
                "19970902T090000",
                "19970902T120000",
                "19970902T150000",
            ]
        ],
        [
            "RRULE:FREQ=MINUTELY;INTERVAL=15;COUNT=6",
            "DTSTART;TZID=America/New_York:19970902T090000",
            [],
            [
                "19970902T090000",
                "19970902T091500",
                "19970902T093000",
                "19970902T094500",
                "19970902T100000",
                "19970902T101500",
            ]
        ],
        [
            "RRULE:FREQ=MINUTELY;INTERVAL=90;COUNT=4",
            "DTSTART;TZID=America/New_York:19970902T090000",
            [],
            [
                "19970902T090000",
                "19970902T103000",
                "19970902T120000",
                "19970902T133000",
            ]
        ],
        [
            "RRULE:FREQ=DAILY;BYHOUR=9,11;BYMINUTE=0,20,40",
            "DTSTART;TZID=America/New_York:19970902T090000",
            [],
            [
                "19970902T090000",
                "19970902T092000",
                "19970902T094000",
                "19970902T110000",
                "19970902T112000",
                "19970902T114000",
                "19970903T090000",
                "19970903T092000",
                "19970903T094000",
                "19970903T110000",
                "19970903T112000",
                "...",
            ]
        ],
        [
            "RRULE:FREQ=MINUTELY;INTERVAL=20;BYHOUR=9,12,15,16",
            "DTSTART;TZID=America/New_York:19970902T090000",
            [],
            [
                "19970902T090000",
                "19970902T092000",
                "19970902T094000",
                "19970902T120000",
                "19970902T122000",
                "19970902T124000",
                "19970902T150000",
                "19970902T152000",
                "19970902T154000",
                "19970902T160000",
                "19970902T162000",
                "...",
            ]
        ],
        [
            "RRULE:FREQ=WEEKLY;INTERVAL=2;COUNT=4;BYDAY=TU,SU;WKST=MO",
            "DTSTART;TZID=America/New_York:19970805T090000",
            [],
            [
                "19970805T090000",
                "19970810T090000",
                "19970819T090000",
                "19970824T090000",
            ]
        ],
        [
            "RRULE:FREQ=WEEKLY;INTERVAL=2;COUNT=4;BYDAY=TU,SU;WKST=SU",
            "DTSTART;TZID=America/New_York:19970805T090000",
            [],
            [
                "19970805T090000",
                "19970817T090000",
                "19970819T090000",
                "19970831T090000",
            ]
        ],
        [
            "RRULE:FREQ=MONTHLY;BYMONTHDAY=15,30;COUNT=5",
            "DTSTART;TZID=America/New_York:20070115T090000",
            [],
            [
                "20070115T090000",
                "20070130T090000",
                "20070215T090000",
                "20070315T090000",
                "20070330T090000",
            ]
        ],
        ################################################################################
        # Additional tests:                                                            #
        ################################################################################
        [
            "RRULE:FREQ=DAILY;COUNT=4",
            "DTSTART;TZID=America/New_York:19970902T090000",
            [
                "EXDATE;TZID=America/New_York:19970903T090000",
                "EXDATE;TZID=America/New_York:19970904T090000",
            ],
            [
                "19970902T090000",
                "19970905T090000",
            ]
        ],


    ],
    False,
    names
)

def list_rrule_no_tz(rrule, dtstart, begin, end, exdates, maxres):
    import uICAL
    rule = rrule.split(':', 1)[1]
    start = dtstart.split(':', 1)[1]

    excludes = [ex.split(':', 1)[1] for ex in exdates]

    rr = uICAL.RRule(rule, start, begin=begin, end=end, exclude=excludes)

    results = []
    while rr.next():
        results.append("%04d%02d%02dT%02d%02d%02d" % rr.now())
        if len(results) == maxres:
            break
    return results

    # from itertools import count
    # for n, r, e in zip(count(), results, expected):
    #     assert r == e, "Event index=%s" % n


@pytest.mark.parametrize(*params)
def test_rrule_no_tz(rrule, dtstart, exdates, expected):
    results = list_rrule_no_tz(rrule, dtstart, None, None, exdates, len(expected))

    if len(results) == len(expected):
        if expected[-1] == "...":
            results[-1] = "..."
    assert results == expected


@pytest.mark.parametrize(*params)
def test_rrule_stepping_begin(rrule, dtstart, exdates, expected):
    expected_all = expected

    if "COUNT" in rrule:
        return
    
    for i in range(1, len(expected_all)):
        begin = expected_all[i]
        if begin == "...":
            break
        expected = expected_all[i:]
        results = list_rrule_no_tz(rrule, dtstart, begin, None, exdates, len(expected))

        if len(results) == len(expected):
            if expected[-1] == "...":
                results[-1] = "..."
        
        assert results == expected, "Rolling index=%d" % i
        
        from itertools import count
        for c, r, e in zip(count(), results, expected):
            assert r == e, "Rolling index=%d, count=%d" % (i, c)


def test_begin_end():
    import uICAL

    rr = uICAL.RRule("FREQ=MONTHLY;BYDAY=FR;BYMONTHDAY=13", "19970902T090000", begin="23000101T090000")
    assert rr.next() == True
    assert rr.now() == (2300, 4, 13, 9, 0, 0)


def test_friday_thirteenth():
    import uICAL

    rr = uICAL.RRule("FREQ=MONTHLY;BYDAY=FR;BYMONTHDAY=13", "19700101T090000")
    
    year = None
    for i in range(10000):
        assert rr.next() == True
        now = rr.now()
        if year:
            assert year <= now[0], "Year decreased after %d occureences" % i
        year = now[0]
        assert now[2] == 13
