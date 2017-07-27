#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <termios.h>

#include <sys/ioctl.h>
#include <linux/serial.h>

#include "serial.h"
// #include "debug.h"

#define eprintk printf

static int tty_bauds[] =
{

0, 50, 75, 110, 134, 150, 200, 300, 600, 1200, 1800, 2400, 4800, 9600, 19200,
		38400, 57600, 115200,

};

static int tty_speeds[] =
{

B0, B50, B75, B110, B134,
B150, B200, B300, B600, B1200,
B1800, B2400, B4800, B9600, B19200,
B38400, B57600, B115200,

};

static int serial_open(struct serial_t* tty)
{

	int fd;

	fd = open(tty->device, O_RDWR | O_NOCTTY | O_NONBLOCK);
	if (fd < 0)
	{

		printf("SERIAL: can't open serial %s: %s\n", tty->device,
				strerror(errno));
		return -1;

	}

	tty->fd = fd;

	printf("SERIAL: success open serial %s(fd=%d)\n", tty->device, fd);
	return 0;

}

static int serial_close(struct serial_t *tty)
{

	int retval;

	if (tty->fd <= 0)
		return -1;

	retval = close(tty->fd);
	if (retval < 0)
	{

		printf("SERIAL: failed to close serial %s : %s\n", tty->device,
				strerror(errno));
		return -2;

	}

	tty->fd = 0;

	printf("SERIAL: success close serial %s\n", tty->device);

	return 0;

}

/*get serial's current attribute*/
static int serial_get_attr(struct serial_t *tty)
{

	if (tcgetattr(tty->fd, &tty->options) != 0)
	{

		eprintk("SERIAL: can't get serial's attribute\n");
		return -1;

	}

	return 0;

}

/*update serial's attrbute*/
static int serial_attr_update(struct serial_t *tty)
{

	tcflush(tty->fd, TCIFLUSH);/*handle unrecevie char*/

	if ((tcsetattr(tty->fd, TCSANOW, &tty->options)) < 0)
	{

		return -1;

	}

	return 0;

}

//set the special baud rate,like 28800
int serial_set_special_baud(struct serial_t *tty, int baud)
{

	struct serial_struct ss, ss_set;

	cfsetispeed(&tty->options, B38400);
	cfsetospeed(&tty->options, B38400);
	serial_attr_update(tty);

	if ((ioctl(tty->fd, TIOCGSERIAL, &ss)) < 0)
	{

		printf("BAUD: error to get the serial_struct info: %s\n",
				strerror(errno));
		return -1;

	}

	ss.flags = ASYNC_SPD_CUST;
	ss.custom_divisor = ss.baud_base / baud;

	if ((ioctl(tty->fd, TIOCSSERIAL, &ss)) < 0)
	{

		printf("BAUD: error to set serial_struct: %s\n", strerror(errno));
		return -2;

	}

	ioctl(tty->fd, TIOCGSERIAL, &ss_set);
	printf("BAUD: success set baud to %d,custom_divisor=%d,baud_base=%d\n",
			baud, ss_set.custom_divisor, ss_set.baud_base);

	return 0;

}

static int serial_init_baud(struct serial_t *tty)
{

	int index;

	if (serial_get_attr(tty) < 0)
		return -1;

	/*enable recevier and set local mode*/
	tty->options.c_cflag |= (CLOCAL | CREAD);

	for (index = 0; index < sizeof(tty_bauds) / sizeof(tty_bauds[0]); index++)
	{

		if (tty->baud == tty_bauds[index])
		{

			cfsetispeed(&tty->options, tty_speeds[index]);
			cfsetospeed(&tty->options, tty_speeds[index]);

			if (serial_attr_update(tty) < 0)
				return -2;

			printf("SERIAL: set baud to %d\n", tty->baud);
			return 0;

		}

	}

	printf(
			"SERIAL: %d is a no standard baud,called to set the special baud!!!\n",
			tty->baud);
	if ((serial_set_special_baud(tty, tty->baud)) < 0)
		return -2;

	return 0;

}

static int serial_init_databit(struct serial_t *tty)
{

	if (serial_get_attr(tty) < 0)
		return -1;

	tty->options.c_cflag &= ~CSIZE;
	switch (tty->databit)
	{

	case 5:
		tty->options.c_cflag |= CS5;
		break;
	case 6:
		tty->options.c_cflag |= CS6;
		break;
	case 7:
		tty->options.c_cflag |= CS7;
		break;
	case 8:
		tty->options.c_cflag |= CS8;
		break;
	default:
		eprintk("SERIAL: unsupported databit %d\n", tty->databit);
		return -2;

	}

	if (serial_attr_update(tty) < 0)
		return -3;

	printf("SERIAL: set databit to %d\n", tty->databit);
	return 0;

}

static int serial_init_parity(struct serial_t *tty)
{

	if (serial_get_attr(tty) < 0)
		return -1;

	/*ignore framing and parity error*/
	tty->options.c_iflag = IGNPAR;

	switch (tty->parity)
	{

	case 'n':
	case 'N':
		/* Clear parity enable */
		tty->options.c_cflag &= ~PARENB;
		/* Enable parity checking */
		tty->options.c_iflag &= ~INPCK;
		break;
	case 'o':
	case 'O':
		/* Set odd parity checking*/
		tty->options.c_cflag |= (PARODD | PARENB);
		/* Disnable parity checking */
		tty->options.c_iflag |= (INPCK | ISTRIP);
		break;
	case 'e':
	case 'E':
		/* Enable parity */
		tty->options.c_cflag |= PARENB;
		/* Set even parity checking*/
		tty->options.c_cflag &= ~PARODD;
		/* Disnable parity checking */
		tty->options.c_iflag |= (INPCK | ISTRIP);
		break;
	default:
		eprintk("SERIAL: unsupported parity %c\n", tty->parity);
		return -2;

	}

	if (serial_attr_update(tty) < 0)
		return -3;

	printf("SERIAL: set parity to %c\n", tty->parity);

	return 0;

}

static int serial_init_stopbit(struct serial_t *tty)
{

	if (serial_get_attr(tty) < 0)
		return -1;

	switch (tty->stopbit)
	{

	case 1:
		tty->options.c_cflag &= ~CSTOPB;
		break;
	case 2:
		tty->options.c_cflag |= CSTOPB;
		break;
	default:
		eprintk("SERIAL: unsupported stopbit %d\n", tty->stopbit);
		return -2;

	}

	if (serial_attr_update(tty) < 0)
		return -3;

	printf("SERIAL: set stopbit to %d\n", tty->stopbit);

	return 0;

}

int serial_init(struct serial_t *tty)
{

	/*set default databit:8,parity:N,stopbit:1*/
	if (tty->databit == 0)
		tty->databit = 8;
	if (tty->parity == 0)
		tty->parity = 'N';
	if (tty->stopbit == 0)
		tty->stopbit = 1;

	/*//FIXME:这里是直接设置的，没有使用
	 //设置无奇偶校验8N1
	 tty->options.c_cflag &= ~CSIZE;
	 tty->options.c_cflag  ?= CS8;
	 tty->options.c_cflag &= ~PARENB;
	 tty->options.c_cflag &= ~CSTOPB;
	 tty->options.c_cflag &= ~CSIZE;
	 tty->options.c_cflag  ?= CS8;

	 //终端为本地连接，忽略MODEM的状态线
	 tty->options.c_cflag  ?= CLOCAL;
	 tty->options.c_cflag  ?= CREAD;

	 //设置波特率
	 cfsetispeed(&tty->options,B115200);
	 cfsetospeed(&tty->options,B115200);
	 */

	/*init serial*/
	if (serial_open(tty) < 0)
		goto error;
	if (serial_init_baud(tty) < 0)
		goto error;
	if (serial_init_databit(tty) < 0)
		goto error;
	if (serial_init_parity(tty) < 0)
		goto error;
	if (serial_init_stopbit(tty) < 0)
		goto error;

//FIXME：这里已经解决第一次打开不能读的问题

	/*
	 //设置非加工方式输入
	 tty->options.c_iflag  ?= IGNPAR;
	 tty->options.c_lflag &= ~( ICANON  ? ECHO  ? ISIG  ? IEXTEN  ? ECHOE );
	 tty->options.c_iflag &= ~INPCK;
	 tty->options.c_iflag &= ~ICRNL;
	 tty->options.c_iflag &= ~INLCR;
	 tty->options.c_iflag &= ~ISTRIP;
	 tty->options.c_iflag &= ~IXON;
	 tty->options.c_iflag &= ~BRKINT;
	 */

//输入参数
	tty->options.c_iflag &= ~(IGNBRK | BRKINT | ICRNL | INPCK | ISTRIP | IXON
			| INLCR | IGNCR);
//选择原始输入(非加工方式输入),不回送字符
	tty->options.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG | IEXTEN);

//非加工方式输出
	tty->options.c_oflag &= ~(OPOST);
//阻塞字节数和等待时间
	tty->options.c_cc[VMIN] = 1;
	tty->options.c_cc[VTIME] = 0;

	serial_attr_update(tty);

	/*设置串口读写为阻塞模式*/
	fcntl(tty->fd, F_SETFL, 0);
//非阻塞方式
//fcntl(tty->fd, F_SETFL, FNDELAY);

	return 0;
	error: serial_close(tty);
	return -1;

}

int serial_send(struct serial_t *tty, const char *buf, int length)
{

	int ret, nleft;
	const char *ptmp;

	ret = 0;
	nleft = length;
	ptmp = buf;

	while (nleft > 0)
	{

		ret = write(tty->fd, ptmp, nleft);
		if (ret > 0)
		{

			nleft -= ret;
			ptmp += ret;

		}
		/*usleep(100);*/

	}

	/*
	 if(!nleft){

	 if(length == 1){

	 printf("SERIAL: success send a char,value=%d\n",*buf);

	 }else
	 printf("SERIAL: success send string %s\n",buf);

	 }else
	 printf("SERIAL: WARING,request send %d,but %d not send!!!\n",length,nleft);
	 */

	return length - nleft;

}

int serial_recv(struct serial_t *tty, char *buf, int length)
{

	int ret, left;
//int bytes;

	left = length;

	while (left > 0)
	{

		ret = 0;
//  bytes = 0;

		/*wait util have a key press*/
		ret = read(tty->fd, buf, left);
		if (ret > 0)
		{

			left -= ret;
			if (length != 1)
				buf += ret;

		}

		/*usleep(100);*/

		/* 非阻塞式读取按键，会消耗很大CPU。
		 ioctl(tty->fd, FIONREAD, &bytes);
		 if(bytes > 0){

		 ret = read(tty->fd,buf,left);

		 }

		 if(ret >0){

		 left -= ret;
		 if(length!=1)buf += ret;

		 }

		 //usleep(100);
		 */

	}

	return length - left;

}

//只能一下读取刚好的数据，比如需立即读取8个字符，如果没有就返回错误
//没有阻塞的读取，就是没有数据来的话就要立即返回了，而不是等待数据的到来
int serial_recv_noblock(struct serial_t *tty, char *buf, int length)
{

	int bytes;

	ioctl(tty->fd, FIONREAD, &bytes);
	if (bytes < length)
	{

//printf("SERIAL: request recv %d chars noblock,but can only %d !!!\n",length,bytes);
		return -1;

	}

	return read(tty->fd, buf, length);

}

int serial_remove(struct serial_t *tty)
{

	return serial_close(tty);

}
