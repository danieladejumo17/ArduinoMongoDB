#include "utilities.h"

// ----------- LOGGING ------------
void logdebug(const String& debugString)
{
  Serial.println(String("DEBUG: ") + debugString);
}

void loginfo(const String& info)
{
  Serial.println(String("INFO: ") + info);
}

void logwarn(const String& warning)
{
  Serial.println(String("WARN: ") + warning);
}

void logerr(const String& err)
{
  Serial.println(String("ERROR: ") + err);
}

void logfatal(const String& info)
{
  Serial.println(String("FATAL: ") + info);
}
