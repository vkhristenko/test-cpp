#pragma once

#include "lang_test/linkage/static_members/common.h"

namespace two {

std::string& getStaticMember();

std::string_view const& getStaticMemberStringView();

}
