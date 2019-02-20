//
// Created by Sidorenko Nikita on 11/23/18.
//

#ifndef CPPWRAPPER_MULTIVBOADDRESS_H
#define CPPWRAPPER_MULTIVBOADDRESS_H

struct MultiBufferAddress {
	unsigned int index = 0; // index of the buffer
	unsigned int offset = 0; // offset within buffer
};

#endif //CPPWRAPPER_MULTIVBOADDRESS_H
