#include "utils/nvs.h"

NVSUtil::NVSUtil(const char *namespace_name) : prefs()
{
    prefs.begin(namespace_name, false); // Open in read-write mode
}

NVSUtil::~NVSUtil()
{
    prefs.end(); // Commits changes and closes the Preferences instance
}

bool NVSUtil::saveInt(const char *key, int32_t value)
{
    return prefs.putInt(key, value) == sizeof(int32_t);
}

bool NVSUtil::saveFloat(const char *key, float value)
{
    return prefs.putFloat(key, value) == sizeof(float);
}

bool NVSUtil::saveString(const char *key, const char *value)
{
    return prefs.putString(key, value) > 0;
}

bool NVSUtil::saveBlob(const char *key, const void *data, size_t size)
{
    return prefs.putBytes(key, data, size) == size;
}

int32_t NVSUtil::loadInt(const char *key, int32_t default_value)
{
    return prefs.getInt(key, default_value);
}

float NVSUtil::loadFloat(const char *key, float default_value)
{
    return prefs.getFloat(key, default_value);
}

String NVSUtil::loadString(const char *key, const char *default_value)
{
    return prefs.getString(key, default_value);
}

size_t NVSUtil::loadBlob(const char *key, void *data, size_t size)
{
    return prefs.getBytes(key, data, size);
}