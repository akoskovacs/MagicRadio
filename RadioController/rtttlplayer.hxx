#ifndef RTTTLPLAYER_HXX
#define RTTTLPLAYER_HXX

#include <QThread>
#include <QList>
#include <QVector>
#include <QTimer>
#include <QProgressBar>

struct Note {
    Note(unsigned int dur = 0, unsigned int freq = 0)
        : frequency(freq), duration(dur) {}

    unsigned int frequency;
    unsigned int duration;
};
255
class RTTTLPlayer : public QObject
{
    Q_OBJECT
public:
    explicit RTTTLPlayer(QObject *parent = 0);
    ~RTTTLPlayer();
    int setString(const QString &str);
    void run();
    void stopPlaying();
    void replay();
    bool isPlaying();

signals:
    void playTone(int frequency);
    void stopTone();
    void playStopped();
    void notePlayed(); /* for progress bar update */
    void updateProgress(int);

public slots:
    void changeSpeed(int);

private slots:
    void playNextNote();

private:
    QTimer                 m_timer;
    unsigned int           m_playIndex;
    double                 m_speed;
    QVector<Note>          m_noteList;
    QVectorIterator<Note> *m_noteIter;
};

#endif // RTTTLPLAYER_HXX
