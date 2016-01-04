#include "include/Common.h"
#include <libgui/Element.h>
#include <libgui/ElementManager.h>
#include <gtest/gtest.h>

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
    auto root = make_shared<Element>();
    auto em = make_shared<ElementManager>();
    em->SetRoot(root);

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

TEST(ElementTests, InchesArithmetic)
{
    auto root = make_shared<Element>();
    auto em   = make_shared<ElementManager>();
    em->SetRoot(root);
    em->SetDpiX(100.0);

    int    index = 3;
    inches buffer(5.0);
    inches l(1.0);

    root->SetLeft(l + buffer * index);

    ASSERT_EQ(1600, root->GetLeft());
}

