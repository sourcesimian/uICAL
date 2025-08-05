
def load_ical_with_injected_event(base_ical, event_str):
    cal_end = base_ical.find("END:VCALENDAR")

    return base_ical[:cal_end] + event_str + base_ical[cal_end:]


def get_tzid_datetime_offset(timezones_ical_str, tzid_datetime):
    # The tzmap is not accessible in python, so load an icalendar with timezone info, then inject a fake event with a datetime we would like to get the offset of.
    # Input example: (ical, "TZID=America/New_York:19970902T090000") => "-0300"
    import uICAL

    combined_ical = load_ical_with_injected_event(timezones_ical_str, """BEGIN:VEVENT
DTSTART;""" + tzid_datetime + """
DTEND;""" + tzid_datetime + """
DTSTAMP:20191105T175555Z
CREATED:20190819T210313Z
DESCRIPTION:
LAST-MODIFIED:20190914T102748Z
LOCATION:
SEQUENCE:3
STATUS:CONFIRMED
SUMMARY:testEvent
TRANSP:OPAQUE
END:VEVENT
""")

    cal = uICAL.Calendar(combined_ical)
    cal.next()
    offset = cal.current()["start"][6]

    return f"{offset:+05d}" 
