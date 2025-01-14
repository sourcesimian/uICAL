import pytest

import uICAL


def test_basic_ical1():
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


def test_basic_range():
    ical = open("test/data/ical1.txt").read()

    # Miss on begin: (2019, 10, 17, 10, 0, 0, -300) 1200s
    cal = uICAL.Calendar(ical, begin="20191017T095500-0500", end="20191017T095959-0500")

    res = []
    while cal.next():
        res.append(cal.current())

    assert res == []

    # Miss on end: (2019, 10, 17, 10, 30, 0, -300) 600s
    cal = uICAL.Calendar(ical, begin="20191017T104000-0500", end="20191017T104100-0500")

    res = []
    while cal.next():
        res.append(cal.current())

    assert res == []

    # Full overlap
    cal = uICAL.Calendar(ical, begin="20191017T100900-0500", end="20191017T101000-0500")

    res = []
    while cal.next():
        res.append(cal.current())

    assert res == [
        {
            'duration': 1200,
            'start': (2019, 10, 17, 10, 0, 0, -300),
            'summary': 'Irrigation Front',
            'type': 'EVENT'
        },
    ]

    # Overlap begin and end
    cal = uICAL.Calendar(ical, begin="20191017T101900-0500", end="20191017T102100-0500")

    res = []
    while cal.next():
        res.append(cal.current())

    assert res == [
        {
            'duration': 1200,
            'start': (2019, 10, 17, 10, 0, 0, -300),
            'summary': 'Irrigation Front',
            'type': 'EVENT'
        },
        {
            'duration': 300,
            'start': (2019, 10, 17, 10, 20, 0, -300),
            'summary': 'Irrigation Back',
            'type': 'EVENT'
        },
    ]

def test_bad_range():
    # Negative Range
    ical = open("test/data/ical1.txt").read()

    with pytest.raises(uICAL.error) as ex:
        cal = uICAL.Calendar(ical, begin="20191017T104000-0500", end="20191017T103000-0500")
        while cal.next():
            pass
    assert str(ex.value) == "ValueError: Begin and end describe a negative range"

def test_folding():
    ical = open("test/data/ical_folding.txt").read()
    cal = uICAL.Calendar(ical, begin="20191016T102000Z", end="20191017T103000-0500")

    res = []
    while cal.next():
        res.append(cal.current())

    assert res == [
        {
            'type': 'EVENT',
            'summary': 'This is a short line that is folded.',
            'start': (2019, 10, 17, 10, 20, 0, -300),
            'duration': 300,
        },
        {
            'type': 'EVENT',
            'summary': 'This is a long line which needs to wrap as it\'s longer than 75 characters. https://icalendar.org/iCalendar-RFC-5545/3-1-content-lines.html states that lines longer than 75 characters should be folded.',
            'start': (2019, 10, 17, 10, 30, 0, -300),
            'duration': 600,
        },
    ]
