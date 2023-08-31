#include "video.h"
#include "utils.h"

#include <string>

/**
 * 功能：使用V4L2采集UVC摄像头数据
 * 日期：2018.3.26
 * 用法：在Ubuntu下的Qt工程中添加V4L2.h和V4L2.cpp文件，定义一个V4L2对象，
 *      调用bool V4L2::V4l_Init(char* camera_path, unsigned int frame) 函数对摄像头进行初始化操作
 *      调用QImage V4L2::getImage() 函数就可以获得摄像头传来的一张图片
 *      调用bool closeCamera(void)函数关闭摄像头
 * 注意：需要在V4L2.cpp的54行左右可修改摄像头图片的输出格式，如修改为MJPEG输出
 *      在V4L2.h的宏定义中可以修改输出图片的像素
 *
 * v1.1-2018.9.26:修改注释风格，规范代码
 * v1.2-2019.3.2:优化getImage函数,减少数据转移次数
*/

namespace {

QByteArray v4l2_fourcc2s(quint32 fourcc)
{
    QByteArray buf(5, '\0');
    buf[0] = fourcc & 0x7f;
    buf[1] = (fourcc >> 8) & 0x7f;
    buf[2] = (fourcc >> 16) & 0x7f;
    buf[3] = (fourcc >> 24) & 0x7f;
    if (fourcc & (1 << 31)) {
        buf[4] = '-';
        buf[5] = 'B';
        buf[6] = 'E';
        buf[7] = '\0';
    } else {
        buf[4] = '\0';
    }
    return buf;
}

}


V4L2::V4L2()
{

}

V4L2::~V4L2()
{

}

// Чтобы инициализировать камеру, вам необходимо передать путь монтирования камеры и частоту кадров на выходе и вернуть true, если инициализация прошла успешно.
bool V4L2::init(QString cameraPath, quint32 fps, quint32 width, quint32 height)
{
    if ((fd=open(cameraPath.toStdString().c_str(), O_RDWR)) == -1)           // Режим чтения и записи для открытия камеры
    {
        qd() << "Error opening V4L interface";
        return false;
    }

    if (ioctl(fd, VIDIOC_QUERYCAP, &cap) == -1)        // Проверьте информацию о камере
    {
        qd() << "Error opening device " << cameraPath << ": unable to query device.";
        return false;
    }
    else                                              // Распечатать информацию о камере
    {
        qd() << "driver:\t\t"<<QString::fromLatin1((char *)cap.driver);     // имя водителя
        qd() << "card:\t\t"<<QString::fromLatin1((char *)cap.card);         // Имя устройства
        qd() << "bus_info:\t\t"<<QString::fromLatin1((char *)cap.bus_info); // Расположение магазина в системе Bus
        qd() << "version:\t\t"<<cap.version;                                // driver Версия
        qd() << "capabilities:\t"<<cap.capabilities;                        // набор возможностей, обычно：V4L2_CAP_VIDEO_CAPTURE | V4L2_CAP_STREAMING
    }

    getFormatInfo();

    if (!setFormat(width, height))
        return false;

    if (!setFps(fps))
        return false;

    req.count=Video_count;                              // 3 тайника
    req.type=V4L2_BUF_TYPE_VIDEO_CAPTURE;               // Соответствует типу в структуре v4l2_format.
    req.memory=V4L2_MEMORY_MMAP;                        // В режиме отображения памяти поле счетчика действительно только в том случае, если для него установлено значение V4L2_MEMORY_MMAP.
    if(ioctl(fd,VIDIOC_REQBUFS,&req)==-1)               // Применить видеокэш к ядру
    {
        qd()<<"request for buffers error";
        return false;
    }

    for (i=0; i<Video_count; i++)                       // mmap четыре буфера
    {
        bzero(&buffer[i], sizeof(buffer[i]));
        buffer[i].type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        buffer[i].memory = V4L2_MEMORY_MMAP;
        buffer[i].index = i;
        if (ioctl (fd, VIDIOC_QUERYBUF, &buffer[i]) == -1){ // Получите параметры буфера, эти параметры необходимы для mmap
            qd()<<"query buffer error";
            return false;
        }
        length[i] = buffer[i].length;                   // сохранить размер кэша
        start[i] = (unsigned char *)mmap(NULL,buffer[i].length,PROT_READ |PROT_WRITE, MAP_SHARED, fd, buffer[i].m.offset);
        // Сопоставьте запрошенный кеш ядра с пространством пользователя.
    }
    for (i=0; i<Video_count; i++){
        buffer[i].index = i;
        ioctl(fd, VIDIOC_QBUF, &buffer[i]);             // поставить в очередь кеш
    }
    type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    ioctl (fd, VIDIOC_STREAMON, &type);                 // Открыть поток ввода-вывода

    bzero(&v4lbuf, sizeof(v4lbuf));
    v4lbuf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    v4lbuf.memory = V4L2_MEMORY_MMAP;
    qd()<<"init ok";

    return true;
}

bool V4L2::setFps(quint32 fps)
{
    v4l2_streamparm setfps;            // Эта структура используется при настройке частоты кадров.
    setfps.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    setfps.parm.capture.timeperframe.denominator = fps;  // ожидаемая частота кадров
    setfps.parm.capture.timeperframe.numerator = 1;      // fps=frame/1

    if (ioctl(fd, VIDIOC_S_PARM, &setfps) == -1)           // Установите количество кадров
    {
        qd() << "Unable to set fps";
        return false;
    }

    if (ioctl(fd, VIDIOC_G_PARM, &setfps) == -1)           // Перечитайте структуру, чтобы убедиться, что настройка завершена.
    {
        qd()<<"Unable to get fps";
        return false;
    }

    qd()<<"fps:\t\t"<<setfps.parm.capture.timeperframe.denominator/setfps.parm.capture.timeperframe.numerator;// частота кадров на выходе

    return true;
}

// Используется для установки формата пикселей. Формат вывода изображения. Размер изображения. Метод сканирования.
bool V4L2::setFormat(quint32 width, quint32 height)
{
    v4l2_format fmt;
    fmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;            // формат пикселей
    fmt.fmt.pix.pixelformat = V4L2_PIX_FMT_MJPEG;      // Вывод в формате JPEG
    fmt.fmt.pix.height = height;                       // Размер изображения. Установите здесь ширину и высоту изображения, которое вы хотите вывести.
    fmt.fmt.pix.width = width;
    fmt.fmt.pix.field = V4L2_FIELD_INTERLACED;         // Метод передачи видеокадра, чересстрочный

    if (ioctl(fd, VIDIOC_S_FMT, &fmt) == -1)           // Настройте формат захвата камеры
    {
        qd() << "Unable to set format";
        return false;
    }

    if(ioctl(fd, VIDIOC_G_FMT, &fmt) == -1)            // Перечитайте структуру, чтобы убедиться, что настройка завершена.
    {
        qd() << "Unable to get format";
        return false;
    }

    qd() << "fmt.type:\t\t"<<fmt.type;            // формат выходных пикселей
    qd() << "pix.height:\t"<<fmt.fmt.pix.height;  // размер выходного изображения
    qd() << "pix.width:\t\t"<<fmt.fmt.pix.width;
    qd() << "pix.field:\t\t"<<fmt.fmt.pix.field;  // Метод передачи видеокадра

    return true;
}

void V4L2::getFormatInfo()
{
    v4l2_fmtdesc fmtdesc;           // Эта структура необходима для получения формата, поддерживаемого выходным изображением камеры.
    fmtdesc.index = 0;
    fmtdesc.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;          // Соответствует типу в структуре v4l2_format.
    qd() << "Support format:";
    while (ioctl(fd,VIDIOC_ENUM_FMT, &fmtdesc) != -1)      // Получите формат, поддерживаемый выходным изображением камеры.
    {
        qd() << "\t\t" << fmtdesc.index << QString::fromLatin1((char *)fmtdesc.description) << "\t" << v4l2_fourcc2s(fmtdesc.pixelformat);

        ++fmtdesc.index;
    }


//    __u32               flags;
//    __u8		    description[32];   /* Description string */
//    __u32		    pixelformat;       /* Format fourcc      */
//    __u32		    mbus_code;		/* Media bus code    */
//    __u32		    reserved[3];
}

QImage V4L2::getImage()                               // Получите изображение, снятое камерой, а возвращаемое значение — переменную типа QImage.
{
    v4lbuf.index = n%Video_count;
    ioctl(fd, VIDIOC_DQBUF, &v4lbuf);                  // Удалите из очереди буфер, который получает данные изображения.

    QByteArray temp;
    temp.append((const char *)start[n%Video_count],length[n%Video_count]);
    image.loadFromData(temp);                          // Поместите данные изображения в переменную изображения.

    v4lbuf.index = n%Video_count;
    ioctl(fd, VIDIOC_QBUF, &v4lbuf);                   // Запрашивать блоки кэша, которые уже были прочитаны
    n++;
    if(n == 3){                                        // предотвратить накопительное переполнение
        n = 0;
    }
    return image;                                      // вернуть данные изображения
}

bool V4L2::closeCamera()                              // Закройте камеру и верните true, если закрытие прошло успешно.
{
    if(fd != -1){
        ioctl(fd, VIDIOC_STREAMOFF, &type);            // отображение конечного изображения
        int n = close(fd);                             // выключить видеооборудование
        if(n == -1){
            qd()<<"close camera failed";
            return false;
        }
    }
    for(i=0; i<Video_count; i++){
        if(start[i] != NULL){                          // Освободите запрошенную память
            start[i] = NULL;
        }
    }
    return true;
}


