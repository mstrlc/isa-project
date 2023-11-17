#include "ldap.h"

#include <string>
#include <vector>

#include "ber.h"

// LDAPMessage ::= SEQUENCE {
//      messageID       MessageID,
//      protocolOp      CHOICE {
//           ... },
//      controls       [0] Controls OPTIONAL }

// MessageID ::= INTEGER (0 ..  maxInt)

// maxInt INTEGER ::= 2147483647 -- (2^^31 - 1) --
std::vector<unsigned char> build_ldapmessage(int message_id, unsigned char type) {
    // MessageID
    std::vector<unsigned char> message_id_bytes = create_integer(message_id);

    // ProtocolOp
    std::vector<unsigned char> protocol_op_bytes;
    switch (type) {
        case BIND_RESPONSE:
            protocol_op_bytes = build_bindresponse(RESULT_SUCCESS);
            break;

        case SEARCH_RESULT_ENTRY:
            protocol_op_bytes = build_searchresentry();
            break;

        case SEARCH_RESULT_DONE:
            protocol_op_bytes = build_searchresdone();
            break;

        default:
            break;
    }

    // Controls
    // TODO implement controls

    // Put together the LDAPMessage
    std::vector<unsigned char> result;

    result.push_back(BER_SEQUENCE);                                        // LDAPMessage
    result.push_back(message_id_bytes.size() + protocol_op_bytes.size());  // Length

    result.insert(result.end(), message_id_bytes.begin(), message_id_bytes.end());
    result.insert(result.end(), protocol_op_bytes.begin(), protocol_op_bytes.end());

    return result;
}

// BindResponse ::= [APPLICATION 1] SEQUENCE {
//      COMPONENTS OF LDAPResult,
//      serverSaslCreds    [7] OCTET STRING OPTIONAL }
//
// LDAPResult ::= SEQUENCE {
//      resultCode         ENUMERATED {
//           success                      (0),
//           operationsError              (1),
//           protocolError                (2),
//           ...
//           other                        (80),
//           ...  },
//      matchedDN          LDAPDN,
//      diagnosticMessage  LDAPString,
//      referral           [3] Referral OPTIONAL }
std::vector<unsigned char> build_bindresponse(unsigned char result_code) {
    // ResultCode
    std::vector<unsigned char> result_code_bytes = create_enumerated(result_code);

    // MatchedDN
    std::vector<unsigned char> matched_dn_bytes = create_octet_string("");

    // DiagnosticMessage
    std::vector<unsigned char> diagnostic_message_bytes = create_octet_string("");

    // Referral
    // TODO implement referral

    // ServerSaslCreds
    // TODO implement serverSaslCreds

    // Put together the BindResponse
    std::vector<unsigned char> bindresponse_bytes = create_sequence({result_code_bytes, matched_dn_bytes, diagnostic_message_bytes}, BIND_RESPONSE);
    return bindresponse_bytes;
}

std::vector<unsigned char> build_searchresentry() {
}

std::vector<unsigned char> build_searchresdone() {
}
