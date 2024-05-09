#pragma once
#include "FS.h"
#include "SPIFFS.h"
#include "GTD_config.h"

#define FORMAT_SPIFFS_IF_FAILED true

int8_t GTD_FileCount()
{
    File root = SPIFFS.open("/");
    if (!root)
    {
        Serial.println("- failed to open directory");
        return 0;
    }
    File file = root.openNextFile();
    int8_t count = 0;

    while (file)
    {
        ++count;
        file = root.openNextFile();
    }
    file.close();

    return count;
}

GTD_SUCCESS_CODE GTD_FileRead(const char *path, String &Buffer)
{

    File file = SPIFFS.open(path);
    if (!file || file.isDirectory())
    {
        return GTD_SUCCESS_CODE::BAD;
    }
    while (file.available())
    {
        Buffer += file.read();
    }
    file.close();

    return GTD_SUCCESS_CODE::GOOD;
}

GTD_SUCCESS_CODE GTD_FileWrite(const char *path, const char *message)
{
    File file = SPIFFS.open(path, FILE_WRITE);
    if (!file)
    {
        return GTD_SUCCESS_CODE::BAD;
    }
    if (!file.print(message))
    {
        file.close();
        return GTD_SUCCESS_CODE::BAD;
    }
    file.close();
    return GTD_SUCCESS_CODE::GOOD;
}

GTD_SUCCESS_CODE GTD_FileAppend(const char *path, const char *message)
{
    File file = SPIFFS.open(path, FILE_WRITE);
    if (!file)
    {
        return GTD_SUCCESS_CODE::BAD;
    }
    if (!file.print(message))
    {
        file.close();
        return GTD_SUCCESS_CODE::BAD;
    }
    file.close();
    return GTD_SUCCESS_CODE::GOOD;
}

GTD_SUCCESS_CODE GTD_FileRename(const char *path1, const char *newName)
{
    if (!SPIFFS.rename(path1, newName))
    {
        return GTD_SUCCESS_CODE::BAD;
    }
    return GTD_SUCCESS_CODE::GOOD;
}

GTD_SUCCESS_CODE GTD_FileDelete(const char *path)
{
    if (!SPIFFS.remove(path))
    {
        return GTD_SUCCESS_CODE::BAD;
    }
    return GTD_SUCCESS_CODE::GOOD;
}

GTD_SUCCESS_CODE GTD_FileRWTest()
{
    GTD_SUCCESS_CODE st = GOOD;
    st = GTD_FileWrite("TEST_RW.txt", "TEST_RW_MSG");
    String buf;
    st = GTD_FileRead("TEST_RW.txt", buf);
    if (buf != "TEST_RW_MSG")
    {
        st = GTD_SUCCESS_CODE::BAD;
    }
    return st;
}