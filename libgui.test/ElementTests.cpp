#include "include/Common.h"
#include <libgui/Element.h>
#include <libgui/ElementManager.h>
#include <gtest/gtest.h>
#include "libgui/Layer.h"

using namespace std;
using namespace libgui;

class TestElement: public Element
{
public:
  TestElement(Dependencies dependencies)
    : Element(dependencies)
  {
  }

  ~TestElement()
  {
    if (destructor_callback_)
    {
      destructor_callback_();
    }
  }

  void SetDestructorCallback(const function<void()>& destructor_callback)
  {
    destructor_callback_ = destructor_callback;
  }

private:
  function<void()> destructor_callback_;
};

TEST(ElementTests, WhenRemovingChildren_AllReferencesAreCleaned)
{
  auto em   = make_shared<ElementManager>();
  auto root = em->CreateLayerAbove(nullptr);

  auto child1 = root->CreateChild<Element>();

  auto child2 = root->CreateChild<TestElement>();
  bool wasDestructed = false;
  child2->SetDestructorCallback([&]() { wasDestructed = true; });

  auto grandchild1 = child2->CreateChild<Element>();
  auto grandchild2 = child2->CreateChild<Element>();
  auto grandchild3 = child2->CreateChild<Element>();

  root->RemoveChildren(Element::UpdateWhenRemoving::No);

  child2 = nullptr;

  ASSERT_EQ(true, wasDestructed);
}

TEST(ElementTests, WhenElementDisabled_ChildControlsDisabledAlso)
{
  auto em   = make_shared<ElementManager>();
  auto root = em->CreateLayerAbove(nullptr);

  root->SetTop(5);
  root->SetLeft(5);
  root->SetWidth(50);
  root->SetHeight(50);
  root->SetIsEnabled(false);

  auto child = root->CreateChild<Element>();

  child->SetTop(10);
  child->SetLeft(10);
  child->SetWidth(5);
  child->SetHeight(5);

  auto queryInfo = root->GetElementAtPoint(Point{11, 11});

  ASSERT_EQ(child.get(), queryInfo.ElementAtPoint);
  ASSERT_EQ(true, queryInfo.HasDisabledAncestor);

}