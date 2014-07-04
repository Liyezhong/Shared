/* AeAgentRestart.h */

#ifndef _AE_AGENT_RESTART_H_
#define _AE_FILE_UPLOAD_H_

typedef struct _AeAgentRestart AeAgentRestart;

struct _AeAgentRestart
{
    AeFileInstruction parent;
    AeBool  bHard;
};


#ifdef __cplusplus
extern "C" {
#endif

AeAgentRestart *AeAgentRestartNew(void);

#ifdef __cplusplus
}
#endif

/* _AE_AGENT_RESTART_H_ */
#endif
