//
// Created by Bob Kocisko on 6/8/18.
//

#include "libgui/Rect.h"

#include <gtest/gtest.h>
#include <gmock/gmock.h>

using namespace libgui;
using namespace std;

namespace
{
struct RectDetails
{
  Rect4 Rect;
  int SeenCount = 0;
};
struct Rects
{
  RectDetails This;
  RectDetails Other;
  RectDetails Exclusion;
};

const unsigned int ThisRectId = 1;
const unsigned int OtherRectId = 2;
const unsigned int ExclusionRectId = 4;

const unsigned int T = ThisRectId;
const unsigned int O = OtherRectId;
const unsigned int E = ExclusionRectId;

void UpdateRect(int row, int col, RectDetails& modify)
{
  if (0 == modify.SeenCount)
  {
    modify.Rect.left = col;
    modify.Rect.top = row;
  }
  else if (1 == modify.SeenCount)
  {
    modify.Rect.right = col;
  }
  else if (2 == modify.SeenCount)
  {
    modify.Rect.bottom = row;
  }

  ++modify.SeenCount;
}

// Thanks to https://stackoverflow.com/a/17569578/4307047
template <size_t rows, size_t cols>
Rects Generate(unsigned int (&matrix)[rows][cols])
{
  Rects result;
  for (int i = 0; i < rows; ++i)
  {
    for (int j = 0; j < cols; ++j)
    {
      auto val = matrix[i][j];
      if (val & T)
      {
        UpdateRect(i, j, result.This);
      }
      if (val & O)
      {
        UpdateRect(i, j, result.Other);
      }
      if (val & E)
      {
        UpdateRect(i, j, result.Exclusion);
      }
    }
  }
  return result;
};

}


TEST(Rect4Tests, Test)
{
  const int _ = 0;

  #define DO_TEST \
    auto result = Generate(a); \
    result.This.Rect.ExcludeWith(result.Other.Rect); \
    EXPECT_NEAR(result.Exclusion.Rect.left, result.This.Rect.left, Rect4::Epsilon + 0.0001); \
    EXPECT_NEAR(result.Exclusion.Rect.top, result.This.Rect.top, Rect4::Epsilon + 0.0001); \
    EXPECT_NEAR(result.Exclusion.Rect.right, result.This.Rect.right, Rect4::Epsilon + 0.0001); \
    EXPECT_NEAR(result.Exclusion.Rect.bottom, result.This.Rect.bottom, Rect4::Epsilon + 0.0001); \

  {
    // no intersection
    unsigned int a[10][10] = {
      {T|E,_,_,T|E,_,O,_,_,O,_},
      { _ ,_,_, _ ,_,_,_,_,_,_},
      { _ ,_,_, _ ,_,_,_,_,_,_},
      { _ ,_,_, _ ,_,_,_,_,_,_},
      {T|E,_,_,T|E,_,O,_,_,O,_},
      { _ ,_,_, _ ,_,_,_,_,_,_},
      { _ ,_,_, _ ,_,_,_,_,_,_},
      { _ ,_,_, _ ,_,_,_,_,_,_},
      { _ ,_,_, _ ,_,_,_,_,_,_},
      { _ ,_,_, _ ,_,_,_,_,_,_},
    };
    DO_TEST
  }

  {
    // intersect: other is bottom right (favoring bottom)
    unsigned int a[6][8] = {
      { _ ,T|E, _ , _ ,E|T, _ , _ },
      { _ , _ , _ , _ , _ , _ , _ },
      { _ ,  E, O , _ ,E  , O , _ },
      { _ ,T  , _ , _ ,  T, _ , _ },
      { _ , _ , _ , _ , _ , _ , _ },
      { _ , _ , O , _ , _ , O , _ },
    };
    DO_TEST
  }

  {
    // intersect: other is top right (favoring right)
    unsigned int a[6][8] = {
      { _ , _ , _ , O , _ , O , _ },
      { _ ,T|E, _ , E , T , _ , _ },
      { _ , _ , _ , _ , _ , _ , _ },
      { _ , _ , _ , O , _ , O , _ },
      { _ , _ , _ , _ , _ , _ , _ },
      { _ ,T|E, _ , E , T , _ , _ },
    };
    DO_TEST
  }

  {
    // intersect: other is top right (favoring top)
    unsigned int a[6][8] = {
      { _ , _ , O , _ , _ , O , _ },
      { _ , _ , _ , _ , _ , _ , _ },
      { _ ,T  , _ , _ ,  T, _ , _ },
      { _ ,  E, O , _ ,E  , O , _ },
      { _ , _ , _ , _ , _ , _ , _ },
      { _ ,T|E, _ , _ ,E|T, _ , _ },
    };
    DO_TEST
  }

  {
    // intersect: other is bottom left (favoring bottom)
    unsigned int a[6][8] = {
      { _ , _ ,T|E, _ , _ ,E|T, _ },
      { _ , _ , _ , _ , _ , _ , _ },
      { _ , O ,  E, _ , O ,E  , _ },
      { _ , _ ,T  , _ , _ ,  T, _ },
      { _ , _ , _ , _ , _ , _ , _ },
      { _ , O , _ , _ , O , _ , _ },
    };
    DO_TEST
  }

  {
    // intersect: other is top left (favoring left)
    unsigned int a[6][8] = {
      { _ , O , _ ,O  , _ , _ , _ },
      { _ , _ , T ,  E, _ ,E|T, _ },
      { _ , _ , _ , _ , _ , _ , _ },
      { _ , _ , _ , _ , _ , _ , _ },
      { _ , O , _ ,O  , _ , _ , _ },
      { _ , _ , T ,  E, _ ,E|T, _ },
    };
    DO_TEST
  }

  {
    // other is fully contained in this (favoring right)
    unsigned int a[6][8] = {
      { _ ,T|E, _ , E , _ , T , _ },
      { _ , _ , _ , _ , _ , _ , _ },
      { _ , _ , _ , O , _ , O , _ },
      { _ , _ , _ , _ , _ , _ , _ },
      { _ , _ , _ , O , _ , O , _ },
      { _ ,T|E, _ , E , _ , T , _ },
    };
    DO_TEST
  }
  {
    // other is horizontally contained below
    unsigned int a[6][8] = {
      { _ ,T|E, _ , _ , _ ,E|T, _ },
      { _ , _ , _ , _ , _ , _ , _ },
      { _ ,  E, O , _ , O ,E  , _ },
      { _ ,T  , _ , _ , _ , T , _ },
      { _ , _ , _ , _ , _ , _ , _ },
      { _ , _ , O , _ , O , _ , _ },
    };
    DO_TEST
  }
  {
    // other is horizontally contained above
    unsigned int a[6][8] = {
      { _ , _ , O , _ , O , _ , _ },
      { _ , _ , _ , _ , _ , _ , _ },
      { _ ,T  , _ , _ , _ ,  T, _ },
      { _ ,  E, O , _ , O ,E  , _ },
      { _ , _ , _ , _ , _ , _ , _ },
      { _ ,T|E, _ , _ , _ ,E|T, _ },
    };
    DO_TEST
  }
  {
    // other is vertically contained right
    unsigned int a[6][8] = {
      { _ ,T|E, _ , E , T , _ , _ },
      { _ , _ , _ , O , _ , O , _ },
      { _ , _ , _ , _ , _ , _ , _ },
      { _ , _ , _ , _ , _ , _ , _ },
      { _ , _ , _ , O , _ , O , _ },
      { _ ,T|E, _ , E , T , _ , _ },
    };
    DO_TEST
  }
  {
    // other is vertically contained left
    unsigned int a[6][8] = {
      { _ , _ , T , E , _ ,E|T, _ },
      { _ , O , _ , O , _ , _ , _ },
      { _ , _ , _ , _ , _ , _ , _ },
      { _ , _ , _ , _ , _ , _ , _ },
      { _ , O , _ , O , _ , _ , _ },
      { _ , _ , T , E , _ ,E|T, _ },
    };
    DO_TEST
  }

  {
    // this is completely contained by other
    unsigned int a[6][8] = {
      { _ , O , _ , _ , _ , O , _ },
      { _ , _ , T , _ , T , _ , _ },
      { _ , _ , _ , _ , _ , _ , _ },
      { _ , _ , _ , _ , _ , _ , _ },
      { _ , _ , T , _ , T , _ , _ },
      { _ , O , _ , _ , _ , O , _ },
    };
    auto result = Generate(a);
    result.This.Rect.ExcludeWith(result.Other.Rect);
    ASSERT_TRUE(result.This.Rect.IsEmpty());
  }

  {
    // this is contained by other horizontally above
    unsigned int a[6][8] = {
      { _ , O , _ , _ , _ , O , _ },
      { _ , _ , _ , _ , _ , _ , _ },
      { _ , _ , _ , _ , _ , _ , _ },
      { _ , _ ,T  , _ ,  T, _ , _ },
      { _ , O ,  E, _ ,E  , O , _ },
      { _ , _ ,T|E, _ ,E|T, _ , _ },
    };
    DO_TEST
  }

  {
    // this is contained by other horizontally below
    unsigned int a[6][8] = {
      { _ , _ ,T|E, _ ,E|T, _ , _ },
      { _ , O ,  E, _ ,E  , O , _ },
      { _ , _ ,T  , _ ,  T, _ , _ },
      { _ , _ , _ , _ , _ , _ , _ },
      { _ , _ , _ , _ , _ , _ , _ },
      { _ , O , _ , _ , _ , O , _ },
    };
    DO_TEST
  }

  {
    // this is contained by other vertically right
    unsigned int a[6][8] = {
      { _ , _ , O , _ , _ , O , _ },
      { _ ,T|E, E , T , _ , _ , _ },
      { _ , _ , _ , _ , _ , _ , _ },
      { _ , _ , _ , _ , _ , _ , _ },
      { _ ,T|E, E , T , _ , _ , _ },
      { _ , _ , O , _ , _ , O , _ },
    };
    DO_TEST
  }
  {
    // this is contained by other vertically left
    unsigned int a[6][8] = {
      { _ , O , _ , _ , O , _ , _ },
      { _ , _ , _ , T , E ,E|T, _ },
      { _ , _ , _ , _ , _ , _ , _ },
      { _ , _ , _ , _ , _ , _ , _ },
      { _ , _ , _ , T , E ,E|T, _ },
      { _ , O , _ , _ , O , _ , _ },
    };
    DO_TEST
  }
}

