#pragma once

class Logger
{
  public:
    Logger(const char* tag);

    void debug(...);

  private:
    const char* m_tag;
};