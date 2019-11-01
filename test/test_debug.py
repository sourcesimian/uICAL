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


@pytest.mark.parametrize(*params)
def test_rrule_stepping_begin(rrule, dtstart, exdates, expected):
    expected_all = expected

    for i in range(1, len(expected_all)):
        expected = expected_all[i:]
        begin = expected_all[i]
        results = list_rrule_no_tz(rrule, dtstart, begin, None, exdates, len(expected))

        if len(results) == len(expected):
            if expected[-1] == "...":
                results[-1] = "..."

        from itertools import count
        for c, r, e in zip(count(), results, expected):
            assert r == e, "Rolling index=%d, count=%d" % (i, c)
