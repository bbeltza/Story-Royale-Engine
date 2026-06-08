#ifndef SRE_SAMPLER_HPP
#define SRE_SAMPLER_HPP
#include <Core/Render.h>

namespace sre
{
    // RAII texture class, designed to replace sre::Texture
    class RAIITexture
    {
        sre::Texture* m_ptr = NULL;

        public:
            constexpr RAIITexture() = default;
            RAIITexture(sre::Texture* texture, bool aquire=false): m_ptr(texture) { if (aquire) texture->aquire(); }
            ~RAIITexture() { m_ptr->release(); }

            RAIITexture(const RAIITexture& copy): m_ptr(copy.m_ptr) { m_ptr->aquire(); };
            RAIITexture(RAIITexture&& right): m_ptr(right.m_ptr) { right.m_ptr = NULL; }

            void operator =(RAIITexture&& right) {
                if (m_ptr)
                    m_ptr->release();
                m_ptr = right.m_ptr;
                right.m_ptr = NULL;
            }

            void operator =(const RAIITexture& copy) {
                if (m_ptr)
                    m_ptr->release();
                m_ptr = copy.m_ptr;
                m_ptr->aquire();
            }
            
            sre::Texture* get() const { return m_ptr; }
            sre::Texture* operator ->() const { return m_ptr; }
            operator bool() const { return m_ptr != NULL; }
    };
}

#endif