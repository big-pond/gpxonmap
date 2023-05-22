#ifndef LTIME_H
#define LTIME_H

class QDateTime;

class LTime
{
public:
    static int local_time_offset(QDateTime& dt, int index);
    /* Returns the offset of Moscow time relative to UTC time. */
    static int msk_time_offset(const QDateTime &dt);
    /* Returns the offset of Yekaterinburg time relative to UTC time. */
    static int ekb_time_offset(const QDateTime &dt);
};

#endif // LTIME_H
