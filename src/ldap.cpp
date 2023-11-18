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
ber_bytes build_ldapmessage(int message_id, ber_bytes protocolop_bytes) {
    // MessageID
    ber_bytes message_id_bytes = create_integer(message_id);

    // ProtocolOp
    ber_bytes protocol_op_bytes = protocolop_bytes;

    // Controls
    // TODO implement controls

    // Put together the LDAPMessage
    ber_bytes ldapmessage_bytes;

    ldapmessage_bytes.push_back(BER_SEQUENCE);  // LDAPMessage
    ber_bytes length_bytes = create_length(message_id_bytes.size() + protocol_op_bytes.size());
    for (unsigned char c : length_bytes) {
        ldapmessage_bytes.push_back(c);
    }
    ldapmessage_bytes.insert(ldapmessage_bytes.end(), message_id_bytes.begin(), message_id_bytes.end());
    ldapmessage_bytes.insert(ldapmessage_bytes.end(), protocol_op_bytes.begin(), protocol_op_bytes.end());

    return ldapmessage_bytes;
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
ber_bytes build_bindresponse(int messageid, unsigned char result_code) {
    // ResultCode
    ber_bytes result_code_bytes = create_enumerated(result_code);

    // MatchedDN
    ber_bytes matched_dn_bytes = create_octet_string("");  // TODO implement matchedDN

    // DiagnosticMessage
    ber_bytes diagnostic_message_bytes = create_octet_string("");  // TODO implement diagnosticMessage

    // Referral
    // TODO implement referral

    // ServerSaslCreds
    // TODO implement serverSaslCreds

    // Put together the BindResponse
    ber_bytes bindresponse_bytes = create_sequence({result_code_bytes, matched_dn_bytes, diagnostic_message_bytes}, BIND_RESPONSE);

    ber_bytes ldapmessage_bytes = build_ldapmessage(messageid, bindresponse_bytes);

    return ldapmessage_bytes;
}

// PartialAttributeList ::= SEQUENCE OF
//                      partialAttribute PartialAttribute
//
// PartialAttribute ::= SEQUENCE {
//      type       AttributeDescription,
//      vals       SET OF value AttributeValue }
ber_bytes build_partialattributelist(std::vector<std::tuple<std::string, std::string>> attributes) {
    std::vector<ber_bytes> partial_attributes_list;

    for (std::tuple<std::string, std::string> attribute : attributes) {
        // Type
        ber_bytes type_bytes = create_octet_string(std::get<0>(attribute));

        // Vals
        ber_bytes vals_bytes = create_set({create_octet_string(std::get<1>(attribute))});

        // Put together the PartialAttribute
        ber_bytes partial_attribute_bytes = create_sequence({type_bytes, vals_bytes});

        partial_attributes_list.push_back(partial_attribute_bytes);
    }

    // Put together the PartialAttributeList
    ber_bytes partial_attribute_list_bytes = create_sequence(partial_attributes_list);
    return partial_attribute_list_bytes;
}

// SearchResultEntry ::= [APPLICATION 4] SEQUENCE {
//      objectName      LDAPDN,
//      attributes      PartialAttributeList }

// PartialAttributeList ::= SEQUENCE OF
//                      partialAttribute PartialAttribute
//
// PartialAttribute ::= SEQUENCE {
//      type       AttributeDescription,
//      vals       SET OF value AttributeValue }
//
// AttributeDescription ::= LDAPString
//                         -- Constrained to <attributedescription>
//
// AttributeValue ::= OCTET STRING
ber_bytes build_searchresentry(int messageid, std::string uid, std::string cn, std::string mail) {
    // objectName
    std::string objectname = "uid=" + uid + ",ou=fit,dc=vutbr,dc=cz";
    ber_bytes objectname_bytes = create_octet_string(objectname);

    // attributes
    ber_bytes attributes_bytes = build_partialattributelist({std::make_tuple("uid", uid), std::make_tuple("cn", cn), std::make_tuple("mail", mail)});

    // Put together the SearchResultEntry
    ber_bytes searchresultentry_bytes = create_sequence({objectname_bytes, attributes_bytes}, SEARCH_RESULT_ENTRY);

    ber_bytes ldapmessage_bytes = build_ldapmessage(messageid, searchresultentry_bytes);

    return ldapmessage_bytes;
}

// SearchResultDone ::= [APPLICATION 5] LDAPResult
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
ber_bytes build_searchresdone(int messageid, unsigned char result_code) {
    // ResultCode
    ber_bytes result_code_bytes = create_enumerated(result_code);

    // MatchedDN
    ber_bytes matched_dn_bytes = create_octet_string("");  // TODO implement matchedDN

    // DiagnosticMessage
    ber_bytes diagnostic_message_bytes = create_octet_string("");  // TODO implement diagnosticMessage

    // Referral
    // TODO implement referral

    // Put together the SearchResultDone
    ber_bytes searchresultdone_bytes = create_sequence({result_code_bytes, matched_dn_bytes, diagnostic_message_bytes}, SEARCH_RESULT_DONE);

    ber_bytes ldapmessage_bytes = build_ldapmessage(messageid, searchresultdone_bytes);

    return ldapmessage_bytes;
}
