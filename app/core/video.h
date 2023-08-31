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

#pragma once

#include <fcntl.h>                                  // Откройте заголовочный файл, в котором находится функция открытия камеры.
#include <sys/mman.h>                               // Сопоставьте запрошенный кеш ядра с заголовочным файлом функции mmap в пользовательском пространстве.
#include <linux/videodev2.h>                        // Заголовочный файл, в котором находится структура, связанная с V4L2.
#include <unistd.h>                                 // Закройте заголовочный файл, в котором находится функция закрытия камеры.
#include <sys/ioctl.h>
#include <QDebug>
#include <QImage>

#define Video_count    3                            // Буферизованные кадры
#define Image_Width    1280                         // Пиксели выходного изображения
#define Image_High     720

class V4L2
{
public:
    V4L2();
    ~V4L2();
    bool init(QString cameraPath, unsigned int frame);// Функция инициализации камеры должна передать путь установки камеры и частоту кадров на выходе и возвращает true, если инициализация прошла успешно.
    QImage getImage();                         // Получите изображение: возвращаемое значение — это переменная типа QImage.
    bool closeCamera(void);                        // Закройте камеру, верните true, если закрытие прошло успешно.
    int n = 0;                                          // Используется в функции getImage для управления тем, какой кеш используется при получении изображений.

private:
    int                           i;
    int                           fd;               // ручка камеры
    int                           length[Video_count];// Размер кеша, используемого для хранения запросов
    QImage                        image;
    unsigned char *               start[Video_count];// Используется для сохранения данных изображения.

    struct v4l2_buffer            buffer[Video_count];// Эта структура используется при запросе кеша из ядра, и каждая структура v4l2_buffer соответствует кешу в драйвере камеры ядра.
    struct v4l2_format            fmt;               // Используется для установки формата пикселей. Формат вывода изображения. Размер изображения. Метод сканирования.
    struct v4l2_fmtdesc           fmtdesc;           // Эта структура необходима для получения формата, поддерживаемого выходным изображением камеры.
    struct v4l2_capability        cap;               // Эта структура необходима при проверке информации о камере.
    struct v4l2_streamparm        setfps;            // Эта структура используется при настройке частоты кадров.
    struct v4l2_requestbuffers    req;               // Эта структура используется при подаче запроса на кеш из ядра.
    struct v4l2_buffer            v4lbuf;            // Эта структура используется, когда кэш выводится из очереди и ставится в очередь.
    enum   v4l2_buf_type          type;              // Эта структура используется при открытии потока ввода-вывода.
};
