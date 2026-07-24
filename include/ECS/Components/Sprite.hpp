#ifndef SREECS_SPRITE_HPP
#define SREECS_SPRITE_HPP

#include <ECS/Component.hpp>

#include <Datatypes/Rect.h>
#include <Datatypes/Color.h>

#include <Base/Texture.hpp>

// Render Instance declaration to avoid including <Core/Render.h>, for `sreECS::SpriteFrame::get_renderinstance`, please include Render.h to use it.
struct sre_RenderInstance1;

namespace sreECS
{
    struct SpriteFrame
    {
        sre::vec2ut scale;
        sre::vec2ut offset;

        sre::rect2Di region;
        sre::RAIITexture texture;

        sre_RenderInstance1 get_renderinstance(const SpriteFrame& base, sre::col4 modulate, sre::Texture*& out_texture) const;
        void render(sre::s32 renderflags, const SpriteFrame& base, sre::col4 modulate=sre::WHITE) const;

        constexpr SpriteFrame(const sre::vec2ut& _scale={1, 1}, const sre::vec2ut& _offset={0, 0}, const sre::rect2Di& _region={0, 0, 0, 0}):
            scale(_scale), offset(_offset), region(_region) {}
        inline SpriteFrame(const sre::vec2ut& _scale, const sre::vec2ut& _offset, const sre::rect2Di& _region, const sre::RAIITexture& _texture):
            scale(_scale), offset(_offset), region(_region), texture(_texture) {}
    };

    struct Sprite : public Component
    {
        using Frame = sreECS::SpriteFrame;

        Frame base;
        sre::col4 modulate = sre::WHITE;

        // Adds new frames, self explanatory
        // @returns For `add_frame` The new index of that frame, for `add_frames`, the first index of the frame arrays. Call `get_frame()` or `get_frame_ptr` to get the contents of the frame from its index
        sre::u32 add_frames(const Frame* frames, sre::u32 count);
        sre::u32 add_frame(Frame&& frame);

        // Removes a range of frames
        // @returns The number of frames actually removed
        sre::u32 remove_frames(sre::u32 index, sre::u32 count);

        // Resize the frame container to contain enough space to fit `count` frames in a single allocation
        // sreECS::Sprite's frame system is built on top of an std::vector, this will call std::vector<Frame>::resize
        // If the allocated space is big enough to fit `count` frames, then this call won't do anything
        inline void reserve_frames(sre::u32 count) { m_frames.reserve(count); }

        // Get the pointer to the frame at an index, safe to call it with an index out of bounds, just that it returns NULL.

        // `get_frame_ptr()` but const
        inline const Frame* get_frame_ptr(sre::u32 index) const noexcept { return m_frames.size() < index ? &m_frames.at(index) : NULL; }
        inline       Frame* get_frame_ptr(sre::u32 index)       noexcept { return m_frames.size() < index ? &m_frames.at(index) : NULL; }

        // Get the reference to the frame at an index. `index` cannot be higher or equal to the number of existing frames

        inline const Frame& get_frame(sre::u32 index) const { return m_frames.at(index); }
        inline       Frame& get_frame(sre::u32 index)       { return m_frames.at(index); }

        inline sre::u32 num_frames() const noexcept { return static_cast<sre::u32>(m_frames.size()); }
        inline sre::u32 current_frame() const noexcept { return m_frame; }

        // Increment the frame index, and return `true` if the frame index has been incremented, or `false` if it's currently at the last available frame
        inline bool next() noexcept {
            if ((m_frame + 1) >= m_frames.size())
                return false;

            m_frame++;
            return true;
        }
        // Decrement the frame index, and return `true` if it has been decremented, or `false` if it's at the first frame
        inline bool prev() noexcept {
            return !m_frame ? false : (++m_frame || true);
        }

        inline bool set(sre::u32 index) noexcept {
            return index >= m_frames.size() ? false : (m_frame = index) || true;            
        }
        
        // Render the sprite
        void on_render(Entity& entity) override;
    public:
        inline sre::u32 add_frame(const Frame& frame={}) { return add_frames(&frame, 1); }
        inline sre::u32 add_frame(const sre::vec2ut& scale, const sre::vec2ut& offset, const sre::rect2Di& region={ 0, 0, 0, 0 }, const sre::RAIITexture& texture={}) {
            Frame frame{ scale, offset, region, texture };
            return add_frames(&frame, 1);
        }

        template <typename... Args>
        inline sre::u32 add_frames(const Frame& first, Args&&... args) {
            Frame frames[] = { first, args... };
            return add_frames(frames, sizeof(frames)/sizeof(*frames));
        }

        inline bool remove_frame(sre::u32 index) { return remove_frames(index, 1) != 0; }

        inline Sprite(const Frame& baseframe={}, sre::col4 modulate=sre::WHITE):
            base(baseframe), modulate(modulate) {}
    private:
        sre::u32 m_frame = 0;
        std::vector<Frame> m_frames;
    };
}

#endif