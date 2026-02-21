import pytest 

def names(argvalue):
    if isinstance(argvalue, (list,)):
        return '-'
    if argvalue.startswith("RRULE"):
        return argvalue[6:]
    return '-'

def gen_params(filename, timezones_ical_filename):
    from helpers import get_tzid_datetime_offset;

    with open(timezones_ical_filename) as timezones_ical_fh:
        tz_ical_str = timezones_ical_fh.read()

    tests = []
    with open(filename) as fh:
        rrule = None
        dtstart = None
        dtstart_offset = None
        exdates = []
        expected = []
        comments = []

        for line in fh:
            line = line.rstrip()
            if line.startswith('#'):
                comments.append(line)

            elif line.startswith('RRULE'):
                rrule = line.split(":",1)[1]

            elif line.startswith('DTSTART'):
                dtstart = line.split(";",1)[1]
                dtstart_offset = get_tzid_datetime_offset(tz_ical_str, dtstart)
                dtstart = dtstart[-15:] + dtstart_offset

            elif line.startswith(' - '):
                expected_date = line[3:]
                if expected_date == "...":
                    expected.append(expected_date)
                else:
                    expected.append(expected_date + dtstart_offset)

            elif line.startswith('EXDATE'):
                exdate = line.split(";",1)[1]

                exdate_offset = get_tzid_datetime_offset(tz_ical_str, exdate)
                exdate = exdate[-15:] + exdate_offset
                
                exdates.append(exdate)


            elif not line and rrule and dtstart and expected:
                # for c in comments:
                #     print(c)
                tests.append(
                    [
                        rrule,
                        dtstart,
                        exdates,
                        expected,
                        dtstart_offset
                    ]
                )
                print(rrule)
                rrule = None
                dtstart = None
                dtstart_offset = None
                exdates = []
                expected = []
                comments = []

            elif not line == "":
                raise ValueError("Unhandled line: " + line)

    return (
        ('rrule', 'dtstart', 'exdates', 'expected', 'dtstart_offset'),
        tests,
        False,
        names,
    )

params = gen_params('./test/data/rrule.txt', './test/data/ical_timezones.txt')

def list_rrule(rrule, dtstart, begin, end, exdates, maxres, dtstart_offset):
    import uICAL

    rr = uICAL.RRule(rrule, dtstart, begin=begin, end=end, exclude=exdates)

    results = []
    while rr.next():
        results.append("%04d%02d%02dT%02d%02d%02d" % rr.now() + dtstart_offset)
        if len(results) == maxres:
            break

    if rr.next():
        results[-1] = "..."

    return results


@pytest.mark.parametrize(*params)
def test_rrule(rrule, dtstart, exdates, expected, dtstart_offset):
    results = list_rrule(rrule, dtstart, None, None, exdates, len(expected), dtstart_offset)

    assert results == expected


@pytest.mark.parametrize(*params)
def test_rrule_stepping_begin(rrule, dtstart, exdates, expected, dtstart_offset):
    expected_all = expected

    if "COUNT" in rrule:
        return
    
    for i in range(1, len(expected_all)):
        begin = expected_all[i]
        if begin == "...":
            break
        expected = expected_all[i:]
        results = list_rrule(rrule, dtstart, begin, None, exdates, len(expected), dtstart_offset)
        
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
