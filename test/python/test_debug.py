import pytest 
import test_rrule
import uICAL

def names(argvalue):
    if isinstance(argvalue, (list,)):
        return '-'
    if isinstance(argvalue, uICAL.VLine):
        return argvalue.value
    if isinstance(argvalue, uICAL.DateTime):
        return '-'
    return '-'

params = (
    ('rrule', 'dtstart', 'exdates', 'expected'),
    [
        [
            uICAL.VLine("RRULE:FREQ=MONTHLY;BYMONTHDAY=15,30;COUNT=5"),
            uICAL.VLine("DTSTART;TZID=America/New_York:20070115T090000"),
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

@pytest.mark.parametrize(*params)
def test_rrule_stepping_begin(rrule, dtstart, exdates, expected):
    expected_all = expected

    for i in range(1, len(expected_all)):
        expected = expected_all[i:]
        begin = expected_all[i]
        results = test_rrule.list_rrule_times(rrule, uICAL.DateTime(dtstart.value), begin, None, exdates, len(expected))

        if len(results) == len(expected):
            if expected[-1] == "...":
                results[-1] = "..."

        from itertools import count
        for c, r, e in zip(count(), results, expected):
            assert r == e, "Rolling index=%d, count=%d" % (i, c)
