
#ifndef AUDIO_DECODE_PROCESSOR_H
#define AUDIO_DECODE_PROCESSOR_H

#include <QObject>
#include <QByteArray>
#include <QString>

#include <opus/opus.h>

class AudioDecodeDataQueue;
class PCMPlayQueue;

class AudioDecodeProcessor : public QObject
{
    Q_OBJECT
public:
    explicit AudioDecodeProcessor(QObject *parent = nullptr);
    ~AudioDecodeProcessor();

    void setInputQueue(AudioDecodeDataQueue* queue);
    void setOutputQueue(PCMPlayQueue* queue);

signals:
    void SIG_decodeError(const QString& error);

public slots:
    void slot_start();
    void slot_stop();

private:
    void processLoop();

private:
    AudioDecodeDataQueue* m_inputQueue;
    PCMPlayQueue* m_outputQueue;
    OpusDecoder* m_decoder;
    bool m_running;
};

#endif
