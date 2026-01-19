#ifndef SRE_SIGNAL_H
#define SRE_SIGNAL_H
#include <C_API.h>
#include <ints.h>

SRE_CAPI_BEGIN

typedef struct sre_Signal sre_Signal;
typedef struct sre_Connection sre_Connection;

typedef void (*sre_signalfunction)(void* signal_userdata, void* connection_userdata, void* fire_userdata);

sre_Signal* sre_signalcreateEx(void* userdata, sre_usize customdata_size, void** custom_dataout);
sre_Signal* sre_signalcreate(void* userdata) { return sre_signalcreateEx(userdata, 0, NULL); }
void sre_signaldestroy(sre_Signal* signal);

sre_Connection* sre_signalconnect(sre_Signal* signal, void* userdata, sre_signalfunction function);
void sre_signaldisconnect(sre_Connection* connection);

void* sre_signalwait(sre_Signal* signal, unsigned timeout);

_Bool sre_signalfile(sre_Signal* signal, void* data);

SRE_CAPI_END

#endif