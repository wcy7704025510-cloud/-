#include "audio_read.h"

//数据量 40ms 640字节
//1s的数据量 640*25=16000 byte

//speex窄带模式 8kHz 20ms 采样点 160 每一个点是16位2进制(2字节)编码 操作320字节

Audio_Read::Audio_Read(QObject *parent) : QObject(parent)
{
    //首先初始化设备
    //声卡采样格式
    format.setSampleRate(8000);
    format.setChannelCount(1);
    format.setSampleSize(16);
    format.setCodec("audio/pcm");
    format.setByteOrder(QAudioFormat::LittleEndian);
    format.setSampleType(QAudioFormat::UnSignedInt);
    QAudioDeviceInfo info = QAudioDeviceInfo::defaultInputDevice();
    if (!info.isFormatSupported(format)) {
        QMessageBox::information(NULL , "提示", "打开音频设备失败");
        format = info.nearestFormat(format);
    }
    m_recordState = audio_state::_Stop;

    //编码器初始化
    //speex 初始化
    speex_bits_init(&bits_enc);
    Enc_State = speex_encoder_init(speex_lib_get_mode(SPEEX_MODEID_NB));

    //设置质量为 8(15kbps)
    int tmp = SPEEX_QUALITY;
    speex_encoder_ctl(Enc_State,SPEEX_SET_QUALITY,&tmp);

#ifdef USE_VAD
    //vad create
    if(0!=WebRtcVad_Create(&handle)){
        qDebug()<<"vad create fail";
    }
    //vad init
    if(0!=WebRtcVad_Init(handle)){
        qDebug()<<"vad init fail";
    }
#endif
}

Audio_Read::~Audio_Read()
{
    if(m_timer){
        m_timer->stop();
        delete m_timer;
        m_timer=nullptr;
    }
    if(audio_in)
    {
        audio_in->stop();
        delete audio_in;
        audio_in=nullptr;
    }
#ifdef USE_VAD
    WebRtcVad_Free(handle);
#endif
}

void Audio_Read::start()
{
    if( m_recordState != audio_state::_Record ){
        /* QAudioInput* */audio_in = new QAudioInput(format, this);
        /*QIODevice* */myBuffer_in = audio_in->start();//声音采集开始
        //通过定时器, 每次超时, 从缓冲区 m_buffer_in 中提取数据
        m_timer = new QTimer(this);
        connect( m_timer , &QTimer::timeout , this , &Audio_Read::slot_readMore );
        m_timer->start(20);     //采集音频时间间隔不能超过40ms
    }
    m_recordState = audio_state::_Record;
}

void Audio_Read::pause()
{
    if( m_recordState == audio_state::_Record ){
        //声音采集恢复, 可以将原 QAudioInput 对象回收
        if(audio_in)
        {
            audio_in->stop();
            delete audio_in;
            audio_in=nullptr;
        }
        m_timer->stop();
        delete m_timer;
        m_timer=nullptr;
    }
    m_recordState = audio_state::_Pause;
}


//压缩率：640->76
void Audio_Read::slot_readMore()
{
#ifndef USE_SPEEX

    if (!audio_in)
        return;

    QByteArray m_buffer(2048,0);
    qint64 len = audio_in->bytesReady();
    if (len < 640)
    {
        return;
    }
    //从音频设备对应的缓冲区中读取 640字节
    qint64 l = myBuffer_in->read(m_buffer.data(), 640);
    qDebug() << "l sizes:"<< l ;
    QByteArray frame;
    frame.append(m_buffer.data(),640);

    //以信号的形式发送
    Q_EMIT SIG_audioFrame( frame );
#else
    if (!audio_in)
        return;

    QByteArray m_buffer(2048,0);
    qint64 len = audio_in->bytesReady();
    if (len < 640)
    {
        return;
    }
    //从音频设备对应的缓冲区中读取 640字节
    qint64 l = myBuffer_in->read(m_buffer.data(), 640);
    qDebug() << "l sizes:"<< l ;

    //////////////////////////////////////////////////
    QByteArray frame;
//    frame.append(m_buffer.data(),640);
    char bytes[800] = {0};
    int i = 0;
    float input_frame1[320];
    char* pInData = (char*)m_buffer.data() ;
    char buf[800] = {0};
    memcpy( buf , pInData , 640);

    //静音检测 之后再编码
#ifdef USE_VAD
    char* bufferdata=(char*)m_buffer.data();
    //每次操作 320字节
    for(int i=0;i<640;i+=320){
        char* tmp=bufferdata+i;
        //返回值   -1:错误     0:静音      1:有声
        if(0==WebRtcVad_Process(handle,8000,(short*)tmp,160)){
            //静音    清零这片空间
            memset(tmp,0,320);
            qDebug()<<"静音";
        }
    }
#endif

    //speex 窄带模式 8kHz 每次编码 320字节 大端存储 数据
    //PCM 采集的是小端的
    int nbytes = 0;
    {
        //小端转化为大端-->float类型
        short num = 0;
        for (i = 0;i < 160;i++)
        {
            num = (uint8_t)buf[2 * i] | (((uint8_t)buf[2 * i + 1]) << 8);
            input_frame1[i] = num;
            //num = m_buffer[2 * i] | ((short)(m_buffer[2 * i + 1]) << 8);
            //qDebug() << "float in" << num << input_frame1[i];
        }
        //压缩数据
        speex_bits_reset(&bits_enc);
        speex_encode(Enc_State,input_frame1,&bits_enc);
        nbytes = speex_bits_write(&bits_enc,bytes,800);
        frame.append(bytes,nbytes);
        //小端转化为大端
        for (i = 0;i < 160;i++)
        {
            num = (uint8_t)buf[2 * i + 320] | (((uint8_t)buf[2 * i + 1 + 320]) << 8);
            input_frame1[i] = num;
        }
        //压缩数据
        speex_bits_reset(&bits_enc);
        speex_encode(Enc_State,input_frame1,&bits_enc);
        nbytes = speex_bits_write(&bits_enc,bytes,800);
        frame.append(bytes,nbytes);
        qDebug() << "nbytes = " << frame.size();
    }

    //以信号的形式发送
    Q_EMIT SIG_audioFrame( frame );
#endif
}
