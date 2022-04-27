#define BMC_SA 0x20

int ipmicmd(int sa, int lun, int netfn, int cmd, int datalen, void *data,
	    int resplen, int *rlen, void *resp);
