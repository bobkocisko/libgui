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
    auto root = em->AddLayerAbove(nullptr);

    auto child1 = make_shared<Element>();
    root->AddChild(child1);

    auto child2 = make_shared<TestElement>();
    root->AddChild(child2);
    bool wasDestructed = false;
    child2->SetDestructorCallback([&]() { wasDestructed = true; });

    auto grandchild1 = make_shared<Element>();
    child2->AddChild(grandchild1);
    auto grandchild2 = make_shared<Element>();
    child2->AddChild(grandchild2);
    auto grandchild3 = make_shared<Element>();
    child2->AddChild(grandchild3);

    root->RemoveChildren();

    child2 = nullptr;

    ASSERT_EQ(true, wasDestructed);
}

TEST(ElementTests, WhenElementDisabled_ChildControlsDisabledAlso)
{
    auto em   = make_shared<ElementManager>();
    auto root = em->AddLayerAbove(nullptr);

    root->SetTop(5);
    root->SetLeft(5);
    root->SetWidth(50);
    root->SetHeight(50);
    root->SetIsEnabled(false);

    auto child = make_shared<Element>();
    root->AddChild(child);

    child->SetTop(10);
    child->SetLeft(10);
    child->SetWidth(5);
    child->SetHeight(5);

    root->InitializeAll();

    auto queryInfo = root->GetElementAtPoint(Point{11, 11});

    ASSERT_EQ(child.get(), queryInfo.ElementAtPoint);
    ASSERT_EQ(true, queryInfo.HasDisabledAncestor);

}