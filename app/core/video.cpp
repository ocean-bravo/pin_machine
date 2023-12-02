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

quint32 stringToFourcc(QString fmt)
{
    // TODO: проверки тут бы сделать разные
    return v4l2_fourcc(fmt[0].toLatin1(), fmt[1].toLatin1(), fmt[2].toLatin1(), fmt[3].toLatin1());
}

int yuyv422_to_rgb_pixel(int y, int u, int v)
{
    unsigned int pixel32 = 0;
    unsigned char *pixel = (unsigned char *)&pixel32;
    int r, g, b;
    r = y + (1.370705 * (v-128));
    g = y - (0.698001 * (v-128)) - (0.337633 * (u-128));
    b = y + (1.732446 * (u-128));
    if(r > 255) r = 255;
    if(g > 255) g = 255;
    if(b > 255) b = 255;
    if(r < 0) r = 0;
    if(g < 0) g = 0;
    if(b < 0) b = 0;
    pixel[0] = r * 220 / 256;
    pixel[1] = g * 220 / 256;
    pixel[2] = b * 220 / 256;
    return pixel32;
}

int yuyv422_to_rgb888_buffer(unsigned char *yuyv422, unsigned char *rgb, quint32 width, quint32 height)
{
    quint32 in, out = 0;
    quint32 pixel_16;
    unsigned char pixel_24[3];
    quint32 pixel32;
    int y0, u, y1, v;
    for(in = 0; in < width * height * 2; in += 4)
    {
        pixel_16 =
            yuyv422[in + 3] << 24 |
            yuyv422[in + 2] << 16 |
            yuyv422[in + 1] <<  8 |
            yuyv422[in + 0];
        y0 = (pixel_16 & 0x000000ff);
        u  = (pixel_16 & 0x0000ff00) >>  8;
        y1 = (pixel_16 & 0x00ff0000) >> 16;
        v  = (pixel_16 & 0xff000000) >> 24;
        pixel32 = yuyv422_to_rgb_pixel(y0, u, v);
        pixel_24[0] = (pixel32 & 0x000000ff);
        pixel_24[1] = (pixel32 & 0x0000ff00) >> 8;
        pixel_24[2] = (pixel32 & 0x00ff0000) >> 16;
        rgb[out++] = pixel_24[0];
        rgb[out++] = pixel_24[1];
        rgb[out++] = pixel_24[2];
        pixel32 = yuyv422_to_rgb_pixel(y1, u, v);
        pixel_24[0] = (pixel32 & 0x000000ff);
        pixel_24[1] = (pixel32 & 0x0000ff00) >> 8;
        pixel_24[2] = (pixel32 & 0x00ff0000) >> 16;
        rgb[out++] = pixel_24[0];
        rgb[out++] = pixel_24[1];
        rgb[out++] = pixel_24[2];
    }
    return 0;
}

}


V4L2::V4L2()
{

}

V4L2::~V4L2()
{

}

// Чтобы инициализировать камеру, вам необходимо передать путь монтирования камеры и частоту кадров на выходе и вернуть true, если инициализация прошла успешно.
bool V4L2::init(QString cameraPath, quint32 fps, quint32 width, quint32 height, QString pixelformat)
{
    _width = width;
    _height = height;

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

    if (!setFormat(width, height, pixelformat))
        return false;

    if (!setFps(fps))
        return false;

    req.count=1;                              // 3 тайника
    req.type=V4L2_BUF_TYPE_VIDEO_CAPTURE;               // Соответствует типу в структуре v4l2_format.
    req.memory=V4L2_MEMORY_MMAP;                        // В режиме отображения памяти поле счетчика действительно только в том случае, если для него установлено значение V4L2_MEMORY_MMAP.
    if(ioctl(fd,VIDIOC_REQBUFS,&req)==-1)               // Применить видеокэш к ядру
    {
        qd()<<"request for buffers error";
        return false;
    }

    for (i=0; i<1; i++)                       // mmap четыре буфера
    {
        bzero(&buffer, sizeof(buffer));
        buffer.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        buffer.memory = V4L2_MEMORY_MMAP;
        buffer.index = i;
        if (ioctl (fd, VIDIOC_QUERYBUF, &buffer) == -1){ // Получите параметры буфера, эти параметры необходимы для mmap
            qd()<<"query buffer error";
            return false;
        }
        length = buffer.length;                   // сохранить размер кэша
        start = (unsigned char *)mmap(NULL,buffer.length,PROT_READ |PROT_WRITE, MAP_SHARED, fd, buffer.m.offset);
        // Сопоставьте запрошенный кеш ядра с пространством пользователя.
    }
    for (i=0; i<1; i++){
        buffer.index = i;
        ioctl(fd, VIDIOC_QBUF, &buffer);             // поставить в очередь кеш
    }
    type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    ioctl (fd, VIDIOC_STREAMON, &type);                 // Открыть поток ввода-вывода

    bzero(&v4lbuf, sizeof(v4lbuf));
    v4lbuf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    v4lbuf.memory = V4L2_MEMORY_MMAP;
    qd() << "init ok";

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

    qd()<<"fps:\t\t"<< setfps.parm.capture.timeperframe.denominator / setfps.parm.capture.timeperframe.numerator;// частота кадров на выходе

    return true;
}

// Используется для установки формата пикселей. Формат вывода изображения. Размер изображения. Метод сканирования.
bool V4L2::setFormat(quint32 width, quint32 height, QString pixelformat)
{
    v4l2_format fmt;
    fmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;               // формат пикселей
    fmt.fmt.pix.pixelformat = stringToFourcc(pixelformat); // Вывод в формате JPEG, MJPG, YUYV или другой
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

    qd() << "Current format:";
    qd() << "fmt.type:\t\t"<<fmt.type;            // формат выходных пикселей
    qd() << "pix.height:\t"<<fmt.fmt.pix.height;  // размер выходного изображения
    qd() << "pix.width:\t\t"<<fmt.fmt.pix.width;
    qd() << "pix.field:\t\t"<<fmt.fmt.pix.field;  // Метод передачи видеокадра
    qd() << "pix.pixelformat:\t" << v4l2_fourcc2s(fmt.fmt.pix.pixelformat);

    return true;
}

void V4L2::getFormatInfo()
{
    v4l2_fmtdesc fmtdesc;           // Эта структура необходима для получения формата, поддерживаемого выходным изображением камеры.
    fmtdesc.index = 0;
    fmtdesc.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;          // Соответствует типу в структуре v4l2_format.
    qd() << "Supported formats:";
    while (ioctl(fd,VIDIOC_ENUM_FMT, &fmtdesc) != -1)      // Получите формат, поддерживаемый выходным изображением камеры.
    {
        qd() << "\t\t" << fmtdesc.index << QString::fromLatin1((char *)fmtdesc.description) << "\t" << v4l2_fourcc2s(fmtdesc.pixelformat);

        ++fmtdesc.index;
    }
}

QImage V4L2::getImage()                               // Получите изображение, снятое камерой, а возвращаемое значение — переменную типа QImage.
{
    v4l2_buffer v4lbuf;
    v4lbuf.index = 0;
    ioctl(fd, VIDIOC_DQBUF, &v4lbuf);                  // Удалите из очереди буфер, который получает данные изображения.



    //static int fmt = 0;

    uchar outbuf[1000000];
    yuyv422_to_rgb888_buffer(start, outbuf, int(_width), int(_height));
    QImage image(outbuf, int(_width), int(_height), QImage::Format_RGB888); // Поместите данные изображения в переменную изображения.
    //qd() << "now format " << fmt;
    //++fmt;

    v4lbuf.index = 0;
    ioctl(fd, VIDIOC_QBUF, &v4lbuf);                   // Запрашивать блоки кэша, которые уже были прочитаны
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
//    for(i=0; i<Video_count; i++)
//    {
        if(start != NULL)
        {                          // Освободите запрошенную память
            start = NULL;
        }
//    }
    return true;
}


