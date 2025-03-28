#ifndef NVS_UTIL_H
#define NVS_UTIL_H

#include <Preferences.h>
#include <Arduino.h>

class NVSUtil
{
public:
    NVSUtil(const char *namespace_name);
    ~NVSUtil();

    // Save methods (renamed from Save to save for convention)
    bool saveInt(const char *key, int32_t value);
    bool saveFloat(const char *key, float value);
    bool saveString(const char *key, const char *value);
    bool saveBlob(const char *key, const void *data, size_t size);

    // Load methods
    int32_t loadInt(const char *key, int32_t default_value = 0);
    float loadFloat(const char *key, float default_value = 0.0f);
    String loadString(const char *key, const char *default_value = "");
    size_t loadBlob(const char *key, void *data, size_t size);

private:
    Preferences prefs;
};

#endif // NVS_UTIL_H