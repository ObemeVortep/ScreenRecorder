#ifndef ICONVERTER_H
#define ICONVERTER_H

#include <vector>

class IConverter {
public:
	virtual ~IConverter() = default;
	/**
	 * Initialize the converter.
	 * returns 0 on success, negative value on failure.
	 */
	virtual int Initialize(int iNewWidth, int iNewHeight) = 0;
	/**
	 * Convert raw frame to encoded format.
	 * returns a vector containing the encoded data.
	 */
	virtual std::vector<unsigned char> Convert(const std::vector<unsigned char>& vRawFrame) = 0;
};

#endif // ICONVERTER_H