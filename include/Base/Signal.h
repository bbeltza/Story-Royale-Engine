#ifndef SRE_SIGNAL_H
#define SRE_SIGNAL_H
#include <C_API.h>
#include <ints.h>

SRE_CAPI_BEGIN

typedef struct sre_Signal sre_Signal;
typedef struct sre_Connection sre_Connection;

typedef void (*sre_signalfunction)(void* signal_userdata, void* connection_data, void* fire_userdata);

sre_Signal* sre_signalcreate(void* userdata);
void sre_signaldestroy(sre_Signal* signal);

sre_Connection* sre_signalconnectEx(sre_Signal* signal, sre_signalfunction function, size_t extra_size, void** extra_data);
sre_Connection* sre_signalconnect(sre_Signal* signal, void* userdata, sre_signalfunction function);

// Increment the reference count of the connection to make sure it doesn't get deleted once it's detached from the signal
// @return Always `connection`
sre_Connection* sre_signalacquire(sre_Connection* connection);
// Decrement the reference count of the connection and detach it from its signal if the reference count is 0
void sre_signalrelease(sre_Connection* connection);
// Detach the connection from its signal, and decrement its reference count
void sre_signaldisconnect(sre_Connection* connection);

void* sre_signalwait(sre_Signal* signal);

bool sre_signalfire(sre_Signal* signal, void* data);

SRE_CAPI_END

#endif