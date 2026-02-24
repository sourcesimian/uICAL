import pytest
import warnings

import uICAL

def names(argvalue):
    if isinstance(argvalue, (list,)):
        return '-'
    if isinstance(argvalue, uICAL.VLine):
        return argvalue.value
    if isinstance(argvalue, uICAL.DateTime):
        return '-'
    return '-'

def line_to_datetime(line, tzmap):
    vline = uICAL.VLine(line)

    tzid = vline.getParam('TZID')
    if tzid and tzmap:
        return uICAL.DateTime(vline.value, tzid=tzid, tzmap=tzmap)
    else:
        return uICAL.DateTime(vline.value)


def gen_params(filename, tzmap):
    tests = []
    with open(filename) as fh:
        lineno = 0
        rrule = None
        dtstart = None
        exdates = []
        expected = []
        comments = []
        no = 0

        for line in fh:
            no += 1
            line = line.rstrip()
            if line.startswith('#'):
                comments.append(line)
            elif line.startswith('RRULE'):
                rrule = uICAL.VLine(line)
            elif line.startswith('DTSTART'):
                lineno = no
                dtstart = line_to_datetime(line, tzmap)
            elif line.startswith(' - '):
                expected.append(line[3:])
            elif line.startswith('EXDATE'):
                exdates.append(line_to_datetime(line, tzmap))
            elif line:
                warnings.warn(f"{filename}:{no}: {line}", SyntaxWarning)
                pass  # unknown line
            elif not line and rrule and dtstart and expected:
                # for c in comments:
                #     print(c)
                tests.append(
                    [
                        (filename, lineno),
                        rrule,
                        dtstart,
                        exdates,
                        expected
                    ]
                )
                print(rrule)
                lineno = 0
                rrule = None
                dtstart = None
                exdates = []
                expected = []
                comments = []

    return (
        ('src', 'rrule', 'dtstart', 'exdates', 'expected'),
        tests,
        False,
        names,
    )

tzmap = uICAL.TZMap()
tzmap.fromFile('./test/data/ical_timezone.txt')

params = gen_params('./test/data/rrule.txt', tzmap)

def list_rrule_times(rrule, dtstart, begin, end, exdates, maxres):

    begin = uICAL.DateTime(begin) if begin else  None
    end = uICAL.DateTime(end) if end else  None

    rr = uICAL.RRule(rrule.value, dtstart, begin=begin, end=end, exclude=exdates)

    results = []
    while rr.next():
        dt = rr.now()
        results.append(str(dt))
        if len(results) == maxres:
            if rr.next():
                results.append("...")
            break
    return results

    # from itertools import count
    # for n, r, e in zip(count(), results, expected):
    #     assert r == e, "Event index=%s" % n


@pytest.mark.parametrize(*params)
def test_rrule(src, rrule, dtstart, exdates, expected):

    maxres = len(expected)
    if expected[-1] == "...":
        maxres -= 1

    results = list_rrule_times(rrule, dtstart, None, None, exdates, maxres)

    results = [r[:15] for r in results]  # Strip away time zone

    assert len(expected) == len(results), src
    assert expected == results, src


@pytest.mark.parametrize(*params)
def test_rrule_stepping_begin(src, rrule, dtstart, exdates, expected):
    expected_all = expected

    if "COUNT" in rrule.value:
        return
    
    for i in range(1, len(expected_all)):
        begin = expected_all[i]
        if begin == "...":
            break
        expected = expected_all[i:]

        maxres = len(expected)
        if expected[-1] == "...":
            maxres -= 1

        results = list_rrule_times(rrule, dtstart, begin, None, exdates, maxres)

        results = [r[:15] for r in results]  # Strip away time zone

        assert len(expected) == len(results), "Rolling index=%d, %s" % (i, src)
        assert expected == results, "Rolling index=%d, %s" % (i, src)

        from itertools import count
        for c, r, e in zip(count(), results, expected):
            assert r == e, "Rolling index=%d, count=%d, %s" % (i, c, src)


def test_begin_end():

    dtstart = uICAL.DateTime("19970902T090000")
    begin = uICAL.DateTime("23000101T090000")
    rr = uICAL.RRule("FREQ=MONTHLY;BYDAY=FR;BYMONTHDAY=13", dtstart, begin=begin)
    assert rr.next() == True
    assert rr.now().as_tuple() == (2300, 4, 13, 9, 0, 0, '')


def test_friday_thirteenth():

    dtstart = uICAL.DateTime("19700101T090000")
    rr = uICAL.RRule("FREQ=MONTHLY;BYDAY=FR;BYMONTHDAY=13", dtstart)
    
    year = None
    for i in range(10000):
        assert rr.next() == True
        now = rr.now().as_tuple()
        if year:
            assert year <= now[0], "Year decreased after %d occureences" % i
        year = now[0]
        assert now[2] == 13
