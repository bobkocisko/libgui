//
// Created by Bob Kocisko on 2/3/16.
//

#include "libgui/Types.h"
#include "libgui/Element.h"
#include "libgui/ElementManager.h"
#include "libgui/Layer.h"

#include <gtest/gtest.h>
#include <gmock/gmock.h>
using namespace libgui;
using namespace std;

using ::testing::Return;

TEST(TypesTest, InchesArithmetic)
{
  auto em   = make_shared<ElementManager>();
  auto root = em->AddLayerAbove(nullptr);
  em->SetDpiX(100.0);
  em->SetDpiY(100.0);

  int    index = 3;
  Inches buffer(5.0);
  Inches l(1.0);

  root->SetLeft(l + buffer * index);

  ASSERT_EQ(1600, root->GetLeft());

  auto top = 1.0 * inches;
  root->SetTop(top / 2);

  ASSERT_EQ(50, root->GetTop());
}

TEST(TypesTest, ConvertInchesToPixels)
{
  auto em      = make_shared<ElementManager>();
  auto element = em->AddLayerAbove(nullptr);
  em->SetDpiX(100.0);
  em->SetDpiY(200.0);

  element->SetTop(5 * inches);
  element->SetLeft(3.0 * inches);

  ASSERT_EQ(1000.0, element->GetTop());
  ASSERT_EQ(300.0, element->GetLeft());
}

TEST(TypesTest, CombineInchPixelMath)
{
  auto em      = make_shared<ElementManager>();
  auto element = em->AddLayerAbove(nullptr);
  em->SetDpiX(100.0);
  em->SetDpiY(200.0);

  element->SetLeft(50);
  element->SetRight(element->GetLeft() + 3 * inches);

  ASSERT_EQ(300.0, element->GetWidth());

  element->SetTop(100);
  element->SetBottom(element->GetTop() + 2.5 * inches);

  ASSERT_EQ(500, element->GetHeight());

  ASSERT_EQ(200, element->GetHPixels(2 * inches));
  ASSERT_EQ(400, element->GetVPixels(2 * inches));

  ASSERT_EQ(25, element->GetLeft() - 0.25 * inches);
}

TEST(TypesTest, LocationConversions)
{
  HPixels x(50, 500);
  VPixels y(20, 200);

  Location location(x, y);

  ASSERT_EQ(50, location.x);
  ASSERT_EQ(20, location.y);

  auto em      = make_shared<ElementManager>();
  auto element = em->AddLayerAbove(nullptr);
  em->SetDpiX(100.0);
  em->SetDpiY(200.0);

  element->SetLeft(500);

  element->SetTop(200);

  Location l2(element->GetLeft(), element->GetTop());
  ASSERT_EQ(500, l2.x);
  ASSERT_EQ(200, l2.y);
}