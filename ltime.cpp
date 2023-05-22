#include "ltime.h"
#include <QDateTime>

#include <QDebug>

int LTime::local_time_offset(QDateTime &dt, int index)
{
    int offset = 0;
    switch(index)
    {
    case 0: offset = msk_time_offset(dt); break;
    case 1: offset = ekb_time_offset(dt); break;
    }
    return offset;
}

int LTime::msk_time_offset(const QDateTime &dt)
{
    int offset = 0;
    QDateTime d201410  = QDateTime(QDate(2014, 10, 26), QTime(2,0)); // d201410 = datetime(2014, 10, 26, 2)
    QDateTime d201103 = QDateTime(QDate(2011, 3, 26), QTime(2,0)); //datetime(2011, 3, 27, 2)
    QDateTime d193006 = QDateTime(QDate(1930, 6, 16), QTime(0,0)); //datetime(1930, 6, 16)
    if (dt >= d201410)
        offset = 3;
    else if ((d201103 < dt) && (dt < d201410))
        offset = 4;
    else if ((1996 <= dt.date().year()) && (dt.date().year() <= 2010))
    {
        QDate autum_date = QDate(dt.date().year(), 10, 31);
        while (autum_date.dayOfWeek()!=7) autum_date = autum_date.addDays(-1);
        QDate spring_date = QDate(dt.date().year(), 3, 31);
        while (spring_date.dayOfWeek()!=7) spring_date = spring_date.addDays(-1);
        QDateTime dt_autumn = QDateTime(autum_date, QTime(2,0));
        QDateTime dt_spring = QDateTime(spring_date, QTime(2,0));
        qDebug() << dt_spring << dt_autumn;
        offset = 3;
        if ((dt_spring < dt) && (dt < dt_autumn))
            offset = 4;
    }
    else if ((1985 <= dt.date().year()) && (dt.date().year() <= 1995))
    {
        QDate autum_date = QDate(dt.date().year(), 9, 30);
        while (autum_date.dayOfWeek()!=7) autum_date = autum_date.addDays(-1);
        QDate spring_date = QDate(dt.date().year(), 3, 31);
        while (spring_date.dayOfWeek()!=7) spring_date = spring_date.addDays(-1);
        QDateTime dt_autumn = QDateTime(autum_date, QTime(2,0));
        QDateTime dt_spring = QDateTime(spring_date, QTime(2,0));
        offset = 3;
        if ((dt_spring < dt) && (dt < dt_autumn))
            offset = 4;
    }
    else if (dt.date().year() == 1984)
    {
        QDateTime dt_autumn = QDateTime(QDate(1984,9,30), QTime(0,0));
        QDateTime dt_spring = QDateTime(QDate(1984,4, 1), QTime(0,0));
        offset = 3;
        if ((dt_spring < dt) && (dt < dt_autumn))
            offset = 4;
    }
    else if ((1981 <= dt.date().year()) && (dt.date().year() <= 1983))
    {
        QDateTime dt_autumn = QDateTime(QDate(1984,10, 1), QTime(0,0));
        QDateTime dt_spring = QDateTime(QDate(1984, 4, 1), QTime(0,0));
        offset = 3;
        if ((dt_spring < dt) && (dt < dt_autumn))
            offset = 4;
    }
    else if ((d193006 <= dt) && (dt.date().year() <= 1981))
        offset = 3;
    return offset;
}

int LTime::ekb_time_offset(const QDateTime &dt)
{
    return msk_time_offset(dt) + 2;
}
