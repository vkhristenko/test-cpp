#include <iostream>

///
/// Динамические структуры данных
/// Разработать консольное приложение для работы с динамической структурой в соответствии с вариантом задания.
/// 22    Стек    нет    struct {char[], int}
///

///
/// Динамические структуры данных
/// Разработать консольное приложение для работы с динамической структурой в соответствии с вариантом задания.
/// 22    Стек    нет    struct {char[], int}
///

struct StackItem {
    char* array;
    int size;
};

struct Vector {
    Vector()
        : data_(nullptr), size_(0), capacity_(0)
    {
    }

   void push_back(StackItem const& item)
   {
       if (size_ < capacity_)
       {
            data_[size_] = item;
            size_++;
            return ;
       }

       capacity_ += 100;
       StackItem* memory = new StackItem[capacity_];
       for (int i=0; i<size_; i++) {
         	memory[i] = data_[i];
       }
       delete [] data_;
      data_ = memory;

      push_back(item);
   }

   void pop_back() {
       if (size_ > 0) {
          	size_ = size_ - 1;
       }
   }

   StackItem const& get(int idx) const {
    	return data_[idx];
    }
   StackItem& get(int idx) {
	return data_[idx];
    }

   int size() const {
       return size_;
   }

   StackItem* data_;
   int size_;
   int capacity_;
};

struct Stack {
    Stack()
    {
    }
	
    void push(StackItem const& item)
    {
        data_.push_back(item);
    }

    void pop()
    {
        data_.pop_back();
    }

    StackItem const& top() const 
    {
         return data_.get(data_.size() - 1);
    }

    bool empty() const {
         return data_.size() == 0;
    }

    Vector data_;
};

void TestStack() {
   Stack stack;

    assert(stack.empty());

    int n;
    std::cin >> n;

    for (std::size_t i=0; i<n; i++) {
       StackItem item;
       std::cin >> item.size;
       item.array = new char[item.size];
       for (int j=0; j<item.size; j++) {
           std::cin >> item.array[j];
       }
       stack.push(item);
   }
    assert(! stack.empty());

    while (! stack.empty()) {
        StackItem const& top = stack.top();
        std::cout << top.size << " ";
        for (int i=0; i<top.size; i++) {
            std::cout << top.array[i];
        }
        std::cout << std::endl;
        stack.pop();
    }

    assert(stack.empty());
}

int main() {
    TestStack();

    return 0;
}
