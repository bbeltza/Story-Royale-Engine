#include <Core/Thread.hpp>
#include <utils/logging.h>

sre::Thread::result_type sre::Thread::join()
{
    int success;
    result_type ret = sre_threadjoin(m_id, &success);

    m_id = SRE_INVALIDTHREAD;
    return ret;
}

void sre::Thread::detach()
{
    sre_threaddetach(m_id);
    m_id = SRE_INVALIDTHREAD;
}