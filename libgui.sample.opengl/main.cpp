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
#include "libgui/Button.h"

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
using libgui::Button;

typedef struct
{
    float x, y, z;
    float r, g, b, a;
} vertex_t;

typedef struct
{
    int r, g, b;
} color_t;

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

void FillRectangle(double left, double top, double right, double bottom, int r, int g, int b, float a = 1.0);
void OutlineRectangle(double left, double top, double right, double bottom, int r, int g, int b, double lineWidth);
void DrawText(double centerX, double centerY, std::string text);

void DrawButton(Element* e);

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
    auto root = elementManager->AddLayer("Main Layer");
    root->SetViewModel(itemsVm);
    root->SetArrangeCallback(
        [](shared_ptr<Element> e)
        {
            e->SetLeft(0);
            e->SetRight(windowWidth);
            e->SetTop(0);
            e->SetBottom(windowHeight);
        });
    root->SetDrawCallback(
        [](Element* e, const boost::optional<Rect4>& redrawRegion)
        {
            Rect4 region;
            if (redrawRegion)
            {
                region = redrawRegion.get();
            }
            else
            {
                region = Rect4(e->GetLeft(), e->GetTop(), e->GetRight(), e->GetBottom());
            }
            FillRectangle(region.left, region.top, region.right, region.bottom, 0xFF, 0xFF, 0xFF);
        });


    // Build the screen elements
    auto header = make_shared<Element>("Header");
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

        auto launchButton = make_shared<Button>();
        {
            header->AddChild(launchButton);
            launchButton->SetArrangeCallback(
                [](shared_ptr<Element> e)
                {
                    auto p = e->GetParent();
                    e->SetCenterX(p->GetCenterX());
                    e->SetCenterY(p->GetCenterY());
                    e->SetWidth(150);
                    e->SetHeight(25);
                });
            launchButton->SetDrawCallback(
                [](Element* e, const boost::optional<Rect4>& redrawRegion)
                {
                    DrawButton(e);
                    DrawText(e->GetCenterX(), e->GetCenterY(), "Launch Message");
                });

            launchButton->SetEventCallback(
                [](shared_ptr<Button> b, Button::OutputEvent event)
                {
                    if (Button::OutputEvent::Clicked == event)
                    {
                        // launch the modal
                        auto layer = b->GetElementManager()->AddLayer("Popup Layer");
                        {
                            layer->SetArrangeCallback(
                                [layer](shared_ptr<Element> e)
                                {
                                    e->SetCenterX(windowWidth / 2);
                                    e->SetCenterY(windowHeight / 2);
                                    e->SetWidth(300);
                                    e->SetHeight(200);

                                    // Optimize drawing updates
                                    layer->SetOpaqueArea(e->GetBounds());

                                    // Notify libgui of drawing that exceeds bounds
                                    e->SetVisualBounds(Rect4(0, 0, windowWidth, windowHeight));
                                });
                            layer->SetDrawCallback(
                                [](Element* e, const boost::optional<Rect4>& redrawRegion)
                                {
                                    bool drawOpaqueArea;

                                    // Cover the other elements with transparent layer
                                    Rect4 region;
                                    if (redrawRegion)
                                    {
                                        region         = redrawRegion.get();
                                        drawOpaqueArea = e->Intersects(redrawRegion.get());
                                    }
                                    else
                                    {
                                        region         = e->GetVisualBounds().get();
                                        drawOpaqueArea = true;
                                    }
                                    FillRectangle(region.left, region.top, region.right, region.bottom,
                                                  0xFF, 0xFF, 0xFF, 0.7);

                                    if (drawOpaqueArea)
                                    {
                                        FillRectangle(e->GetLeft(), e->GetTop(), e->GetRight(), e->GetBottom(),
                                                      0xFF, 0xFF, 0xFF);
                                        OutlineRectangle(e->GetLeft(), e->GetTop(), e->GetRight(), e->GetBottom(),
                                                         0xCB, 0xCB, 0xCB, 1.0);
                                    }
                                });

                            auto okButton = make_shared<Button>();
                            {
                                layer->AddChild(okButton);
                                okButton->SetArrangeCallback(
                                    [](shared_ptr<Element> e)
                                    {
                                        auto p = e->GetParent();
                                        e->SetHeight(41);
                                        e->SetLeft(p->GetLeft());
                                        e->SetRight(p->GetRight());
                                        e->SetBottom(p->GetBottom());
                                    });
                                okButton->SetDrawCallback(
                                    [](Element* e, const boost::optional<Rect4>& redrawRegion)
                                    {
                                        DrawButton(e);
                                        DrawText(e->GetCenterX(), e->GetCenterY(), "Ok");
                                    });
                                okButton->SetEventCallback(
                                    [b](shared_ptr<Button> b2, Button::OutputEvent event2)
                                    {
                                        if (Button::OutputEvent::Clicked == event2)
                                        {
                                            auto l = b2->GetLayer();
                                            b2->GetElementManager()->RemoveLayer(l);

                                            // Re-enable the main layer
                                            b->GetLayer()->SetIsEnabled(true);
                                        }
                                    });
                            }

                            auto messageLabel = make_shared<Element>("Message");
                            {
                                layer->AddChild(messageLabel);
                                messageLabel->SetArrangeCallback(
                                    [](shared_ptr<Element> e)
                                    {
                                        auto ps = e->GetPrevSibling();
                                        auto p  = e->GetParent();
                                        e->SetLeft(p->GetLeft());
                                        e->SetTop(p->GetTop());
                                        e->SetRight(p->GetRight());
                                        e->SetBottom(ps->GetTop());
                                    });
                                messageLabel->SetDrawCallback(
                                    [](Element* e, const boost::optional<Rect4>&)
                                    {
                                        DrawText(e->GetCenterX(), e->GetCenterY(), "This is a message");
                                    });
                            }
                        }

                        // Disable this layer and draw the new layer
                        b->GetLayer()->SetIsEnabled(false);
                        layer->ArrangeAndDraw();
                    }
                });
        }

    }

    auto footer = make_shared<Element>("Footer");
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
            [](shared_ptr<Element> cellContainer)
            {
                auto cell_background = make_shared<Element>("Cell Background");
                {
                    cellContainer->AddChild(cell_background);
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

                    auto text = make_shared<Element>("Cell Text");
                    {
                        cell_background->AddChild(text);
                        text->SetDrawCallback(
                            [](Element* e, const boost::optional<Rect4>& redrawRegion)
                            {
                                auto ivm = dynamic_pointer_cast<ItemViewModel>(e->GetViewModel());
                                DrawText(e->GetCenterX(), e->GetCenterY(), ivm->GetName());

                                #ifdef DBG
                                printf("%s\n", ivm->GetName().c_str()); fflush(stdout);
                                #endif
                            });
                    }
                }
            });
    }

    auto grid_scroll = make_shared<Scrollbar>(grid);
    {
        root->AddChild(grid_scroll);
        grid_scroll->SetArrangeCallback(
            [grid, gridScrollWidth, header, footer](shared_ptr<Element> e)
            {
                auto p          = e->GetParent();
                auto can_scroll = dynamic_pointer_cast<Grid>(grid)->CanScroll();

                if (can_scroll)
                {
                    e->SetIsVisible(true);
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

        grid_scroll->InitializeThis();
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

    shared_ptr<Slider::Thumb> slider_thumb;

    auto slider = make_shared<Slider>();
    {
        root->AddChild(slider);
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

        slider->InitializeThis();
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

        slider_thumb = slider->GetThumb();
        slider_thumb->SetDrawCallback(
            [](Element* e, const boost::optional<Rect4>& redrawRegion)
            {
                FillRectangle(e->GetLeft(), e->GetTop(), e->GetRight(), e->GetBottom(), 0x77, 0x77, 0x77);
            });
    }

    auto sliderValueText = make_shared<Element>("Slider Value Text");
    {
        footer->AddChild(sliderValueText);
        sliderValueText->SetDrawCallback(
            [slider](Element* e, const boost::optional<Rect4>& redrawRegion)
            {
                std::string valueString = std::to_string(slider->GetValue());
                DrawText(e->GetCenterX(), e->GetCenterY(), valueString);
            });
        slider_thumb->AddArrangeDependent(sliderValueText);
    }

    root->InitializeAll();
}

static void cursor_position_callback(GLFWwindow* window, double xpos, double ypos)
{
    elementManager->NotifyNewPoint(PointerInputId, Point{xpos, ypos});
}

static void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
    if (button == GLFW_MOUSE_BUTTON_LEFT)
    {
        if (action == GLFW_PRESS)
        {
            elementManager->NotifyDown(PointerInputId);
        }
        else if (action == GLFW_RELEASE)
        {
            elementManager->NotifyUp(PointerInputId);
        }
    }
}

void FillRectangle(double left, double top, double right, double bottom, int r, int g, int b, float a)
{
    auto rf = float(r) / 255.0;
    auto gf = float(g) / 255.0;
    auto bf = float(b) / 255.0;

    vertex_buffer_t* buffer;
    buffer = vertex_buffer_new("vertex:3f,color:4f");
    vertex_t vertices[] = {{left,  top,    0, rf, gf, bf, a},  // 0
                           {right, top,    0, rf, gf, bf, a},  // 1
                           {right, bottom, 0, rf, gf, bf, a},  // 2
                           {left,  bottom, 0, rf, gf, bf, a}}; // 3

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

    mat4_set_identity(&model);
}

void DrawButton(Element* e)
{
    color_t outlineColor;
    color_t fillColor;

    auto b = dynamic_cast<Button*>(e);
    switch (b->GetVisibleState())
    {
        case Button::Idle:
            outlineColor = color_t{0xCB, 0xCB, 0xCB};
            fillColor    = color_t{0xEB, 0xEB, 0xEB};
            break;
        case Button::Pending:
            outlineColor = color_t{0x00, 0x00, 0x00};
            fillColor    = color_t{0xEB, 0xEB, 0xEB};
            break;
        case Button::Engaged:
            outlineColor = color_t{0x00, 0x00, 0x00};
            fillColor    = color_t{0xCB, 0xCB, 0xCB};
            break;
        case Button::EngagedRemotely:
            outlineColor = color_t{0x00, 0x00, 0x00};
            fillColor    = color_t{0xEB, 0xEB, 0xEB};
            break;
    }

    FillRectangle(e->GetLeft(), e->GetTop(), e->GetRight(), e->GetBottom(),
                  fillColor.r, fillColor.g, fillColor.b);
    OutlineRectangle(e->GetLeft(), e->GetTop(), e->GetRight(), e->GetBottom(),
                     outlineColor.r, outlineColor.g, outlineColor.b, 1.0);
}

void display(GLFWwindow* window)
{
    auto& redrawnRegionOpt = elementManager->GetRedrawnRegion();
    if (redrawnRegionOpt)
    {
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
        RefreshEntireWindow(window, width, height);
    }
}

void windowRefresh(GLFWwindow* window)
{
    if (firstWindowRefresh)
    {
        RefreshEntireWindow(window, initialWindowWidth, initialWindowHeight);

        firstWindowRefresh = false;
    }
}

void init()
{
    text_buffer   = text_buffer_new(LCD_FILTERING_OFF,
                                    "shaders/text.vert",
                                    "shaders/text.frag");
    vec4 black    = {{0.0, 0.0, 0.0, 1.0}};
    vec4 darkGray = {{float(0x44) / 255.f, float(0x44) / 255.f, float(0x44) / 255.f, 1.0}};
    text_buffer->base_color = black;

    vec4 none = {{1.0, 1.0, 1.0, 0.0}};
    markup.family              = (char*) "fonts/Vera.ttf";
    markup.size                = 12.0;
    markup.bold                = 0;
    markup.italic              = 0;
    markup.rise                = 0.0;
    markup.spacing             = 0.0;
    markup.gamma               = 1.0;
    markup.foreground_color    = darkGray;
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
