#ifndef MAINWINDOW_HXX
#define MAINWINDOW_HXX

#include <QMainWindow>

#ifdef Q_OS_WIN
# include <windows.h>
#endif

#include "../firmware/requests.h"   /* custom request numbers */


class RTTTLPlayer;
namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

public slots:
    void setFrequency(int freq);
    void enableRadio();
    void disableRadio();
    void stopTone();
    void dimLed(int pwm);
    void disableLED();
    void playTone(int freq);

private slots:
    void on_txButton_pressed();
    void connectDevice() { connectDevice(true); }
    void connectDevice(bool showError);
    void openRtttlFile();
    void openRtttlFile(const QString &);
    void on_txButton_released();
    void on_ledTurnerButton_toggled(bool checked);
    void on_playButton_clicked();
    void on_playStopped();
    void on_rtttlBox_textChanged();
    void on_defaultSliderButton_clicked();
    void on_action_openRTTTL_triggered();

private:
    void usbSendCommand(enum USBControllCommand cmd, int value = 0);
    void dragEnterEvent(QDragEnterEvent *);
    void dropEvent(QDropEvent *);

    Ui::MainWindow *ui;
    RTTTLPlayer    *m_player;
    struct usb_dev_handle *m_usbHandle;
    quint16         m_radioFrequency;
    QAction        *m_openAction;
    QAction        *m_pasteAction;
    QAction        *m_clearAction;

};

#endif // MAINWINDOW_HXX
