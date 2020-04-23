// ----------------------------------------------------------------------------
// -                        Open3D: www.open3d.org                            -
// ----------------------------------------------------------------------------
// The MIT License (MIT)
//
// Copyright (c) 2018 www.open3d.org
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
// IN THE SOFTWARE.
// ----------------------------------------------------------------------------

#pragma once

#include "Menu.h"

#include <functional>
#include <memory>
#include <string>

namespace open3d {
namespace gui {

struct Theme;
class Window;

class Application {
public:
    static Application &GetInstance();

    virtual ~Application();

    // Initializes the application, and in particular, finds the path for
    // the resources. If you can provide the argc/argv arguments it is more
    // reliable.
    void Initialize();
    void Initialize(int argc, const char *argv[]);

    /// Does not return until the UI is completely finished. Use this if you
    /// just need something simple.
    void Run();
    bool RunOneTick();  // internal use
    void Quit();        // closes all windows (and exits as a result)

    std::shared_ptr<Menu> GetMenubar() const;
    void SetMenubar(std::shared_ptr<Menu> menubar);

    /// Must be called on the same thread that calls Run()
    void AddWindow(std::shared_ptr<Window> window);
    /// Must be called on the same thread that calls Run()
    void RemoveWindow(Window *window);

    /// Creates a message box window the next time the event loop processes.
    /// This message box will be a separate window and not associated with any
    /// of the other windows shown with AddWindow().
    ///     THIS FUNCTION SHOULD BE USED ONLY AS A LAST RESORT!
    /// If you have a window, you should use Window::ShowMessageBox() so that
    /// the message box will be modal to that window. If you do not have a
    /// window it is better to use ShowNativeAlert(). If the platform does not
    /// have an alert (like Linux), then this can be used as a last resort.
    void ShowMessageBox(const char *title, const char *message);

    // (std::string not good in interfaces for ABI reasons)
    const char *GetResourcePath() const;

    /// This is primarily intended for use by the Window class. Any size-related
    /// fields (for example, fontSize) should be accessed through
    /// Window::GetTheme() as they be updated to reflect the pixel scaling
    /// on the monitor where the Window is displayed.
    const Theme &GetTheme() const;

    /// Delivers the itemId to the active window. Used internally.
    void OnMenuItemSelected(Menu::ItemId itemId);

    /// Cleanup everything right now. An example of usage is Cocoa's
    /// -applicationWillTermiate: AppDelegate message. Using Quit would result
    /// in a crash (and an unsightly message from macOS) due to destructing
    /// the windows at the wrong time.
    void OnTerminate();

private:
    Application();

    enum class RunStatus { CONTINUE, DONE };
    RunStatus ProcessQueuedEvents();

private:
    struct Impl;
    std::unique_ptr<Impl> impl_;
};

}  // namespace gui
}  // namespace open3d
