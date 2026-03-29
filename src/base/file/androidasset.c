#include <Base/File.h>
#include <android/asset_manager.h>
#include <android/asset_manager_jni.h>

#include <SDL_system.h>
static AAssetManager* s_mgr;
static jobject s_mgrref;

static bool androidasset_open(sre_FileImpl* impl, const char* path, int mode)
{
    if (!s_mgr)
    {
        JNIEnv* env = (JNIEnv*)SDL_AndroidGetJNIEnv();
        (*env)->PushLocalFrame(env, 2);

            jobject jcontext = (jclass)SDL_AndroidGetActivity();
            jclass sdlclass = (*env)->GetObjectClass(env, jcontext);

            jmethodID getAssets_mid = (*env)->GetMethodID(env, sdlclass,
                                  "getAssets", "()Landroid/content/res/AssetManager;");
            jobject jmanager = (*env)->CallObjectMethod(env, jcontext, getAssets_mid);
            s_mgrref = (*env)->NewGlobalRef(env, jmanager);

            s_mgr = AAssetManager_fromJava(env, jmanager);

            if (!s_mgr)
            {
                (*env)->DeleteGlobalRef(env, s_mgrref);
                s_mgrref = NULL;
            }

        (*env)->PopLocalFrame(env, NULL);
    }
    if (!s_mgr)
        return false;

    if (mode != SRE_FILE_READ && mode != SRE_FILE_DEFAULT)
        return false;

    AAsset* asset = AAssetManager_open(s_mgr, path, AASSET_MODE_UNKNOWN);
    if (!asset)
        return false;

    *impl = asset;
    return true;
}

static void androidasset_close(sre_FileImpl impl)
{
    AAsset_close((AAsset*)impl);
}

static size_t androidasset_read(sre_FileImpl impl, void* data, size_t size)
{
    int res = AAsset_read((AAsset*)impl, data, size);
    assert(res >= 0);
    return res;
}

static size_t androidasset_write(sre_FileImpl impl, const void* data, size_t size)
{
    (void)impl;
    (void)data;
    (void)size;

    return 0;
}

static bool androidasset_seek(sre_FileImpl impl, long offset, sre_seek _origin)
{
    int origin = -1;
    switch (_origin)
    {
        case SRE_SEEK_SET: origin = SEEK_SET; break;
        case SRE_SEEK_CUR: origin = SEEK_CUR; break;
        case SRE_SEEK_END: origin = SEEK_END; break;
    }

    assert( origin != -1 );

    return AAsset_seek((AAsset*)impl, (off_t)offset, origin) != -1;
}

static long androidasset_tell(sre_FileImpl impl)
{
    return AAsset_seek((AAsset*)impl, 0, SEEK_CUR);
}

static size_t androidasset_size(sre_FileImpl impl)
{
    return AAsset_getLength((AAsset*)impl);
}

static const sre_byte* androidasset_begin(sre_FileImpl impl)
{
    return (const sre_byte*)AAsset_getBuffer((AAsset*)impl);
}

const sre_FVFT SRE_ANDROIDASSET_VFT = {
    androidasset_open,
    androidasset_close,

    androidasset_read,
    androidasset_write,
    
    androidasset_seek,
    androidasset_tell,

    androidasset_size,
    androidasset_begin
};