//
// Created by Bob Kocisko on 12/18/15.
//

#pragma once

namespace libgui
{

// An InputId identifies the particular input that is causing an
// input event.  InputId 1 represents a pointer.  Other
// input ids represent unique touch points.  InputId 0 is undefined.
const int PointerInputId = 1;

const int FirstTouchId = PointerInputId + 1;

struct InputIdentifier
{
  InputIdentifier(int inputId);

  bool IsPointer();

  bool IsTouch();

  operator int() const;

  bool operator==(const InputIdentifier other);

private:
  int _inputId;
};

typedef InputIdentifier InputId;

}