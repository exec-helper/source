#include "cast-impl.h"

using std::string;
using std::vector;

namespace execHelper::config::detail {
template class Cast<string, vector<string>>;
template class Cast<vector<string>, vector<string>>;
template class Cast<bool, vector<string>>;
template class Cast<Path, vector<string>>;
template class Cast<char, vector<string>>;
template class Cast<uint32_t, vector<string>>;
} // namespace execHelper::config::detail
