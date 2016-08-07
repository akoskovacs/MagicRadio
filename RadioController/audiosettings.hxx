#ifndef AUDIOSETTINGS_HXX
#define AUDIOSETTINGS_HXX

#include <QDialog>

namespace Ui {
class AudioSettings;
}

class AudioSettings : public QDialog
{
    Q_OBJECT

public:
    explicit AudioSettings(QWidget *parent = 0);
    ~AudioSettings();

private slots:
    void deviceChanged(int);
    void volumeChanged(int);

private:
    Ui::AudioSettings *ui;
};

#endif // AUDIOSETTINGS_HXX
