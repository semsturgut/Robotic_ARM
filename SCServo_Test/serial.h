#ifndef __SERIAL_H_
#define __SERIAL_H_

#include <termios.h>

#ifdef __cplusplus
extern "C"{
#endif

/**
 * @brief 串口设置
 * @note 默认串口的设置是 8N1,就是数据位8位，无奇偶校检,停止位为1
 * @par 结构体成员说明：
 * - fd: 设备文件描述符,用于操作串口
 * - device: 设备文件,/dev/ttyS0,...
 * - baud: 波特率,1200,28800
 * - databit: 数据位,5,6,7,8
 * - parity: 奇偶校检,O,E,N
 * - stopbit: 停止位,1,2
 * - startbit: 起始位,1
 * - options: 串口设置选项
 *
 */

/*FIXME: not support start bit*/
struct serial_t {

int fd;
char*device;

int baud;
int databit;
char parity;
int stopbit;
int startbit;

struct termios options;

};

int serial_init(struct serial_t *tty);
int serial_send(struct serial_t *tty, const char *buf, int length);
int serial_recv(struct serial_t *tty, char *buf, int length);
int serial_recv_noblock(struct serial_t *tty, char *buf, int length);
int serial_remove(struct serial_t *tty);

#ifdef __cplusplus
}
#endif

#endif /*__SERIAL_H_*/
