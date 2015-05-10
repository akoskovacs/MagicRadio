#include "mainwindow.hxx"
#include "ui_mainwindow.h"
#include "rtttlplayer.hxx"

#include "opendevice.h" /* common code moved to separate module */
#include "../firmware/usbconfig.h"  /* device's VID/PID and names */

#include <QFileDialog>
#include <QFile>
#include <QMessageBox>
#include <QDebug>
#include <QMimeData>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    m_usbHandle = 0;
    m_player = 0;
    usb_init();
    connectDevice(false);

    m_openAction  = new QAction(this);
    m_openAction->setText(tr("RTTTL megnyitása"));
    m_openAction->setIcon(QIcon::fromTheme("document-open"));

    m_pasteAction = new QAction(this);
    m_pasteAction->setText(tr("RTTTL beillesztése"));
    m_pasteAction->setIcon(QIcon::fromTheme("edit-paste"));

    m_clearAction = new QAction(this);
    m_clearAction->setText(tr("RTTTL törlése"));
    m_clearAction->setIcon(QIcon::fromTheme("edit-clear"));
    m_clearAction->setEnabled(false);

    ui->toolBar->addAction(m_openAction);
    ui->toolBar->addSeparator();
    ui->toolBar->addAction(m_pasteAction);
    ui->toolBar->addAction(m_clearAction);

    m_player = new RTTTLPlayer(this);
    connect(m_player, &RTTTLPlayer::playTone, this, &MainWindow::playTone);
    connect(m_player, &RTTTLPlayer::stopTone, this, &MainWindow::stopTone);
    connect(m_player, &RTTTLPlayer::playStopped, this, &MainWindow::on_playStopped);
    connect(ui->speedSlider, SIGNAL(sliderMoved(int)), m_player, SLOT(changeSpeed(int)));
    connect(m_player, &RTTTLPlayer::updateProgress, ui->playProgress, &QProgressBar::setValue);
    connect(m_openAction, SIGNAL(triggered()), this, SLOT(openRtttlFile()));
    connect(m_pasteAction, &QAction::triggered, ui->rtttlBox, &QPlainTextEdit::paste);
    connect(m_clearAction, &QAction::triggered, ui->rtttlBox, &QPlainTextEdit::clear);
    setAcceptDrops(true);
}

MainWindow::~MainWindow()
{
    disableRadio();
    if (m_usbHandle != 0) {
        usb_close(m_usbHandle);
    }
    delete m_player;
    delete ui;
}

void MainWindow::openRtttlFile(const QString &fname)
{
    QFile rtttlFile(fname);
    if (rtttlFile.open(QIODevice::ReadOnly)) {
        QTextStream in(&rtttlFile);
        ui->rtttlBox->clear();
        ui->rtttlBox->setPlainText(in.readAll());
    }
}

void MainWindow::openRtttlFile()
{
   QString fname = QFileDialog::getOpenFileName(this, tr("RTTTL fájl megnyitása")
                                                , QString(), tr("RTTTL (*.txt *.rtttl);;Bármi más (*.*)"));
   if (!fname.isEmpty()) {
        openRtttlFile(fname);
   }
}

void MainWindow::on_txButton_pressed()
{
    enableRadio();
}

void MainWindow::on_txButton_released()
{
    disableRadio();
}

void MainWindow::connectDevice(bool showError)
{
    const unsigned char rawVid[2] = {USB_CFG_VENDOR_ID}, rawPid[2] = {USB_CFG_DEVICE_ID};
    char vendor[] = {USB_CFG_VENDOR_NAME, 0}, product[] = {USB_CFG_DEVICE_NAME, 0};
    int vid, pid;

    if (m_usbHandle != 0)
        return;

    vid = rawVid[1] * 256 + rawVid[0];
    pid = rawPid[1] * 256 + rawPid[0];
    if (usbOpenDevice(&m_usbHandle, vid, vendor, pid, product, NULL, NULL, NULL) != 0) {
        if (showError) {
            QMessageBox::critical(this, tr("Eszköz információ"), tr("Nem sikerült csatlakozni az eszközhöz"));
        }
        statusBar()->showMessage(tr("Nincs csatlakoztatott eszköz"));
        m_usbHandle = 0;
        return;
    }
    statusBar()->showMessage(tr("Az eszköz sikeresen csatlakoztatva"));
}

void MainWindow::setFrequency(int freq)
{
    m_radioFrequency = freq;
}

void MainWindow::dragEnterEvent(QDragEnterEvent *e)
{
    if (e->mimeData()->hasUrls()) {
        e->acceptProposedAction();
    }
}

void MainWindow::dropEvent(QDropEvent *e)
{
    ui->rtttlBox->clear();
    //qDebug() << e->mimeData()->urls().first().toLocalFile();
    openRtttlFile(e->mimeData()->urls().first().toLocalFile());
}

void MainWindow::usbSendCommand(enum USBControllCommand cmd, int value)
{
    int cnt;
    char errcode;
    if (m_usbHandle == 0)
        return;

    cnt = usb_control_msg(m_usbHandle, USB_TYPE_VENDOR | USB_RECIP_DEVICE | USB_ENDPOINT_IN
                          , cmd, value, 0, &errcode, sizeof(errcode), 5000);

    if (cnt < 0 || errcode != 0) {
        statusBar()->showMessage(tr("Hiba az USB kapcsolatban: %1").arg(usb_strerror()));
        return;
    }
}

void MainWindow::enableRadio()
{
    usbSendCommand(CMD_TX_ON, m_radioFrequency);
}

void MainWindow::disableRadio()
{
    usbSendCommand(CMD_TX_OFF);
}

void MainWindow::dimLed(int pwm)
{
    usbSendCommand(CMD_LED_ON, pwm);
}

void MainWindow::disableLED()
{
    usbSendCommand(CMD_LED_OFF);
}

void MainWindow::on_ledTurnerButton_toggled(bool checked)
{
    if (checked) {
        ui->ledTurnerButton->setText(tr("LED bekapcsolása"));
        disableLED();
    } else {
        ui->ledTurnerButton->setText(tr("LED kikapcsolása"));
        dimLed(255);
    }
}

void MainWindow::playTone(int freq)
{
    // qDebug() << "frequency:" << freq;
    ui->freqBox->setValue(freq);
    setFrequency(freq);
    enableRadio();
    if (ui->flashLed->isChecked()) {
        dimLed(128);
    }
}

void MainWindow::stopTone()
{
    disableRadio();
    if (ui->flashLed->isChecked()) {
        disableLED();
    }
}

void MainWindow::on_playButton_clicked()
{
    if (m_player == 0) {
        return;
    }
    if (m_player->isPlaying()) {
        m_player->stopPlaying();
        return;
    }
    m_player->run();
    ui->playButton->setText(tr("Megállítás"));
}

void MainWindow::on_playStopped()
{
    ui->playButton->setText(tr("Lejátszás"));
}

void MainWindow::on_rtttlBox_textChanged()
{
    QString str = ui->rtttlBox->toPlainText();
    if (str.isEmpty() || str.isNull()) {
        ui->playButton->setEnabled(false);
        m_clearAction->setEnabled(false);
        return;
    }
    ui->playButton->setEnabled(true);
    m_clearAction->setEnabled(true);

    int max = m_player->setString(str);
    if (max == -1) {
        statusBar()->showMessage(tr("Érvénytelen RTTTL sztring!"));
        return;
    }
    ui->playProgress->setMaximum(max);
}

void MainWindow::on_defaultSliderButton_clicked()
{
    ui->speedSlider->setValue(10);

}

void MainWindow::on_action_openRTTTL_triggered()
{
    openRtttlFile();
}
