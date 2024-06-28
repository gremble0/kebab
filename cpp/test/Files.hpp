#ifndef KEBAB_FILES_HPP
#define KEBAB_FILES_HPP

#include <fstream>

namespace Kebab {

void ASSERT_FILES_EQ(std::ifstream &f1, std::ifstream &f2);
void replace_expected();

} // namespace Kebab

#endif
