#ifndef AUDIOWORKER_H
#define AUDIOWORKER_H

#include <QObject>
#include <QThread>

class CMediaManager;

// 发送音频帧线程类声明
// （待扩展封装 Speex 压缩与无限循环）
class AudioWorker : public QObject {
    Q_OBJECT
public:
    explicit AudioWorker(CMediaManager* manager = nullptr);
    ~AudioWorker();

public slots:
    void slot_dowork(char* buf, int len);

private:
    QThread* m_pThread;
    CMediaManager* m_manager;
};

#endif // AUDIOWORKER_H