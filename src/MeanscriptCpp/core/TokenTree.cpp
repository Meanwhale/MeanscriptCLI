
//#define FLOAT_TO_INT_FORMAT(f) (*reinterpret_cast<int*>(&(f)))
//#define INT_FORMAT_TO_FLOAT(i) (*reinterpret_cast<float*>(&(i)))
#include "MS.h"
namespace meanscriptcore {
using namespace meanscript;
TokenTree::TokenTree()
{ }

TokenTree::~TokenTree() { delete root; }
} // namespace meanscript(core)
// C++ END
