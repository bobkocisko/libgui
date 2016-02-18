//
// Created by Bob Kocisko on 2/18/16.
//




/* Some of the following code has been copied from the freetype-gl project
 * which is licensed as follows:
 * -----------------------------------------------------------------------
 *  * Copyright 2011,2012 Nicolas P. Rougier. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 *  1. Redistributions of source code must retain the above copyright notice,
 *     this list of conditions and the following disclaimer.
 *
 *  2. Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY NICOLAS P. ROUGIER ''AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO
 * EVENT SHALL NICOLAS P. ROUGIER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
 * INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * The views and conclusions contained in the software and documentation are
 * those of the authors and should not be interpreted as representing official
 * policies, either expressed or implied, of Nicolas P. Rougier.
 *
 */

#include "freetype-gl.h"
#include "vertex-buffer.h"
#include "text-buffer.h"
#include "markup.h"
#include "shader.h"
#include "mat4.h"

#include <GLFW/glfw3.h>
#include <vec234.h>
#include <stdio.h>

typedef struct
{
    float x, y, z;
    float r, g, b, a;
} vertex_t;

mat4 model, view, projection;

GLuint shader;

vertex_buffer_t* lines_buffer;

static void cursor_position_callback(GLFWwindow* window, double xpos, double ypos)
{

}

static void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
    if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS)
    { ;
    }
}

void display(GLFWwindow* window)
{
    glClearColor(0.40, 0.40, 0.45, 1.00);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
    glBlendColor(1.0, 1.0, 1.0, 1.0);
    glUseProgram(shader);
    {
        glUniformMatrix4fv(glGetUniformLocation(shader, "model"),
                           1, 0, model.data);
        glUniformMatrix4fv(glGetUniformLocation(shader, "view"),
                           1, 0, view.data);
        glUniformMatrix4fv(glGetUniformLocation(shader, "projection"),
                           1, 0, projection.data);
        vertex_buffer_render(lines_buffer, GL_LINES);
    }


    glfwSwapBuffers(window);

}

void reshape(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);

    // Flip along the y axis so that we can use top-to-bottom coordinates
    mat4_set_orthographic(&projection, 0, width, height, 0, -1, 1);
}

void init()
{
    shader = shader_load("shaders/v3f-c4f.vert",
                         "shaders/v3f-c4f.frag");

    float left   = 20;
    float right  = 100;
    float top    = 20;
    float bottom = 100;


    lines_buffer = vertex_buffer_new("vertex:3f,color:4f");
    vertex_t vertices[] = {{left - 10,  top,         0, 0, 0, 0, 1}, // top
                           {right + 10, top,         0, 0, 0, 0, 1},

                           {left - 10,  bottom,      0, 0, 0, 0, 1}, // bottom
                           {right + 10, bottom,      0, 0, 0, 0, 1},

                           {left,       top - 10,    0, 0, 0, 0, 1}, // left
                           {left,       bottom + 10, 0, 0, 0, 0, 1},
                           {right,      top - 10,    0, 0, 0, 0, 1}, // right
                           {right,      bottom + 10, 0, 0, 0, 0, 1}};
    GLuint   indices[]  = {0, 1, 2, 3, 4, 5, 6, 7};
    vertex_buffer_push_back(lines_buffer, vertices, 8, indices, 8);

    mat4_set_identity(&projection);
    mat4_set_identity(&model);
    mat4_set_identity(&view);

}

int main(void)
{
    GLFWwindow* window;

    /* Initialize the library */
    if (!glfwInit())
    {
        return -1;
    }

    /* Create a windowed mode window and its OpenGL context */
    window = glfwCreateWindow(1, 1, "Hello World", NULL, NULL);
    if (!window)
    {
        printf("Could not create window.  Exiting...\n");
        glfwTerminate();
        return -1;
    }

    /* Make the window's context current */
    glfwMakeContextCurrent(window);

    glfwSetFramebufferSizeCallback(window, reshape);
    glfwSetWindowRefreshCallback(window, display);
    glfwSetCursorPosCallback(window, cursor_position_callback);
    glfwSetMouseButtonCallback(window, mouse_button_callback);

    GLenum err = glewInit();
    if (GLEW_OK != err)
    {
        printf("Could not load GLEW.  Exiting...\n");
        glfwTerminate();
        return -1;
    }

    init();

    glfwSetWindowSize(window, 640, 480);
    glfwShowWindow(window);

    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window))
    {
        /* Poll for and process events */
        glfwWaitEvents();

        /* Update Display */
        display(window);
    }

    glfwTerminate();
    return 0;
}

void InitElements()
{
    DrawingManager::Get()->SetStartClippingCallback([&](Rect4 r)
                                                    {
                                                        D2D1_RECT_F clipRect;
                                                        clipRect.left   = r.left;
                                                        clipRect.right  = r.right;
                                                        clipRect.top    = r.top;
                                                        clipRect.bottom = r.bottom;
                                                        render_target_->PushAxisAlignedClip(clipRect,
                                                                                            D2D1_ANTIALIAS_MODE_ALIASED);
                                                    });
    DrawingManager::Get()->SetStopClippingCallback([&]()
                                                   {
                                                       render_target_->PopAxisAlignedClip();
                                                   });

// Main view model
    auto itemsVm = make_shared<ItemsViewModel>();

// Set up the root element to match the window size
    auto root = make_shared<Element>();
    m_elementManager->SetRoot(root);
    root->SetElementManager(m_elementManager);
    root->SetViewModel(itemsVm);
    root->SetArrangeCallback([&](shared_ptr <Element> e)
                             {
                                 D2D1_SIZE_F rtSize = render_target_->GetSize();
                                 e->SetLeft(0);
                                 e->SetRight(rtSize.width);
                                 e->SetTop(0);
                                 e->SetBottom(rtSize.height);
                             });


// Build the screen elements
    auto header = make_shared<Element>();
    {
        root->AddChild(header);
        header->SetArrangeCallback([&](shared_ptr <Element> e)
                                   {
                                       auto p = e->GetParent();
                                       e->SetLeft(p->GetLeft());
                                       e->SetRight(p->GetRight());
                                       e->SetTop(p->GetTop());
                                       e->SetHeight(41.0);
                                   });
        header->SetDrawCallback([&](shared_ptr <Element> e)
                                {
                                    DrawBorder(1.0, SharedResources::Get()->GrayBrush, e);
                                });
    }

    auto footer = make_shared<Element>();
    {
        root->AddChild(footer);
        footer->SetArrangeCallback([&](shared_ptr <Element> e)
                                   {
                                       auto p = e->GetParent();
                                       e->SetLeft(p->GetLeft());
                                       e->SetRight(p->GetRight());
                                       e->SetBottom(p->GetBottom());
                                       e->SetHeight(41.0);
                                   });
        footer->SetDrawCallback([&](shared_ptr <Element> e)
                                {
                                    DrawBorder(1.0, SharedResources::Get()->GrayBrush, e);
                                });
    }

    auto grid_scroll_width = 32;

    auto grid = make_shared<Grid>();
    {
        root->AddChild(grid);
        grid->SetArrangeCallback([=](shared_ptr <Element> e)
                                 {
                                     auto p = e->GetParent();
                                     e->SetTop(header->GetBottom());
                                     e->SetBottom(footer->GetTop());

// CanScroll method depends on the height to be specified first
                                     auto can_scroll = dynamic_pointer_cast<Grid>(e)->CanScroll();
                                     if (can_scroll)
                                     {
                                         e->SetRight(p->GetRight() - grid_scroll_width - 10);
                                     }
                                     else
                                     {
                                         e->SetRight(p->GetRight() - 10);
                                     }
                                     e->SetLeft(p->GetLeft() + grid_scroll_width);
                                 });

        grid->SetTopPadding(5);
        grid->SetBottomPadding(5);

        grid->SetCellHeight(100);
        grid->SetColumns(3);
        grid->SetItemsProvider(itemsVm);

        grid->SetCellCreateCallback(
            [&]()
            {
                auto cell_background = make_shared<Element>();
                cell_background->SetArrangeCallback(
                    [](shared_ptr <Element> e)
                    {
                        auto p = e->GetParent();
                        e->SetLeft(p->GetLeft() + 10);
                        e->SetRight(p->GetRight());
                        e->SetTop(p->GetTop() + 5);
                        e->SetBottom(p->GetBottom() - 5);
                    });

                cell_background->SetDrawCallback(
                    [&](shared_ptr <Element> e)
                    {
                        D2D1_ROUNDED_RECT rr;
                        rr.rect.left   = e->GetLeft();
                        rr.rect.right  = e->GetRight();
                        rr.rect.top    = e->GetTop();
                        rr.rect.bottom = e->GetBottom();
                        rr.radiusX     = 4.5;
                        rr.radiusY     = 4.5;

                        render_target_->FillRoundedRectangle(rr,
                                                             SharedResources::Get()
                                                                 ->LightGrayBrush);
                    });
                {
                    auto text = make_shared<Element>();
                    cell_background->AddChild(text);
                    text->SetDrawCallback(
                        [&](shared_ptr <Element> e)
                        {
                            auto        ivm =
                                            dynamic_pointer_cast<ItemViewModel>(
                                                e->GetViewModel());
                            D2D1_RECT_F rect;
                            rect.left   = e->GetLeft();
                            rect.right  = e->GetRight();
                            rect.top    = e->GetTop();
                            rect.bottom = e->GetBottom();
                            render_target_->DrawTextW(
                                ivm->GetName().c_str(),
                                ivm->GetName().length(),
                                m_pTextFormat,
                                rect,
                                SharedResources::Get()->DarkGrayBrush);
                        });
                }
                return cell_background;
            });
    }

    auto grid_scroll = make_shared<Scrollbar>(grid);
    {
        root->AddChild(grid_scroll);
        grid_scroll->Initialize();
        grid_scroll->SetArrangeCallback(
            [=](shared_ptr <Element> e)
            {
                auto p          = e->GetParent();
                auto can_scroll = dynamic_pointer_cast<Grid>(grid)->CanScroll();

                if (can_scroll)
                {
                    e->SetWidth(grid_scroll_width);
                    e->SetRight(p->GetRight());
                    e->SetTop(header->GetBottom());
                    e->SetBottom(footer->GetTop());
                }
                else
                {
                    e->SetIsVisible(false);
                }
            });

        grid_scroll->SetDrawCallback(
            [&](shared_ptr <Element> e)
            {
// Draw the scrollbar background
                auto        sr = SharedResources::Get();
                D2D1_RECT_F rect;
                rect.left   = e->GetLeft();
                rect.right  = e->GetRight();
                rect.top    = e->GetTop();
                rect.bottom = e->GetBottom();
                render_target_->FillRectangle(rect, sr->LightGrayBrush);
            });

        auto track = grid_scroll->GetTrack();
        track->SetArrangeCallback(
            [=](shared_ptr <Element> e)
            {
                auto p = e->GetParent();
                e->SetLeft(p->GetLeft() + 5);
                e->SetRight(p->GetRight() - 5);
                e->SetTop(p->GetTop() + 5);
                e->SetBottom(p->GetBottom() - 5);
            });

        auto thumb = grid_scroll->GetThumb();
        thumb->SetDrawCallback(
            [&](shared_ptr <Element> e)
            {
                auto        sr = SharedResources::Get();
                D2D1_RECT_F rect;
                rect.left   = e->GetLeft();
                rect.right  = e->GetRight();
                rect.top    = e->GetTop();
                rect.bottom = e->GetBottom();
                render_target_->FillRectangle(rect, sr->MediumGrayBrush);
            });

    }

    auto slider = make_shared<Slider>();
    {
        root->AddChild(slider);
        slider->Initialize();
        slider->SetThumbHeight(50);
        slider->SetArrangeCallback(
            [=](shared_ptr <Element> e)
            {
                auto p = e->GetParent();
                e->SetWidth(grid_scroll_width);
                e->SetLeft(p->GetLeft());
                e->SetTop(header->GetBottom());
                e->SetBottom(footer->GetTop());
            });

        slider->SetDrawCallback(
            [&](shared_ptr <Element> e)
            {
// Draw the scrollbar background
                auto        sr = SharedResources::Get();
                D2D1_RECT_F rect;
                rect.left   = e->GetLeft();
                rect.right  = e->GetRight();
                rect.top    = e->GetTop();
                rect.bottom = e->GetBottom();
                render_target_->FillRectangle(rect, sr->LightGrayBrush);
            });

        auto slider_track = slider->GetTrack();
        slider_track->SetArrangeCallback(
            [=](shared_ptr <Element> e)
            {
                auto p = e->GetParent();
                e->SetLeft(p->GetLeft() + 5);
                e->SetRight(p->GetRight() - 5);
                e->SetTop(p->GetTop() + 5);
                e->SetBottom(p->GetBottom() - 5);
            });

        auto slider_thumb = slider->GetThumb();
        slider_thumb->SetDrawCallback(
            [&](shared_ptr <Element> e)
            {
                auto        sr = SharedResources::Get();
                D2D1_RECT_F rect;
                rect.left   = e->GetLeft();
                rect.right  = e->GetRight();
                rect.top    = e->GetTop();
                rect.bottom = e->GetBottom();
                render_target_->FillRectangle(rect, sr->MediumGrayBrush);
            });
    }

    auto sliderValueText = make_shared<Element>();
    {
        footer->AddChild(sliderValueText);
        sliderValueText->SetDrawCallback(
            [=](shared_ptr <Element> e)
            {
                wstring     valueString = to_wstring(slider->GetValue());
                D2D1_RECT_F rect;
                rect.left   = e->GetLeft();
                rect.right  = e->GetRight();
                rect.top    = e->GetTop();
                rect.bottom = e->GetBottom();
                render_target_->DrawTextW(
                    valueString.c_str(), valueString.length(),
                    m_pTextFormat, rect, SharedResources::Get()->DarkGrayBrush);

            });
    }
}

void App::DrawBorder(double strokeWidth, ID2D1Brush* brush, shared_ptr <Element> e)
{
    DrawBorder(LEFT | TOP | RIGHT | BOTTOM, strokeWidth, brush, e);
}

void App::DrawBorder(int sides, double strokeWidth, ID2D1Brush* brush, shared_ptr <Element> e)
{
    double halfStrokeWidth = strokeWidth / 2;

    if ((LEFT | TOP | RIGHT | BOTTOM) == sides)
    {
        // All sides: use a rectangle and draw its border
        D2D1_RECT_F rect = {};
        rect.left   = e->GetLeft() + halfStrokeWidth;
        rect.right  = e->GetRight() - halfStrokeWidth;
        rect.top    = e->GetTop() + halfStrokeWidth;
        rect.bottom = e->GetBottom() - halfStrokeWidth;
        render_target_->DrawRectangle(rect, brush, strokeWidth);
    }
    else
    {
        D2D1_RECT_F outerRect;
        outerRect.left   = e->GetLeft();
        outerRect.right  = e->GetRight();
        outerRect.top    = e->GetTop();
        outerRect.bottom = e->GetBottom();

        D2D1_RECT_F rect = {};
        rect.left   = outerRect.left + halfStrokeWidth;
        rect.right  = outerRect.right - halfStrokeWidth;
        rect.top    = outerRect.top + halfStrokeWidth;
        rect.bottom = outerRect.bottom - halfStrokeWidth;
        // One or more side: use lines
        if (0 < (sides & LEFT))
        {
            render_target_->DrawLine(
                D2D1::Point2F(rect.left, outerRect.top),
                D2D1::Point2F(rect.left, outerRect.bottom),
                brush, strokeWidth);
        }
        if (0 < (sides & RIGHT))
        {
            render_target_->DrawLine(
                D2D1::Point2F(rect.right, outerRect.top),
                D2D1::Point2F(rect.right, outerRect.bottom),
                brush, strokeWidth);
        }
        if (0 < (sides & TOP))
        {
            double left  = outerRect.left;
            double right = outerRect.right;
            if (0 < (sides & LEFT))
            {
                left += strokeWidth;
            }
            if (0 < (sides & RIGHT))
            {
                right -= strokeWidth;
            }
            render_target_->DrawLine(
                D2D1::Point2F(left, rect.top),
                D2D1::Point2F(right, rect.top),
                brush, strokeWidth);
        }
        if (0 < (sides & BOTTOM))
        {
            double left  = outerRect.left;
            double right = outerRect.right;
            if (0 < (sides & LEFT))
            {
                left += strokeWidth;
            }
            if (0 < (sides & RIGHT))
            {
                right -= strokeWidth;
            }
            render_target_->DrawLine(
                D2D1::Point2F(left, rect.bottom),
                D2D1::Point2F(right, rect.bottom),
                brush, strokeWidth);
        }
    }
}
