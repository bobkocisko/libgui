//
// Created by Bob Kocisko on 2/8/16.
//

#include "libgui/ScopeExit.h"

namespace libgui
{

ScopeExit::ScopeExit(std::function<void()> f)
  : ScopeExit(TerminateOnException, f)
{

}

ScopeExit::ScopeExit(ExceptionHandling exceptionHandling, std::function<void()> f)
  : _exceptionHandling(exceptionHandling), _f(f)
{

}

ScopeExit::~ScopeExit() noexcept
{
  switch (_exceptionHandling)
  {
    case TerminateOnException:
    {
      _f();

      break;
    }
    case IgnoreAnyExceptions:
    {
      try
      {
        _f();
      }
      catch (...)
      {
        // Ignore
      }

      break;
    }
  }
}

}