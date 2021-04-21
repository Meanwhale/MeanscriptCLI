// Auto-generated: do not edit.
namespace meanscriptcore {
using namespace meanscript;
class ClassMaker
{
public:
 Semantics* semantics;
 MSCode* config;
 Array<int> offsetStack;
 Array<int> arrayItemSizeStack;
 Array<int> arrayItemCountStack;
 Array<std::string> memberStack;
 Array<std::string> arrayStack;
ClassMaker();
static void findTypes (Semantics & semantics, Array<int> & code);
static void createStructDefs (Semantics & semantics, Array<int> & code);
void pushName (int32_t offset, std::string memberName, std::string arrayName, int32_t arrayItemSize, int32_t arrayItemCount, int32_t depth);
void makeJavaAcc (MSOutputPrint & os, std::string setterData, int32_t depth, int32_t accessType);
void makeIndexedAddress (MSOutputPrint & os, int32_t baseAddress, int32_t depth);
void makeIndexCheck (MSOutputPrint & os, int32_t depth);
void makeJavaMembers (MSOutputPrint & os, StructDef* sd, int32_t addressOffset, int32_t depth);
void makeJava (Array<int> & code, std::string packageName, std::string folderName);
~ClassMaker();
};
} // namespace meanscript(core)
// C++ header END
