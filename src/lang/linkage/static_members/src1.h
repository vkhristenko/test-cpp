#pragma once

#include "lang_test/linkage/static_members/common.h"

namespace one {

std::string& getStaticMember();

std::string_view const& getStaticMemberStringView();

}
