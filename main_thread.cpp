#include "main_thread.h"
#include "btmanager/bt_test.h"
#include "bt_a2dp_sink.h"
#include "btmanager/include/bt_alsa.h"



void main_thread::calculatePowerSpectrum(const char* pcmData, int sampleRate, int N) {
    fftw_complex* in = (fftw_complex*)fftw_malloc(sizeof(fftw_complex) * N);
      fftw_complex* out = (fftw_complex*)fftw_malloc(sizeof(fftw_complex) * N);

      for (int i = 0; i < N; ++i) {
          short sample = pcmData[i * 2] | (pcmData[i * 2 + 1] << 8);
          in[i][0] = static_cast<double>(sample);
          in[i][1] = 0.0;
      }

      fftw_plan plan = fftw_plan_dft_1d(N, in, out, FFTW_FORWARD, FFTW_ESTIMATE);
      fftw_execute(plan);

      // 输出功率（dB单位）
      int aa =0;
      for (int i = 0; i < N / 2; ++i) {
          double real = out[i][0];
          double imag = out[i][1];
          double magnitude = std::sqrt(real * real + imag * imag);
          double power = magnitude * magnitude;
         // double dB = 10 * std::log10(power + 1e-10);  // 避免log10(0)

          double frequency = i * sampleRate / N;
         // qDebug() << "Frequency:" << frequency << "Hz, Power (dB):" << power;
         if (static_cast<int>(frequency) % 15  && frequency!=0)
          {
             if(power == 0)
             {
                   continue;
             }
            spectrumMeta[aa] =power*7;
           // qDebug() <<aa <<"frequency"<<frequency <<"spectrumMeta" <<spectrumMeta[aa];
            aa++;
            if(aa >61)
            {
                fftw_destroy_plan(plan);
                fftw_free(in);
                fftw_free(out);
                return;
            }
          }
      }

      fftw_destroy_plan(plan);
      fftw_free(in);
      fftw_free(out);
}

main_thread::main_thread()
{
    // manager = new QNetworkConfigurationManager(this);

    // // 当网络配置改变时进行响应
    // connect(manager, &QNetworkConfigurationManager::configurationChanged, this, &main_thread::checkNetworkStatus);
    // connect(manager, &QNetworkConfigurationManager::onlineStateChanged, this, &main_thread::checkNetworkStatus);

}
#define  PULLUP_SD    do{ \
                        usleep(500*1000); \
                        system("echo 1 > /proc/rp_gpio/output_sd"); \
                        printf("功放打开\n"); \
                    } while(0)

#define  PULLDOWN_SD    do{ \
                        system("echo 0 > /proc/rp_gpio/output_sd"); \
                        printf("功放关闭\n"); \
                    } while(0)
void main_thread::run()
{
   // int SDoffCnt=100;

    while(1)
    {
        if (occurredFlag)
        {

         // PULLDOWN_SD;
          PULLUP_SD;
          occurredFlag =  0;
        }



        if(trackUpdate>0)
        {
            trackUpdate= 0;
            emit updateAudioTrack();
        }
       // qDebug() << "231";
        SpectrumMetaData();
        //qDebug() << "456" <<GetGpioStatus("/proc/rp_gpio/output_lock");;

     //   usleep(1000*1);
    }
}

bool main_thread::isConnected()
{
  //  return manager->isOnline();
}

int main_thread::GetGpioStatus(QString GPIO_fILE)
{
    QString filePath =GPIO_fILE;

    // 创建 QFile 对象并传入路径
    QFile file(filePath);

    // 尝试打开文件
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
     //   qDebug() << "无法打开文件:" << file.errorString();
        return -1;
    }

    QString get_data = file.readAll();
    file.close();

    return get_data.toInt(NULL,10);
}

void main_thread::SpectrumMetaData()
{
    // 假设采样率为44100Hz，并且我们有一段PCM数据（示例：1秒钟，44100个样本）
    int sampleRate = 44100;
    int N = 44100;  // 1秒钟的PCM数据，假设为44100Hz采样率

    // 示例：生成一个简单的100Hz正弦波作为PCM数据
  /*  std::vector<short> pcmData(N);  // PCM数据存储为16位短整型数据
    for (int i = 0; i < N; ++i) {
        pcmData[i] = static_cast<short>(std::sin(2 * M_PI * 100 * i / sampleRate) * 32767);  // 100Hz正弦波
    }

    // 将std::vector<short>转换为char*
    char* pcmDataChar = reinterpret_cast<char*>(pcmData.data());
*/
    // 调用计算功率谱函数
   // qDebug() << "pcm_lenth "<<pcm_lenth;
    if(pcm_lenth >0 && volume_scale >0)
    {
      /*  for(int i =0;i<pcm_lenth;i++)
        {
            printf(" %02x" , pcm_data[i]);
        }
        printf("\n");*/
        calculatePowerSpectrum((const char *)pcm_data, sampleRate, pcm_lenth);
    }
}

#include <QElapsedTimer>  // 引入 QElapsedTimer

void main_thread::checkNetworkStatus()//drop
{
    QElapsedTimer timer;  // 创建计时器
    timer.start();  // 启动计时器

    if (isConnected() && isNetOk == false)
    {
        qDebug() << "Device is connected to the network.";
        emit wlanConnected();
        isNetOk = true;
        qDebug() << "12312313123123123";
    }
    else
    {
        qDebug() << "Device is not connected to the network.";
        isNetOk = false;
    }

    qint64 elapsed = timer.elapsed();  // 获取从启动到现在的时间（毫秒）
    qDebug() << "Network check finished, cost" << elapsed * 1000 << "us";  // 打印花费的时间，转换为微秒
}
