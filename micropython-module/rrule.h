#ifndef mp_rrule_h
#define mp_rrule_h

#ifdef __cplusplus
extern "C" {
#endif

    using rrule_p = void*;

    rrule_p RRule_new();
    void RRule_del(rrule_p p);

    bool RRule_next(rrule_p p);
    void RRule_now(rrule_p p);

#ifdef __cplusplus
}
#endif

#endif
