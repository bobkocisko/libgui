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

#include "libgui/Element.h"
#include "libgui/ElementManager.h"
#include "libgui/Layer.h"
#include "libgui/Grid.h"
#include "libgui/Scrollbar.h"
#include "libgui/Slider.h"
#include "libgui/IntersectionStack.h"

#include "freetype-gl.h"
#include "vertex-buffer.h"
#include "text-buffer.h"
#include "markup.h"
#include "shader.h"
#include "mat4.h"

#include <GLFW/glfw3.h>
#include <vec234.h>
#include <stdio.h>
#include <stack>
#include <boost/optional.hpp>

#include "include/ItemsViewModel.h"

using libgui::ElementManager;
using libgui::Rect4;
using libgui::Element;
using libgui::Grid;
using libgui::Scrollbar;
using libgui::Slider;

typedef struct
{
    float x, y, z;
    float r, g, b, a;
} vertex_t;

mat4 model, view, projection;

GLuint shader;

text_buffer_t* text_buffer;

markup_t markup;

std::shared_ptr<ElementManager> elementManager;

libgui::IntersectionStack clipStack;

bool isClipping = false;

#define GLERR(EXP) { EXP; CheckOpenGLError(#EXP); }

void CheckOpenGLError(const char* expression)
{
    auto result = glGetError();
    if (GL_NO_ERROR != result)
    {
        throw runtime_error(std::string("OpenGL error for statement ") +
                            expression + ": " + (char*) gluErrorString(result));
    }
}

auto initialWindowWidth = 640;

auto initialWindowHeight = 480;

int windowWidth = -1;

int windowHeight = -1;

bool firstWindowRefresh = true;

void FillRectangle(double left, double top, double right, double bottom, int r, int g, int b);
void OutlineRectangle(double left, double top, double right, double bottom, int r, int g, int b, double lineWidth);
void DrawText(double centerX, double centerY, std::string text);

void InitElements()
{
    elementManager = make_shared<ElementManager>();

    // OpenGL does not support stacking scissor regions, so we use a helper class
    // IntersectionStack to manage the stack and then we forward the final current
    // result to OpenGL's glScissor and glEnable/glDisable calls

    elementManager->SetPushClipCallback(
        [](const Rect4& newRegion)
        {
            clipStack.PushRegion(newRegion);
        });

    elementManager->SetPopClipCallback(
        []()
        {
            clipStack.PopRegion();
        });

    clipStack.SetRegionChangedCallback(
        [](const boost::optional<Rect4>& regionOpt)
        {
            if (regionOpt)
            {
                auto region = regionOpt.get();

                auto left   = int(round(region.left));
                auto bottom = int(round(region.bottom));
                auto width  = int(round(region.right)) - left;
                auto height = bottom - int(round(region.top));
                GLERR(glScissor(left, windowHeight - bottom, width, height));

                // Enable the scissor test if not already enabled
                if (!isClipping)
                {
                    GLERR(glEnable(GL_SCISSOR_TEST));
                    isClipping = true;
                }
            }
            else
            {
                // Disable the scissor test if not already disabled
                if (isClipping)
                {
                    GLERR(glDisable(GL_SCISSOR_TEST));
                    isClipping = false;
                }
            }
        });

    // Main view model
    auto itemsVm = make_shared<ItemsViewModel>();

    // Set up the root element to match the window size
    auto root = elementManager->AddLayer();
    root->SetViewModel(itemsVm);
    root->SetArrangeCallback(
        [](shared_ptr<Element> e)
        {
            printf("Arranging root with size %d, %d\n", windowWidth, windowHeight);
            fflush(stdout);
            e->SetLeft(0);
            e->SetRight(windowWidth);
            e->SetTop(0);
            e->SetBottom(windowHeight);
        });


    // Build the screen elements
    auto header = make_shared<Element>();
    {
        root->AddChild(header);
        header->SetArrangeCallback(
            [](shared_ptr<Element> e)
            {
                auto p = e->GetParent();
                e->SetLeft(p->GetLeft());
                e->SetRight(p->GetRight());
                e->SetTop(p->GetTop());
                e->SetHeight(41.0);
            });
        header->SetDrawCallback(
            [](Element* e, const boost::optional<Rect4>& redrawRegion)
            {
                OutlineRectangle(e->GetLeft(), e->GetTop(), e->GetRight(), e->GetBottom(), 0xAB, 0xAB, 0xAB, 1.0);
            });
    }

    auto footer = make_shared<Element>();
    {
        root->AddChild(footer);
        footer->SetArrangeCallback(
            [](shared_ptr<Element> e)
            {
                auto p = e->GetParent();
                e->SetLeft(p->GetLeft());
                e->SetRight(p->GetRight());
                e->SetBottom(p->GetBottom());
                e->SetHeight(41.0);
            });
        footer->SetDrawCallback(
            [](Element* e, const boost::optional<Rect4>& redrawRegion)
            {
                OutlineRectangle(e->GetLeft(), e->GetTop(), e->GetRight(), e->GetBottom(), 0xAB, 0xAB, 0xAB, 1.0);
            });
    }

    auto gridScrollWidth = 32;

    auto grid = make_shared<Grid>();
    {
        root->AddChild(grid);
        grid->InitializeThis();
        grid->SetArrangeCallback(
            [header, footer, gridScrollWidth](shared_ptr<Element> e)
            {
                auto p = e->GetParent();
                e->SetTop(header->GetBottom());
                e->SetBottom(footer->GetTop());

                // CanScroll method depends on the height to be specified first
                auto can_scroll = dynamic_pointer_cast<Grid>(e)->CanScroll();
                if (can_scroll)
                {
                    e->SetRight(p->GetRight() - gridScrollWidth - 10);
                }
                else
                {
                    e->SetRight(p->GetRight() - 10);
                }
                e->SetLeft(p->GetLeft() + gridScrollWidth);
            });

        grid->SetTopPadding(5);
        grid->SetBottomPadding(5);

        grid->SetCellHeight(100);
        grid->SetColumns(3);
        grid->SetItemsProvider(itemsVm);

        grid->SetCellCreateCallback(
            []()
            {
                auto cell_background = make_shared<Element>();
                cell_background->SetArrangeCallback(
                    [](shared_ptr<Element> e)
                    {
                        auto p = e->GetParent();
                        e->SetLeft(p->GetLeft() + 10);
                        e->SetRight(p->GetRight());
                        e->SetTop(p->GetTop() + 5);
                        e->SetBottom(p->GetBottom() - 5);
                    });

                cell_background->SetDrawCallback(
                    [](Element* e, const boost::optional<Rect4>& redrawRegion)
                    {
                        // Rounded at 4.5 px radius would be better
                        FillRectangle(e->GetLeft(), e->GetTop(), e->GetRight(), e->GetBottom(), 0xEB, 0xEB, 0xEB);
                    });
                {
                    auto text = make_shared<Element>();
                    cell_background->AddChild(text);
                    text->SetDrawCallback(
                        [](Element* e, const boost::optional<Rect4>& redrawRegion)
                        {
                            auto ivm = dynamic_pointer_cast<ItemViewModel>(e->GetViewModel());
                            DrawText(e->GetCenterX(), e->GetCenterY(), ivm->GetName());
                        });
                }
                return cell_background;
            });
    }

    auto grid_scroll = make_shared<Scrollbar>(grid);
    {
        root->AddChild(grid_scroll);
        grid_scroll->InitializeThis();
        grid_scroll->SetArrangeCallback(
            [grid, gridScrollWidth, header, footer](shared_ptr<Element> e)
            {
                auto p          = e->GetParent();
                auto can_scroll = dynamic_pointer_cast<Grid>(grid)->CanScroll();

                if (can_scroll)
                {
                    e->SetWidth(gridScrollWidth);
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
            [](Element* e, const boost::optional<Rect4>& redrawRegion)
            {
                // Draw the scrollbar background
                FillRectangle(e->GetLeft(), e->GetTop(), e->GetRight(), e->GetBottom(), 0xCD, 0xCD, 0xCD);
            });

        auto track = grid_scroll->GetTrack();
        track->SetArrangeCallback(
            [](shared_ptr<Element> e)
            {
                auto p = e->GetParent();
                e->SetLeft(p->GetLeft() + 5);
                e->SetRight(p->GetRight() - 5);
                e->SetTop(p->GetTop() + 5);
                e->SetBottom(p->GetBottom() - 5);
            });

        auto thumb = grid_scroll->GetThumb();
        thumb->SetDrawCallback(
            [](Element* e, const boost::optional<Rect4>& redrawRegion)
            {
                FillRectangle(e->GetLeft(), e->GetTop(), e->GetRight(), e->GetBottom(), 0x77, 0x77, 0x77);
            });

    }

    auto slider = make_shared<Slider>();
    {
        root->AddChild(slider);
        slider->InitializeThis();
        slider->SetThumbHeight(50);
        slider->SetArrangeCallback(
            [gridScrollWidth, header, footer](shared_ptr<Element> e)
            {
                auto p = e->GetParent();
                e->SetWidth(gridScrollWidth);
                e->SetLeft(p->GetLeft());
                e->SetTop(header->GetBottom());
                e->SetBottom(footer->GetTop());
            });

        slider->SetDrawCallback(
            [](Element* e, const boost::optional<Rect4>& redrawRegion)
            {
                // Draw the scrollbar background
                FillRectangle(e->GetLeft(), e->GetTop(), e->GetRight(), e->GetBottom(), 0xCD, 0xCD, 0xCD);
            });

        auto slider_track = slider->GetTrack();
        slider_track->SetArrangeCallback(
            [](shared_ptr<Element> e)
            {
                auto p = e->GetParent();
                e->SetLeft(p->GetLeft() + 5);
                e->SetRight(p->GetRight() - 5);
                e->SetTop(p->GetTop() + 5);
                e->SetBottom(p->GetBottom() - 5);
            });

        auto slider_thumb = slider->GetThumb();
        slider_thumb->SetDrawCallback(
            [](Element* e, const boost::optional<Rect4>& redrawRegion)
            {
                FillRectangle(e->GetLeft(), e->GetTop(), e->GetRight(), e->GetBottom(), 0x77, 0x77, 0x77);
            });
    }

    auto sliderValueText = make_shared<Element>();
    {
        footer->AddChild(sliderValueText);
        sliderValueText->SetDrawCallback(
            [slider](Element* e, const boost::optional<Rect4>& redrawRegion)
            {
                std::string valueString = std::to_string(slider->GetValue());
                DrawText(e->GetCenterX(), e->GetCenterY(), valueString); // Dark gray
            });
    }

    root->InitializeAll();
}

static void cursor_position_callback(GLFWwindow* window, double xpos, double ypos)
{

}

static void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
    if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS)
    { ;
    }
}

void FillRectangle(double left, double top, double right, double bottom, int r, int g, int b)
{
    auto rf = float(r) / 255.0;
    auto gf = float(g) / 255.0;
    auto bf = float(b) / 255.0;

    vertex_buffer_t* buffer;
    buffer = vertex_buffer_new("vertex:3f,color:4f");
    vertex_t vertices[] = {{left,  top,    0, rf, gf, bf, 1},  // 0
                           {right, top,    0, rf, gf, bf, 1},  // 1
                           {right, bottom, 0, rf, gf, bf, 1},  // 2
                           {left,  bottom, 0, rf, gf, bf, 1}}; // 3

    GLuint indices[] = {3, 0, 2, 1};
    vertex_buffer_push_back(buffer, vertices, 4, indices, 4);

    GLERR(glUseProgram(shader));
    {
        GLERR(glUniformMatrix4fv(glGetUniformLocation(shader, "model"),
                                 1, 0, model.data));
        GLERR(glUniformMatrix4fv(glGetUniformLocation(shader, "view"),
                                 1, 0, view.data));
        GLERR(glUniformMatrix4fv(glGetUniformLocation(shader, "projection"),
                                 1, 0, projection.data));
        vertex_buffer_render(buffer, GL_TRIANGLE_STRIP);
    }

    vertex_buffer_delete(buffer);
}

void OutlineRectangle(double left, double top, double right, double bottom, int r, int g, int b, double lineWidth)
{
    auto rf = float(r) / 255.0;
    auto gf = float(g) / 255.0;
    auto bf = float(b) / 255.0;

    lineWidth = round(lineWidth);
    GLERR(glLineWidth((float) lineWidth));
    auto halfLineWidth = lineWidth / 2;

    auto x1 = float(round(left) + halfLineWidth);
    auto x2 = float(round(right) - halfLineWidth);
    auto y1 = float(round(top) + halfLineWidth);
    auto y2 = float(round(bottom) - halfLineWidth);

    vertex_buffer_t* buffer;
    buffer = vertex_buffer_new("vertex:3f,color:4f");
    vertex_t vertices[] = {{x1, y1, 0, rf, gf, bf, 1},  // 0
                           {x2, y1, 0, rf, gf, bf, 1},  // 1
                           {x2, y2, 0, rf, gf, bf, 1},  // 2
                           {x1, y2, 0, rf, gf, bf, 1}}; // 3
    GLuint   indices[]  = {0, 1, 2, 3};
    vertex_buffer_push_back(buffer, vertices, 4, indices, 4);

    GLERR(glUseProgram(shader));
    {
        GLERR(glUniformMatrix4fv(glGetUniformLocation(shader, "model"),
                                 1, 0, model.data));
        GLERR(glUniformMatrix4fv(glGetUniformLocation(shader, "view"),
                                 1, 0, view.data));
        GLERR(glUniformMatrix4fv(glGetUniformLocation(shader, "projection"),
                                 1, 0, projection.data));
        vertex_buffer_render(buffer, GL_LINE_LOOP);
    }

    vertex_buffer_delete(buffer);
}

void DrawText(double centerX, double centerY, std::string text)
{

    text_buffer_clear(text_buffer);

    vec2 pen = {{0, 0}};
    text_buffer_add_text(text_buffer, &pen, &markup, text.c_str(), text.length());

    text_buffer_align(text_buffer, &pen, ALIGN_CENTER);

    vec4 bounds = text_buffer_get_bounds(text_buffer, &pen);

    Size currentSize;
    currentSize.width  = bounds.width;
    currentSize.height = bounds.height;

    float x = float(centerX) - (currentSize.width / 2);
    float y = float(centerY) - (currentSize.height / 2);

    // Flip the text so it looks right inside a flipped world
    mat4_set_scaling(&model, 1, -1, 1);

    // Now translate to the appropriate location
    mat4_translate(&model, round(x), round(y), 0);

    glUseProgram(text_buffer->shader);
    {
        glUniformMatrix4fv(glGetUniformLocation(text_buffer->shader, "model"),
                           1, 0, model.data);
        glUniformMatrix4fv(glGetUniformLocation(text_buffer->shader, "view"),
                           1, 0, view.data);
        glUniformMatrix4fv(glGetUniformLocation(text_buffer->shader, "projection"),
                           1, 0, projection.data);
        text_buffer_render(text_buffer);
    }
}

void display(GLFWwindow* window)
{
    auto& redrawnRegionOpt = elementManager->GetRedrawnRegion();
    if (redrawnRegionOpt)
    {
        printf("swapping buffers\n");
        fflush(stdout);

        // Something has changed since the last time anything was redrawn
        glfwSwapBuffers(window);

        // Copy from back to front buffer just the region that changed
        // so that both buffers are identical

        auto& redrawnRegion = redrawnRegionOpt.get();

        auto left   = int(round(redrawnRegion.left));
        auto top    = int(round(redrawnRegion.top));
        auto right  = int(round(redrawnRegion.right));
        auto bottom = int(round(redrawnRegion.bottom));

        GLERR(glBlitFramebuffer(left, bottom, right, top,
                                left, bottom, right, top,
                                GL_COLOR_BUFFER_BIT, GL_NEAREST));

        // Clear the redrawn region for next time
        elementManager->ClearRedrawnRegion();
    }

}

void RefreshEntireWindow(GLFWwindow* window, int width, int height)
{
    GLERR(glViewport(0, 0, width, height));

    // Flip along the y axis so that we can use top-to-bottom coordinates
    mat4_set_orthographic(&projection, 0, width, height, 0, -1, 1);

    GLERR(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));

    windowWidth  = width;
    windowHeight = height;
    elementManager->ArrangeAndDrawAll();

}

void reshape(GLFWwindow* window, int width, int height)
{
    if (width != windowWidth || height != windowHeight)
    {
        printf("Reshaping with size %d, %d\n", width, height);
        fflush(stdout);

        RefreshEntireWindow(window, width, height);
    }
}

void windowRefresh(GLFWwindow* window)
{
    if (firstWindowRefresh)
    {
        printf("First window refresh\n");
        fflush(stdout);

        RefreshEntireWindow(window, initialWindowWidth, initialWindowHeight);

        firstWindowRefresh = false;
    }
}

void init()
{
    text_buffer = text_buffer_new(LCD_FILTERING_OFF,
                                  "shaders/text.vert",
                                  "shaders/text.frag");
    vec4 black  = {{0.0, 0.0, 0.0, 1.0}};
    text_buffer->base_color = black;

    vec4 none = {{1.0, 1.0, 1.0, 0.0}};
    markup.family              = (char*) "fonts/Vera.ttf";
    markup.size                = 9.0;
    markup.bold                = 0;
    markup.italic              = 0;
    markup.rise                = 0.0;
    markup.spacing             = 0.0;
    markup.gamma               = 1.0;
    markup.foreground_color    = black;
    markup.background_color    = none;
    markup.underline           = 0;
    markup.underline_color     = black;
    markup.overline            = 0;
    markup.overline_color      = black;
    markup.strikethrough       = 0;
    markup.strikethrough_color = black;
    markup.font                = 0;

    shader = shader_load("shaders/v3f-c4f.vert",
                         "shaders/v3f-c4f.frag");

    mat4_set_identity(&projection);
    mat4_set_identity(&model);
    mat4_set_identity(&view);

    // When we blit later on, this tells opengl where to blit from and to
    GLERR(glReadBuffer(GL_FRONT_LEFT));
    GLERR(glDrawBuffer(GL_BACK_LEFT));

    GLERR(glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA));
    GLERR(glBlendColor(1.0, 1.0, 1.0, 1.0));

    GLERR(glClearColor(1.0, 1.0, 1.0, 1.00));

    InitElements();
}

int main(void)
{
    GLFWwindow* window;

    /* Initialize the library */
    if (!glfwInit())
    {
        return -1;
    }

    glfwWindowHint(GLFW_VISIBLE, GL_FALSE);


    window = glfwCreateWindow(initialWindowWidth, initialWindowHeight, "Test libgui", NULL, NULL);
    if (!window)
    {
        printf("Could not create window.  Exiting...\n");
        glfwTerminate();
        return -1;
    }

    /* Make the window's context current */
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);

    glfwSetFramebufferSizeCallback(window, reshape);
    glfwSetWindowRefreshCallback(window, windowRefresh);
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
