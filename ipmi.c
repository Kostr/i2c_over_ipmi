#include <errno.h>
#include <fcntl.h>
#include <limits.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <sys/ioctl.h>
#include <linux/ipmi.h>

#if _HAVE_DMALLOC_H
#include <dmalloc.h>
#endif

#include "ipmi.h"

static int ipmi_open(void)
{
	int fd;

	fd = open("/dev/ipmi0", O_RDWR);
	if (fd >= 0)
		return fd;
	fd = open("/dev/ipmidev/0", O_RDWR);
	if (fd >= 0)
		return fd;
	fd = open("/dev/ipmidev0", O_RDWR);
	if (fd >= 0)
		return fd;
	fd = open("/dev/bmc", O_RDWR);
	if (fd >= 0)
		return fd;
	return -1;
}

int ipmicmd(int sa, int lun, int netfn, int cmd, int datalen, void *data,
	    int resplen, int *rlen, void *resp)
{
	static int msgid;
	struct ipmi_system_interface_addr saddr;
	struct ipmi_ipmb_addr iaddr;
	struct ipmi_addr raddr;
	struct ipmi_req req;
	struct ipmi_recv rcv;
	fd_set rfd;
	int fd, rc;
	uint8_t tresp[resplen + 1];

	fd = ipmi_open();
	if (fd < 0)
		return -1;

	memset(&req, 0, sizeof(req));
	memset(&rcv, 0, sizeof(rcv));
	if (sa == BMC_SA) {
		memset(&saddr, 0, sizeof(saddr));
		saddr.addr_type = IPMI_SYSTEM_INTERFACE_ADDR_TYPE;
		saddr.channel = IPMI_BMC_CHANNEL;
		saddr.lun = 0;
		req.addr = (void *)&saddr;
		req.addr_len = sizeof(saddr);
	} else {
		memset(&iaddr, 0, sizeof(iaddr));
		iaddr.addr_type = IPMI_IPMB_ADDR_TYPE;
		iaddr.channel = 0;
		iaddr.slave_addr = sa;
		iaddr.lun = lun;
		req.addr = (void *)&iaddr;
		req.addr_len = sizeof(iaddr);
	}

	/* Issue command */
	req.msgid = ++msgid;
	req.msg.netfn = netfn;
	req.msg.cmd = cmd;
	req.msg.data_len = datalen;
	req.msg.data = data;
	rc = ioctl(fd, IPMICTL_SEND_COMMAND, (void *)&req);
	if (rc != 0) {
#ifdef DEBUG
		perror("send");
#endif
		goto end;
	}

	/* Wait for Response */
	FD_ZERO(&rfd);
	FD_SET(fd, &rfd);
	rc = select(fd + 1, &rfd, NULL, NULL, NULL);
	if (rc < 0) {
#ifdef DEBUG
		perror("select");
#endif
		goto end;
	}

	/* Get response */
	rcv.msg.data = tresp;
	rcv.msg.data_len = resplen + 1;
	rcv.addr = (void *)&raddr;
	rcv.addr_len = sizeof(raddr);
	rc = ioctl(fd, IPMICTL_RECEIVE_MSG_TRUNC, (void *)&rcv);
	if (rc != 0 && errno == EMSGSIZE)
#ifdef DEBUG
		printf("too short..\n");
#endif
	if (rc != 0 && errno != EMSGSIZE) {
#ifdef DEBUG
		printf("%s\n", strerror(errno));
#endif
		goto end;
	}
	if (rcv.msg.data[0]) {
#ifdef DEBUG
		printf("IPMI Error: %.2x\n", rcv.msg.data[0]);
#endif
		goto completion_error;
	}
	rc = 0;
	*rlen = rcv.msg.data_len - 1;
	memcpy(resp, rcv.msg.data + 1, *rlen);
end:
	close(fd);
	return rc;

completion_error:
	close(fd);
	return rcv.msg.data[0];
}
