namespace meanscriptcore {
using namespace meanscript;
class Parser
{
public:
static TokenTree* Parse(meanscript::MSInputStream & input);
static bool isValidName(const char * s);
static bool isValidName(MSText* t);
};
} // namespace meanscript(core)
