import pytest 

def names(argvalue):
    if isinstance(argvalue, (list,)):
        return '-'
    if argvalue.startswith("RRULE"):
        return argvalue[6:]
    return '-'

def gen_params(filename):
    tests = []
    with open(filename) as fh:
        rrule = None
        dtstart = None
        extra = []
        expected = []
        comments = []

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
                extra.append(line)
            elif not line and rrule and dtstart and expected:
                # for c in comments:
                #     print(c)
                tests.append(
                    [
                        rrule,
                        dtstart,
                        extra,
                        expected
                    ]
                )
                print(rrule)
                rrule = None
                dtstart = None
                extra = []
                expected = []
                comments = []

    return (
        ('rrule', 'dtstart', 'exdates', 'expected'),
        tests,
        False,
        names,
    )

params = gen_params('./test/data/rrule.txt')

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
