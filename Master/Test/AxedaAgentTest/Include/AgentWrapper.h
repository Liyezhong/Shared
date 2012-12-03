#ifndef AGENTWRAPPER_H
#define AGENTWRAPPER_H

#include <QString>
#include <QDateTime>

#include <stdio.h>
#include <math.h>
#include <unistd.h>

#include "../Include/AeTypes.h"
#include "../Include/AeError.h"
#include "../Include/AeInterface.h"
#include "../Include/AeDemoCommon.h"

// Predefine the model/serial number.
#define MODEL_NUMBER    "DemoModel1"
#define SERIAL_NUMBER   "DemoDevice1"
// Define time periods
#define PING_RATE                15  /// ping rate
#define DRMEXEC_TIME_LIMIT       10  /// execution limit for AeDRMExecute
// Define size of the queue (to check how queue callbacks work)
#define TEST_QUEUE_SIZE_BYTES    500000//1000  /// size of the outgoing message queue in bytes
// Define name of the file to upload:
//#define UPLOAD_FILE_NAME         "/home/colorado/SVN-repo/trunk/Platform/Master/Test/AxedaAgentTest/TestAxeda.xtx"
#define UPLOAD_FILE_NAME         "TestAxeda.xtx"
#define UPLOAD_FILE_NAME2        "AnotherTestAxeda.xtx"

class AgentWrapper : public QObject
{
    Q_OBJECT

class AlarmData;

public:

    AgentWrapper(QObject *pParent = NULL);
    virtual ~AgentWrapper();
    bool Initialize(int argc, char *argv[]);

    // == Member functions for Axeda Agent Callbacks: =======================================================

    static void OnWebErrorCallback(AeInt32 iServerId, AeError iError);
    static void OnDeviceRegisteredCallback(AeInt32 iDeviceId);
    static void QueueStatusCallback(AeDRMQueueStatus iNewStatus);
    static void RemoteSessionEndCallback(AeRemoteInterface *pInterface);
    static AeBool RemoteSessionStartCallback(AeRemoteInterface *pInterface);
    static void OnSOAPMethodCallback(AeInt32 iDeviceId, AeHandle pMethod, AeDRMSOAPCommandStatus *pStatus);
    static void OnSOAPMethodExCallback(AeInt32 iDeviceId, AeInt32 iServerId, AeHandle pMethod, \
                                       AeDRMSOAPCommandId *pSOAPId, AeDRMSOAPCommandStatus *pStatus);
    static void OnCommandSetTagCallback(AeInt32 iDeviceId, AeDRMDataItem *pDataItem, AeDRMSOAPCommandStatus *pStatus);
    static AeBool OnFileDownloadBeginCallback(AeInt32 iDeviceId, AePointer *ppUserData);
    static AeBool OnFileDownloadDataCallback(AeInt32 iDeviceId, AeFileStat *pFile, AeChar *pData, AeInt32 iSize, \
                                             AePointer pUserData);
    static void OnFileDownloadEndCallback(AeInt32 iDeviceId, AeBool bOK, AePointer pUserData);
    static AeBool OnFileUploadBeginCallback(AeInt32 iDeviceId, AeFileUploadSpec **ppUploads, AePointer *ppUserData);
    static AeBool OnFileUploadBeginExCallback(AeInt32 iDeviceId, AeFileUploadSpec **ppUploads, \
                                              AeFileUploadExecuteParam *pParam, AePointer *ppUserData);
    static AeBool OnFileUploadDataCallback(AeInt32 iDeviceId, AeFileStat **ppFile, AeChar **ppData, \
                                           AeInt32 *piSize, AePointer pUserData);
    static void OnFileUploadEndCallback(AeInt32 iDeviceId, AeBool bOK, AePointer pUserData);
    static void OnFileTransferEventCallback(AeInt32 iDeviceId, AeFileTransferEvent iEvent, AePointer pUserData);
    static void OnCommandSetTimeCallback(AeInt32 iDeviceId, AeTimeValue *pTime, AeInt32 *piTZOffset, \
                                         AeDRMSOAPCommandStatus *pStatus);
    static void OnCommandRestartCallback(AeInt32 iDeviceId, AeBool bHard, AeDRMSOAPCommandStatus *pStatus);
    static void OnPingRateUpdateCallback(AeInt32 iServerId, AeDRMPingRate *pPingRate);

signals:

    void Finished();

public slots:

    void run();
    void stop();
    void SetupAlarm();
    void SetupEvent();
    void SetupUpload();
    void SetupAnotherUpload();
    void SetupTwoUploads();

private slots:

    void work();

private:

    void RegisterAllCallbacks();
    void AeDemoSine(AeDRMDataValue *pValue);
    void AeDemoRamp(AeDRMDataValue *pValue);
    void AeDemoString(AeDRMDataValue *pValue);
    AeBool AeDemoAlarm(AeDRMAlarm *pAlarm, AeDRMDataItem *pDataItem);

private:

    AeDemoConfig  m_config;
    AeInt32       m_DeviceId;
    AeInt32       m_ServerId;
    AeTimeValue   m_pingRate;
    AeTimeValue   m_timeLimit;
    bool          m_Active;

}; // class AgentWrapper

class AlarmData {   // draft class
    /* Structure for an Alarm description. */
//    struct _AeDRMAlarm
//    {
//        AeChar          *pName;         /* name */
//        AeChar          *pDescription;  /* textual description */
//        AeChar          *pCondition;    /* condition: LoLo, Lo, Hi, HiHi */
//        AeTimeValue     timeStamp;      /* timestamp */
//        AeInt           iSeverity;      /* severity */
//        AeBool          bActive;        /* is the alarm active? */
//        AeBool          bAck;           /* has the alarm been acknowledged? */
//        AeDRMDataItem   *pDataItem;     /* associated data item (may be NULL) */
//    };

public:

    AlarmData(const QString &nm, const QString &descr, const QString &cond, const QDateTime &dt, \
              quint16 sev, bool act, bool acked, AeDRMDataItem *data) :
            name(nm),
            description(descr),
            condition(cond),
            timestamp(dt),
            severity(sev),
            active(act),
            acknowledged(acked),
            dataitem(data)
    {
    }

    ~AlarmData() {}

public:
    QString name;
    QString description;
    QString condition;
    QDateTime timestamp;
    quint16 severity;
    bool    active;
    bool    acknowledged;
    AeDRMDataItem *dataitem;
};

#endif
