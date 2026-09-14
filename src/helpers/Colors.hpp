// ============================================================================
// License: MIT License (Permissive - free to use in any project)
//
// Copyright (c) 2026 hi42344
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.
// ============================================================================

namespace color {
    // Reset
    constexpr const char* reset = "\033[0m";

    // Modifiers
    constexpr const char* bold = "\033[1m";
    constexpr const char* dim = "\033[2m";
    constexpr const char* italic = "\033[3m";
    constexpr const char* underline = "\033[4m";

    // Truecolor RGB Foreground (24-bit)
    constexpr const char* orange = "\033[38;2;255;165;0m";
    constexpr const char* steam_blue = "\033[38;2;102;192;244m";
    constexpr const char* steam_dark = "\033[38;2;23;26;33m";
    constexpr const char* purple = "\033[38;2;175;95;255m";
    constexpr const char* pink = "\033[38;2;255;105;180m";
    constexpr const char* cyan = "\033[38;2;0;225;255m";
    constexpr const char* lime = "\033[38;2;50;205;50m";

    // Status / UI RGB Colors
    constexpr const char* success = "\033[38;2;76;175;80m";   // Soft Green
    constexpr const char* warning = "\033[38;2;255;152;0m";  // Amber / Orange
    constexpr const char* error = "\033[38;2;244;67;54m";   // Bright Red
    constexpr const char* info = "\033[38;2;33;150;243m";   // Soft Blue
    constexpr const char* muted = "\033[38;2;120;120;120m"; // Muted Gray

    // Standard High-Intensity 16-Color ANSI
    constexpr const char* bright_red = "\033[91m";
    constexpr const char* bright_green = "\033[92m";
    constexpr const char* bright_yellow = "\033[93m";
    constexpr const char* bright_blue = "\033[94m";
    constexpr const char* bright_purple = "\033[95m";
    constexpr const char* bright_cyan = "\033[96m";
    constexpr const char* bright_white = "\033[97m";

    // Backgrounds (Truecolor RGB)
    constexpr const char* bg_dark_gray = "\033[48;2;30;30;30m";
    constexpr const char* bg_red = "\033[48;2;180;40;40m";
    constexpr const char* bg_green = "\033[48;2;40;140;40m";
}