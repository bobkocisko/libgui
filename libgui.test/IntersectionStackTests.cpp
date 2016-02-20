//
// Created by sjm-kociskobd on 2/20/16.
//

#include <libgui/IntersectionStack.h>

#include <gtest/gtest.h>

using namespace std;
using namespace libgui;

TEST(IntersectionStackTests, Test)
{
    boost::optional<Rect4> currentRegion;
    bool                   currentRegionSet;

    IntersectionStack iStack;
    iStack.SetRegionChangedCallback(
        [&currentRegion, &currentRegionSet](const boost::optional<Rect4>& region)
        {
            currentRegion    = region;
            currentRegionSet = true;
        });

    iStack.PushRegion(Rect4(1, 1, 10, 10));
    {
        ASSERT_TRUE(currentRegion == Rect4(1, 1, 10, 10));

        iStack.PopRegion();
    }
    ASSERT_FALSE(currentRegion);

    iStack.PushRegion(Rect4(1, 1, 10, 10));
    {
        ASSERT_TRUE(currentRegion == Rect4(1, 1, 10, 10));

        iStack.PushRegion(Rect4(2, 2, 12, 6));
        {
            ASSERT_TRUE(currentRegion == Rect4(2, 2, 10, 6));

            iStack.PushRegion(Rect4(12, 12, 20, 20));
            {
                ASSERT_TRUE(currentRegion.get().IsEmpty());

                iStack.PopRegion();
            }
            ASSERT_TRUE(currentRegion == Rect4(2, 2, 10, 6));

            // The following two operations shouldn't modify the region
            currentRegionSet = false;

            iStack.PushRegion(Rect4(1, 1, 10, 10));
            {
                ASSERT_FALSE(currentRegionSet);
                ASSERT_TRUE(currentRegion == Rect4(2, 2, 10, 6));

                iStack.PopRegion();
            }
            ASSERT_FALSE(currentRegionSet);
            ASSERT_TRUE(currentRegion == Rect4(2, 2, 10, 6));

            // Now we should have more modifications
            iStack.PopRegion();
        }
        ASSERT_TRUE(currentRegionSet);
        ASSERT_TRUE(currentRegion == Rect4(1, 1, 10, 10));

        iStack.PopRegion();
    }
    ASSERT_FALSE(currentRegion);
}
