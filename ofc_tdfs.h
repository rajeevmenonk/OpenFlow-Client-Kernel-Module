/********************************************************************
*
* Filename: ofc_tdfs.h
*
* Description: This file contains structures and enums used in
*              OpenFlow client kernel module.
*
*******************************************************************/

#ifndef __OFC_TDFS_H__
#define __OFC_TDFS_H__

/* Common structures */
typedef struct
{
    struct task_struct *pOfcDpThread;
    struct task_struct *pOfcCpThread;
    struct nf_hook_ops netFilterOps;
    int                aDataIfSockFd[OFC_MAX_OF_IF_NUM];
    char               aDataIfName[OFC_MAX_IFNAME_LEN];
    char               aCntrlIfName[OFC_MAX_IFNAME_LEN];
    int                events;
} tOfcGlobals;

/* Data path structures */
typedef struct
{
    struct list_head list;
    int              dataIfNum;
} tDataPktRxIfQ;

typedef struct
{
    struct socket    *aDataSocket[OFC_MAX_OF_IF_NUM];
    struct semaphore semId;
    struct semaphore dataPktQSemId;
    struct semaphore cpMsgQSemId;
    struct list_head pktRxIfListHead; /* Queue for interfaces on which
                                       * data packet is received */
    struct list_head cpMsgListHead;   /* Queue for messages from
                                       * control path sub module */
    struct list_head flowTableListHead;
    int              events;
} tOfcDpGlobals;

/* Control path structures */
typedef struct
{
    struct socket    *pCntrlSocket;
    struct semaphore semId;
    struct semaphore dpMsgQSemId;
    struct list_head dpMsgListHead; /* Queue for messages rx from
                                     * data path sub module */
    int              events;
    int              isModInit;
} tOfcCpGlobals;

typedef struct
{
    __u8   aDstMacAddr[OFC_MAC_ADDR_LEN];
    __u8   aSrcMacAddr[OFC_MAC_ADDR_LEN];
    __u16  vlanId;
    __u16  etherType;
    __u32  srcIpAddr;
    __u32  dstIpAddr;
    __u8   protocolType;
    __u16  srcPortNum;
    __u16  dstPortNum;
    __u8   inPort;
} tOfcMatchFields;

typedef struct
{
    struct list_head  list;
    struct list_head  flowEntryListHead;
    __u32             tableId;
    __u8              numMatch;
    __u32             activeCount;
    __u32             lookupCount;
    __u32             matchedCount;
    __u32             maxEntries;
    __u8              aTableName[32];
} tOfcFlowTable;

typedef struct
{
    struct list_head   list; 
    __u32              hardTimeout;
    __u32              idleTimeout;
    __u8               cookie;
    __u8               cookieMask;
    __u16              flags;
    __u16              priority;
    __u32              bufId;
    __u32              outPort;
    __u32              outGrp;
    __u32              tableId;
    __u32              matchCount;
    struct list_head   matchList;
    struct list_head   instrList;
    tOfcMatchFields    matchFields;
} tOfcFlowEntry;



typedef struct
{
    struct list_head list;
    tOfcFlowEntry    *pFlowEntry;
    __u8             *pPkt;
    __u32            pktLen;
} tDpCpMsgQ;

/* Function Declarations */
int OfcDpReceiveEvent (int events, int *pRxEvents);
int OfcDpSendEvent (int events);
void OfcDumpPacket (char *au1Packet, int len);
struct net_device *OfcGetNetDevByName (char *pIfName);
int OfcConvertStringToIp (char *pString, unsigned int *pIpAddr);

int OfcDpMainTask (void *args);
int OfcDpCreateSocketsForDataPkts (void);
int OfcDpRxDataPacket (void);
int OfcDpSendToDataPktQ (int dataIfNum);
tDataPktRxIfQ *OfcDpRecvFromDataPktQ (void);
tDpCpMsgQ *OfcDpRecvFromCpMsgQ (void);
int OfcDpSendToCpQ (__u8 *pPkt, __u32 pktLen);
int OfcDpCreateFlowTables (void);


int OfcCpMainTask (void *args);
int OfcCpReceiveEvent (int events, int *pRxEvents);
int OfcCpSendEvent (int events);
int OfcCpCreateCntrlSocket (void);
int OfcCpRxControlPacket (void);
int OfcCpSendToDpQ (__u8 *pPkt, __u32 pktLen);
tDpCpMsgQ *OfcCpRecvFromDpMsgQ (void);
void OfcCpRxDpMsg (void);

#endif /* __OFC_TDFS_H__ */
