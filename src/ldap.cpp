/*
 * ldap.cpp
 * 2023-11-20
 *
 * ISA project 2023
 * LDAP server
 *
 * Matyas Strelec xstrel03
 */

#include "ldap.h"

#include <string>
#include <vector>

#include "filter.h"

// Classes
class LDAPMessage {
   public:
    LDAPMessage() {}

    unsigned char ldapmessage_tag;
    int message_id;

    unsigned char get_ldapmessagetag() {
        return this->ldapmessage_tag;
    }

    int get_message_id() {
        return this->message_id;
    }

    void set_message_id(int message_id) {
        this->message_id = message_id;
    }
};

class Request : public LDAPMessage {
   public:
    Request(ber_bytes bytes) {
        this->reader = BERreader(bytes);
    }

    BERreader reader;

   protected:
    ber_bytes bytes;
};

class Response : public LDAPMessage {
   public:
    Response() {}

    BERwriter writer = BERwriter();

    ber_bytes get_bytes() {
        return this->bytes;
    }

    unsigned char* get_raw() {
        return this->bytes.data();
    }

    size_t get_size() {
        return this->bytes.size();
    }

   protected:
    ber_bytes bytes = ber_bytes();
};

class BindRequest : public Request {
    unsigned char tag;
    int version;
    std::string name;
    unsigned char authentication;

   public:
    BindRequest() : Request(ber_bytes()) {}

    BindRequest(ber_bytes bytes) : Request(bytes) {
        this->ldapmessage_tag = reader.read_tag();
        this->message_id = this->reader.read_integer();
        this->parse();
    }

    int get_version() {
        return this->version;
    }

    std::string get_name() {
        return this->name;
    }

    unsigned char get_authentication() {
        return this->authentication;
    }

   private:
    void parse() {
        this->tag = this->reader.read_tag();
        this->version = this->reader.read_integer();
        this->name = this->reader.read_octet_string();
        this->authentication = this->reader.read_tag();
    }
};

class BindResponse : public Response {
    unsigned char tag = BIND_RESPONSE;
    unsigned char result_code;
    std::string matched_dn = "";
    std::string error_message = "";

   public:
    BindResponse(int result_code) : Response() {
        this->result_code = result_code;
    }

    unsigned char get_result_code() {
        return this->result_code;
    }

    void set_result_code(unsigned char result_code) {
        this->result_code = result_code;
    }

    std::string get_matched_dn() {
        return this->matched_dn;
    }

    void set_matched_dn(std::string matched_dn) {
        this->matched_dn = matched_dn;
    }

    std::string get_error_message() {
        return this->error_message;
    }

    void set_error_message(std::string error_message) {
        this->error_message = error_message;
    }

    void build() {
        // ResultCode
        ber_bytes result_code_bytes = this->writer.create_enumerated(this->result_code);

        // MatchedDN
        ber_bytes matched_dn_bytes = this->writer.create_octet_string("");  // TODO implement matchedDN

        // DiagnosticMessage
        ber_bytes diagnostic_message_bytes = this->writer.create_octet_string("");  // TODO implement diagnosticMessage

        // Referral

        // ServerSaslCreds

        // Put together the BindResponse
        ber_bytes bindresponse_bytes = this->writer.create_sequence({result_code_bytes, matched_dn_bytes, diagnostic_message_bytes}, this->tag);

        ber_bytes ldapmessage_bytes = this->writer.build_ldapmessage(this->get_message_id(), bindresponse_bytes);

        this->bytes = ldapmessage_bytes;
    }
};

class SearchRequest : public Request {
    unsigned char tag;
    std::string base_object;
    unsigned char scope;
    unsigned char deref_aliases;
    int size_limit;
    int time_limit;
    bool types_only;
    filter filters;
    // std::string attributes;

   public:
    SearchRequest() : Request(ber_bytes()) {}

    SearchRequest(ber_bytes bytes) : Request(bytes) {
        this->ldapmessage_tag = reader.read_tag();
        this->message_id = this->reader.read_integer();
        this->parse();
    }

    std::string get_base_object() {
        return this->base_object;
    }

    unsigned char get_scope() {
        return this->scope;
    }

    unsigned char get_deref_aliases() {
        return this->deref_aliases;
    }

    int get_size_limit() {
        return this->size_limit;
    }

    int get_time_limit() {
        return this->time_limit;
    }

    bool get_types_only() {
        return this->types_only;
    }

    struct filter get_filter() {
        return this->filters;
    }

   private:
    void parse() {
        this->tag = this->reader.read_tag();
        this->base_object = this->reader.read_octet_string();
        this->scope = this->reader.read_enumerated();
        this->deref_aliases = this->reader.read_enumerated();
        this->size_limit = this->reader.read_integer();
        this->time_limit = this->reader.read_integer();
        this->types_only = this->reader.read_boolean();
        this->filters = this->reader.read_filters();
    }
};

class SearchResEntry : public Response {
    unsigned char tag = SEARCH_RESULT_ENTRY;
    std::string object_name;

    std::string uid = "";
    std::string cn = "";
    std::string mail = "";

   public:
    SearchResEntry(std::string uid, std::string cn, std::string mail) : Response() {
        this->uid = uid;
        this->cn = cn;
        this->mail = mail;
    }

    std::string get_object_name() {
        return this->object_name;
    }

    void set_object_name(std::string object_name) {
        this->object_name = object_name;
    }

    std::string get_uid() {
        return this->uid;
    }

    void set_uid(std::string uid) {
        this->uid = uid;
    }

    std::string get_cn() {
        return this->cn;
    }

    void set_cn(std::string cn) {
        this->cn = cn;
    }

    std::string get_mail() {
        return this->mail;
    }

    void set_mail(std::string mail) {
        this->mail = mail;
    }

    void build() {
        // objectName
        std::string objectname = "uid=" + this->uid + ",ou=fit,dc=vutbr,dc=cz";
        ber_bytes objectname_bytes = this->writer.create_octet_string(objectname);

        // attributes
        ber_bytes attributes_bytes = this->writer.build_partialattributelist({std::make_tuple("uid", uid), std::make_tuple("cn", cn), std::make_tuple("mail", mail)});

        // Put together the SearchResultEntry
        ber_bytes searchresultentry_bytes = this->writer.create_sequence({objectname_bytes, attributes_bytes}, this->tag);

        ber_bytes ldapmessage_bytes = this->writer.build_ldapmessage(this->get_message_id(), searchresultentry_bytes);

        this->bytes = ldapmessage_bytes;
    }
};

class SearchResDone : public Response {
    unsigned char tag = SEARCH_RESULT_DONE;
    unsigned char result_code;
    std::string matched_dn = "";
    std::string error_message = "";

   public:
    SearchResDone(int result_code) : Response() {
        this->result_code = result_code;
    }

    unsigned char get_result_code() {
        return this->result_code;
    }

    void set_result_code(unsigned char result_code) {
        this->result_code = result_code;
    }

    std::string get_matched_dn() {
        return this->matched_dn;
    }

    void set_matched_dn(std::string matched_dn) {
        this->matched_dn = matched_dn;
    }

    std::string get_error_message() {
        return this->error_message;
    }

    void set_error_message(std::string error_message) {
        this->error_message = error_message;
    }

    void build() {
        // ResultCode
        ber_bytes result_code_bytes = this->writer.create_enumerated(this->result_code);

        // MatchedDN
        ber_bytes matched_dn_bytes = this->writer.create_octet_string(this->matched_dn);

        // DiagnosticMessage
        ber_bytes diagnostic_message_bytes = this->writer.create_octet_string(this->error_message);

        // Referral

        // ServerSaslCreds

        // Put together the BindResponse
        ber_bytes searchresultdone_bytes = this->writer.create_sequence({result_code_bytes, matched_dn_bytes, diagnostic_message_bytes}, this->tag);

        ber_bytes ldapmessage_bytes = this->writer.build_ldapmessage(this->get_message_id(), searchresultdone_bytes);

        this->bytes = ldapmessage_bytes;
    }
};

class UnbindRequest : public Request {
    unsigned char tag;

   public:
    UnbindRequest(ber_bytes bytes) : Request(bytes) {
        this->ldapmessage_tag = reader.read_tag();
        this->message_id = this->reader.read_integer();
        this->parse();
    }

    unsigned char get_tag() {
        return this->tag;
    }

   private:
    void parse() {
        this->tag = this->reader.read_tag();
    }
};
