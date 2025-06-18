#include <iostream>

#include "lang_test/linkage/static_members/src1.h"

namespace two {

std::string& getStaticMember() {
    return ClassTemplateWithStaticDataMember<int>::sMem;
}

std::string_view const& getStaticMemberStringView() {
    std::cout << &ClassTemplateWithStaticDataMember<int>::sStringView
              << std::endl;
    std::cout << (void*)SomeFunction << std::endl;
    PRINTME(&ClassTemplateWithStaticDataMember<int>::non_addressable);
    return ClassTemplateWithStaticDataMember<int>::sStringView;
}

}  // namespace two
