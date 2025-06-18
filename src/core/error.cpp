#include "core/error.h"

namespace core {

char IErrorDetails::s_id = 0;
char StringErrorDetails::s_id = 0;

Error::Error(Error const& e)
    : source_context_{e.source_context_}, details_{e.details_->Clone()} {}

Error& Error::operator=(Error const& e) {
    this->source_context_ = e.source_context_;
    this->details_ = e.details_->Clone();
    return *this;
}

}  // namespace core
