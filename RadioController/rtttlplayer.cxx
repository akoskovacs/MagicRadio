#include "rtttlplayer.hxx"
#include "rtttl_notes.hxx"
#include <QDebug>
#include <QListIterator>
#include <QProcess>
#include <QRegularExpression>

#define DEBUG_ON_HOST 0

namespace {
    const int notes[] = { 0, NOTE_C4, NOTE_CS4, NOTE_D4, NOTE_DS4, NOTE_E4, NOTE_F4, NOTE_FS4
                          , NOTE_G4, NOTE_GS4, NOTE_A4, NOTE_AS4, NOTE_B4, NOTE_C5, NOTE_CS5
                          , NOTE_D5, NOTE_DS5, NOTE_E5, NOTE_F5, NOTE_FS5, NOTE_G5, NOTE_GS5
                          , NOTE_A5, NOTE_AS5, NOTE_B5, NOTE_C6, NOTE_CS6, NOTE_D6, NOTE_DS6
                          , NOTE_E6, NOTE_F6, NOTE_FS6, NOTE_G6, NOTE_GS6, NOTE_A6, NOTE_AS6
                          , NOTE_B6, NOTE_C7, NOTE_CS7, NOTE_D7, NOTE_DS7, NOTE_E7, NOTE_F7
                          , NOTE_FS7, NOTE_G7, NOTE_GS7, NOTE_A7, NOTE_AS7, NOTE_B7 };
#define NOTE_COUNT (int)(sizeof(notes)/sizeof(int))
}

RTTTLPlayer::RTTTLPlayer(QObject *parent) :
    QObject(parent), m_speed(1), m_noteIter(0)
{
    connect(&m_timer, SIGNAL(timeout()), this, SLOT(playNextNote()));
}

RTTTLPlayer::~RTTTLPlayer()
{
    delete m_noteIter;
}

int RTTTLPlayer::setString(const QString &str)
{
    // Absolutely no error checking in h
    int default_dur = 4;
    int default_oct = 6;
    int bpm = 63;
    long wholenote;
    long duration;
    int note;
    int scale;

    m_noteList.clear();
    QStringList main = str.split(':');
    if (main.count() < 3) {
        qDebug() << "Only has" << main.count() << "parts";
        return -1;
    }

    /* Skip tone name */
    QString header = main.at(1);
    QStringList hdrlist = header.split(',');
    QStringListIterator it(hdrlist);
    QRegularExpression hdrex("(\\w)\\s*=\\s*(\\d+)");
    while (it.hasNext()) {
        QRegularExpressionMatch match = hdrex.match(it.next());
        if (match.hasMatch()) {
            char ch = match.captured(1)[0].toLatin1();
            switch (ch) {
            case 'd':
                default_dur = match.captured(2).toInt();
                break;

            case 'o':
                default_oct = match.captured(2).toInt();
                break;

            case 'b':
                bpm = match.captured(2).toInt();
                break;
            }
        } else {
            qDebug() << "Cannot match on header:" << it.peekPrevious();
        }
    }

    QString tones = main.at(2);
    QStringList tonelist = tones.split(',');
    QStringListIterator toit(tonelist);
    QRegularExpression torex("(\\d*)(\\w)(\\#)?(\\.)?(\\d*)");

    wholenote = (60 * 1000L / bpm) * 4;  // this is the time for whole note (in milliseconds)
    while (toit.hasNext()) {
        QString cnote = toit.next();
        QRegularExpressionMatch match = torex.match(cnote);
        // BPM usually expresses the number of quarter notes per minute

        if (match.hasMatch()) {
            /* Get duration, if any */
            int num = match.captured(1).toInt();
            if (num) {
                duration = wholenote / num;
            } else {
                duration = wholenote / default_dur;
            }

            note = 0;
            if (!match.captured(2).isNull()) {
                char ch = match.captured(2)[0].toLatin1();
                switch (ch) {
                case 'c':
                    note = 1;
                    break;
                case 'd':
                    note = 3;
                    break;
                case 'e':
                    note = 5;
                    break;
                case 'f':
                    note = 6;
                    break;
                case 'g':
                    note = 8;
                    break;
                case 'a':
                    note = 10;
                    break;

                case 'b':
                    note = 12;
                    break;

                case 'p':
                default:
                    break;
                }
            }

            /* Has '#' ? */
            if (!match.captured(3).isNull()) {
                note++;
            }

            /* Has '.' ? */
            if (!match.captured(4).isNull()) {
                duration += duration/2;
            }

            num = match.captured(5).toInt();
            if (num) {
                scale = num;
            } else {
                scale = default_oct;
            }

            scale += OCTAVE_OFFSET;

            /* Add the calculated note to the list */
            Note n;
            if (note) {
                int ind = (scale - 4) * 12 + note;
                if (ind > 0 && ind < NOTE_COUNT) {
                    n.frequency = notes[ind];
                } else {
                    qDebug() << "Bad index for note" << ind;
                    return -1;
                }
            }
            n.duration  = duration;
            m_noteList.append(n);
        } else {
            qDebug() << toit.peekPrevious() << "not matches";
        }
    }
    if (m_noteIter != 0) {
        delete m_noteIter;
    }
    m_noteIter = new QVectorIterator<Note>(m_noteList);
    m_timer.setSingleShot(true);
    return m_noteList.count();
}

void RTTTLPlayer::playNextNote()
{
    Note n;
#if !DEBUG_ON_HOST
    emit stopTone();
#endif
    if (m_noteIter != 0) {
        if (m_noteIter->hasNext()) {
           m_playIndex++;
           emit updateProgress(m_playIndex);
           n = m_noteIter->next();
           if (n.frequency != 0) {
#if DEBUG_ON_HOST
               QProcess::startDetached(QString("padsp tones %1 %2")
                                 .arg(n.duration*m_speed).arg(n.frequency));
#else
               emit playTone(n.frequency);
#endif
           }
           m_timer.start(n.duration*m_speed);
        } else {
            m_playIndex = 0;
            m_noteIter->toFront();
            emit playStopped();
        }
    }
}

void RTTTLPlayer::run()
{
    m_playIndex = 0;
    m_noteIter->toFront();
    playNextNote();
}

void RTTTLPlayer::stopPlaying()
{
    if (m_timer.isActive()) {
        m_timer.stop();
    }
    if (m_noteIter != 0) {
        m_noteIter->toFront();
    }
    m_playIndex = 0;
    emit stopTone();
    emit playStopped();
}

void RTTTLPlayer::replay()
{
    stopPlaying();
    playNextNote();
}

bool RTTTLPlayer::isPlaying()
{
    return m_timer.isActive();
}

void RTTTLPlayer::changeSpeed(int n)
{
    m_speed = 10.0/n;
}
