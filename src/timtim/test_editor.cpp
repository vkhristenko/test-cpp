#include <algorithm>
#include <iostream>
#include <list>
#include <string>

class Editor {
public:
    Editor() { cursor_ = text_.begin(); }

    void Left() {
        if (cursor_ != text_.begin()) --cursor_;
    }

    void Right() {
        if (cursor_ != text_.end()) ++cursor_;
    }

    void Insert(char token) {
        cursor_ = text_.insert(cursor_, token);
        ++cursor_;
    }

    void Cut(size_t tokens = 1) {
        clipboard_.clear();
        auto end = std::next(cursor_,
                             std::min(tokens, static_cast<size_t>(std::distance(
                                                  cursor_, text_.end()))));
        std::copy(cursor_, end, std::back_inserter(clipboard_));
        cursor_--;
        text_.erase(std::next(cursor_), end);
        cursor_ = std::next(cursor_);
    }

    void Copy(size_t tokens = 1) {
        clipboard_.clear();
        auto end = std::next(cursor_, tokens);
        std::copy(cursor_, end, std::back_inserter(clipboard_));
    }

    void Paste() {
        if (!clipboard_.empty()) {
            cursor_ =
                text_.insert(cursor_, clipboard_.begin(), clipboard_.end());

            for (size_t i = 0; i < clipboard_.size(); i++) {
                ++cursor_;
            }
        }
    }

    std::string GetText() const {
        return std::string(text_.begin(), text_.end());
    }

private:
    std::list<char> text_;
    std::list<char> clipboard_;
    std::list<char>::iterator cursor_;
};

void Test0() {
    auto editor = Editor{};

    for (auto c : {'a', 'b', 'c', 'd'}) {
        editor.Insert(c);
    }
    std::cout << editor.GetText() << std::endl;

    for (int i = 0; i < 3; i++) {
        editor.Left();
    }

    editor.Cut(2);
    std::cout << editor.GetText() << std::endl;

    editor.Insert('w');
    std::cout << editor.GetText() << std::endl;
}

void Test1() {
    Editor editor;
    const std::string text = "hello, world";
    for (char c : text) {
        editor.Insert(c);
    }
    std::cout << editor.GetText() << std::endl;
    // Текущее состояние редактора: ⁠ hello,
    // world| ⁠
    for (size_t i = 0; i < text.size(); ++i) {
        editor.Left();
    }
    std::cout << editor.GetText() << std::endl;
    // Текущее состояние редактора: ⁠ |hello,
    // world ⁠
    editor.Cut(7);
    std::cout << editor.GetText() << std::endl;
    // Текущее состояние редактора: ⁠ |world ⁠
    // в буфере обмена находится текст `hello, `
    for (size_t i = 0; i < 5; ++i) {
        editor.Right();
    }
    std::cout << editor.GetText() << std::endl;
    // Текущее состояние редактора: ⁠ world| ⁠
    editor.Insert(',');
    std::cout << editor.GetText() << std::endl;
    editor.Insert(' ');
    std::cout << editor.GetText() << std::endl;
    // Текущее состояние редактора: ⁠ world,
    // | ⁠
    editor.Paste();
    std::cout << editor.GetText() << std::endl;
    // Текущее состояние редактора: ⁠ world, hello,
    // | ⁠
    editor.Left();
    std::cout << editor.GetText() << std::endl;
    editor.Left();
    std::cout << editor.GetText() << std::endl;
    // Текущее состояние редактора: `world, hello|, `
    editor.Cut(3);  // Будут вырезаны 2 символа
    std::cout << editor.GetText() << std::endl;
    // Текущее состояние редактора: ⁠ world,
    // hello| ⁠
    std::cout << editor.GetText();
}

int main() {
    Test0();
    Test1();
    return 0;
}
