#include "audiosettings.hxx"
#include "ui_audiosettings.h"

#include <QAudioDeviceInfo>

AudioSettings::AudioSettings(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AudioSettings)
{
    ui->setupUi(this);
    const QAudioDeviceInfo &defaultDevice = QAudioDeviceInfo::defaultOutputDevice();
    ui->deviceBox->addItem(defaultDevice.deviceName(), qVariantFromValue(defaultDevice));
    foreach (const QAudioDeviceInfo &deviceInfo, QAudioDeviceInfo::availableDevices(QAudio::AudioOutput)) {
        if (deviceInfo != defaultDevice) {
            ui->deviceBox->addItem(deviceInfo.deviceName(), qVariantFromValue(deviceInfo));
        }
    }
    connect(ui->deviceBox, SIGNAL(activated(int)), SLOT(deviceChanged(int)));
    connect(ui->volumeBox, SIGNAL(valueChanged(int)), this, SLOT(volumeChanged(int)));
}

void AudioSettings::deviceChanged(int index)
{
#if 0
    m_pullTimer->stop();
    m_generator->stop();
    m_audioOutput->stop();
    m_device = ui->deviceBox->itemData(index).value<QAudioDeviceInfo>();
    createAudioOutput();
    m_audioOutput->disconnect(this);
#endif
}

void AudioSettings::volumeChanged(int value)
{
#if 0
    if (m_audioOutput)
        m_audioOutput->setVolume(qreal(value/100.0f));
#endif
}

AudioSettings::~AudioSettings()
{
    delete ui;
}
#if 0
AudioTest::AudioTest()
    :   m_pullTimer(new QTimer(this))
    ,   m_modeButton(0)
    ,   m_suspendResumeButton(0)
    ,   m_deviceBox(0)
    ,   m_device(QAudioDeviceInfo::defaultOutputDevice())
    ,   m_generator(0)
    ,   m_audioOutput(0)
    ,   m_output(0)
    ,   m_buffer(BufferSize, 0)
{
    initializeWindow();
    initializeAudio();
}

void AudioTest::initializeAudio()
{
    connect(m_pullTimer, SIGNAL(timeout()), SLOT(pullTimerExpired()));

    m_pullMode = true;

    m_format.setSampleRate(DataSampleRateHz);
    m_format.setChannelCount(1);
    m_format.setSampleSize(16);
    m_format.setCodec("audio/pcm");
    m_format.setByteOrder(QAudioFormat::LittleEndian);
    m_format.setSampleType(QAudioFormat::SignedInt);

    QAudioDeviceInfo info(QAudioDeviceInfo::defaultOutputDevice());
    if (!info.isFormatSupported(m_format)) {
        qWarning() << "Default format not supported - trying to use nearest";
        m_format = info.nearestFormat(m_format);
    }

    m_generator = new Generator(m_format, DurationSeconds*1000000, ToneSampleRateHz, this);

    createAudioOutput();
}

void AudioTest::createAudioOutput()
{
    delete m_audioOutput;
    m_audioOutput = 0;
    m_audioOutput = new QAudioOutput(m_device, m_format, this);
    m_generator->start();
    m_audioOutput->start(m_generator);
    m_volumeSlider->setValue(int(m_audioOutput->volume()*100.0f));
}

AudioTest::~AudioTest()
{

}

void AudioTest::pullTimerExpired()
{
    if (m_audioOutput && m_audioOutput->state() != QAudio::StoppedState) {
        int chunks = m_audioOutput->bytesFree()/m_audioOutput->periodSize();
        while (chunks) {
           const qint64 len = m_generator->read(m_buffer.data(), m_audioOutput->periodSize());
           if (len)
               m_output->write(m_buffer.data(), len);
           if (len != m_audioOutput->periodSize())
               break;
           --chunks;
        }
    }
}

void AudioTest::toggleMode()
{
    m_pullTimer->stop();
    m_audioOutput->stop();

    if (m_pullMode) {
        m_modeButton->setText(tr(PULL_MODE_LABEL));
        m_output = m_audioOutput->start();
        m_pullMode = false;
        m_pullTimer->start(20);
    } else {
        m_modeButton->setText(tr(PUSH_MODE_LABEL));
        m_pullMode = true;
        m_audioOutput->start(m_generator);
    }

    m_suspendResumeButton->setText(tr(SUSPEND_LABEL));
}

void AudioTest::toggleSuspendResume()
{
    if (m_audioOutput->state() == QAudio::SuspendedState) {
        m_audioOutput->resume();
        m_suspendResumeButton->setText(tr(SUSPEND_LABEL));
    } else if (m_audioOutput->state() == QAudio::ActiveState) {
        m_audioOutput->suspend();
        m_suspendResumeButton->setText(tr(RESUME_LABEL));
    } else if (m_audioOutput->state() == QAudio::StoppedState) {
        m_audioOutput->resume();
        m_suspendResumeButton->setText(tr(SUSPEND_LABEL));
    } else if (m_audioOutput->state() == QAudio::IdleState) {
        // no-op
    }
}

#endif
