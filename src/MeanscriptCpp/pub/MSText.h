namespace meanscript {
class MSText
{
public:
// Text bytes in an integer array. first int is the number of bytes. Bytes go from right to left (to be convinient on C++).
// Specification:
//		{0x00000000}{0x00000000}				= empty text, ie. just the terminating '\0' character
//		{0x00000002}{0x00006261}				= 2 chars ("ab") and '\0', from right to left
//		{0x00000005}{0x64636261}{0x00000065}	= 5 chars ("abcde") and '\0'
// Number of ints after the first int 'i' is '(int)i / 4 + 1' if 'i > 0', and 0 otherwise.
// Can't be modified. TODO: C++ reference counter for smart memory handling.
Array<int> data;
MSText (const std::string & src);
MSText (const uint8_t src[], int32_t start, int32_t length);
void _init (const uint8_t src[], int32_t start, int32_t length);
MSText (const MSText & src);
MSText (Array<int> & src);
MSText (Array<int> & src, int32_t start);
bool match (const MSText & t);
bool match (const char * s);
Array<int> & getData ();
int32_t numBytes () const;
int32_t dataSize () const;
int32_t byteAt (int32_t index) const;
int32_t write (Array<int> & trg, int start) const ;
void makeCopy ( const Array<int> & src, int start);
int32_t compare (const MSText & text) const;
void check ();
std::string getString ();
bool operator <(const MSText& t) const
{
 return compare(t) < 0;
}
};
} // namespace meanscript(core)
