namespace meanscriptcore {
using namespace meanscript;
class TokenTree
{
public:
 MNode* root = 0;
 std::map<MSText, int> texts;
TokenTree();
~TokenTree();
};
} // namespace meanscript(core)
