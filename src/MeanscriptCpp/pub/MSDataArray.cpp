
//#define FLOAT_TO_INT_FORMAT(f) (*reinterpret_cast<int*>(&(f)))
//#define INT_FORMAT_TO_FLOAT(i) (*reinterpret_cast<float*>(&(i)))
#include "MS.h"
namespace meanscript {
using namespace meanscriptcore;

MSDataArray::MSDataArray (MeanMachine* _mm, int32_t _tagAddress, int32_t _dataIndex) 
{
	mm = _mm;
	
	structCode = (*mm).getStructCode();
	dataCode = (*mm).getDataCode();

	tagAddress = _tagAddress;
	dataIndex = _dataIndex;
	tag = (*structCode)[tagAddress];
	
	ASSERT((tag & OPERATION_MASK) == OP_ARRAY_MEMBER, "not an array");
}
int32_t MSDataArray::getItemType ()
{
	return (int32_t)(tag & VALUE_TYPE_MASK);
}

int32_t MSDataArray::getArrayItemCount () 
{
	return (*structCode)[tagAddress + 3];
}

// TODO: getIntAt(i), getTextAt(i), etc.

MSData MSDataArray::getAt (int32_t i) 
{
	int32_t itemCount = getArrayItemCount();
	CHECK(i >=0 && i < itemCount, EC_DATA, "index out of bounds");
	int32_t arrayDataSize = (*structCode)[tagAddress + 2];
	int32_t arrayItemSize = arrayDataSize / itemCount;
	int32_t itemAddress = dataIndex + (i * arrayItemSize);
	return MSData (mm,tagAddress, itemAddress, true);
}

MSDataArray::~MSDataArray() { };

} // namespace meanscript(core)
// C++ END
