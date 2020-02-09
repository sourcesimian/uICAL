import pytest

import uICAL


def test_basic_calendar1():
    ical = open("test/data/ical1.txt").read()
    cal = uICAL.Calendar(ical, begin="20191016T102000Z", end="20191017T103000-0500")

    res = []
    while cal.next():
        res.append(cal.current())

    exp = [
        {
            'type': 'EVENT',
            'summary': 'Irrigation Front',
            'start': (2019, 10, 17, 10, 0, 0, -300),
            'duration': 1200,
        },
        {
            'type': 'EVENT',
            'summary': 'Irrigation Back',
            'start': (2019, 10, 17, 10, 20, 0, -300),
            'duration': 300,
        },
        {
            'type': 'EVENT',
            'summary': 'Irrigation Beds',
            'start': (2019, 10, 17, 10, 30, 0, -300),
            'duration': 600,
        },
    ]
    assert res == exp
