#ifndef LDAP_H
#define LDAP_H

#include <vector>
// BER tags
#define BER_INTEGER 0x02
#define BER_OCTET_STRING 0x04
#define BER_SEQUENCE 0x30
#define BER_SET 0x31
#define BER_ENUMERATED 0x0A

// LDAP message tags
#define BIND_REQUEST 0x60
#define BIND_RESPONSE 0x61
#define SEARCH_REQUEST 0x63
#define SEARCH_RESULT_ENTRY 0x64
#define SEARCH_RESULT_DONE 0x65
#define UNBIND_REQUEST 0x42

// LDAP result code enum
#define RESULT_SUCCESS 0x00
#define RESULT_OPERATIONS_ERROR 0x01
#define RESULT_PROTOCOL_ERROR 0x02

std::vector<unsigned char> build_ldapmessage(int message_id, unsigned char type);

std::vector<unsigned char> build_bindresponse(unsigned char result_code);

std::vector<unsigned char> build_searchresentry();

std::vector<unsigned char> build_searchresdone();

#endif  // LDAP_H