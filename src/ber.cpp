/*
 * ber.cpp
 * 2023-11-20
 *
 * ISA project 2023
 * LDAP server
 *
 * Matyas Strelec xstrel03
 */

#include "ber.h"

#include <iomanip>
#include <iostream>

#include "filter.h"

class BERreader {
    size_t iterator = 0;
    ber_bytes bytes = ber_bytes();

   public:
    BERreader() {
        this->bytes = ber_bytes();
        this->iterator = 0;
    }

    BERreader(ber_bytes bytes) {
        this->bytes = bytes;
        this->iterator = 0;
    }

    void reset() {
        this->iterator = 0;
    }

    int read_integer() {
        unsigned char tag = this->get_next_byte();
        if (tag != BER_INTEGER) {
            throw std::runtime_error("Error reading integer (unexpected tag)");
        }
        size_t length = this->get_length();
        if (length > 4) {
            throw std::runtime_error("Error reading integer (unexpected length)");
        }
        int integer = 0;
        for (size_t i = 0; i < length; i++) {
            integer = integer << 8;
            integer += this->get_next_byte();
        }
        return integer;
    }

    bool read_boolean() {
        unsigned char tag = this->get_next_byte();
        if (tag != BER_BOOLEAN) {
            throw std::runtime_error("Error reading boolean (unexpected tag)");
        }
        size_t length = this->get_length();
        if (length > 1) {
            throw std::runtime_error("Error reading boolean (unexpected length)");
        }
        bool boolean = this->get_next_byte();
        return boolean;
    }

    std::string read_octet_string() {
        unsigned char tag = this->get_next_byte();
        if (tag != BER_OCTET_STRING) {
            throw std::runtime_error("Error reading octet string (unexpected tag)");
        }
        size_t length = this->get_length();
        std::string octet_string = "";
        for (size_t i = 0; i < length; i++) {
            octet_string += this->get_next_byte();
        }
        return octet_string;
    }

    void read_sequence() {
        unsigned char tag = this->get_next_byte();
        if (tag != BER_SEQUENCE) {
            throw std::runtime_error("Error reading sequence (unexpected tag)");
        }
        this->get_length();
    }

    void read_set() {
        unsigned char tag = this->get_next_byte();
        if (tag != BER_SET) {
            throw std::runtime_error("Error reading set (unexpected tag)");
        }
        this->get_length();
    }

    int read_enumerated() {
        unsigned char tag = this->get_next_byte();
        if (tag != BER_ENUMERATED) {
            throw std::runtime_error("Error reading enumerated (unexpected tag)");
        }
        size_t length = this->get_length();
        if (length > 4) {
            throw std::runtime_error("Error reading enumerated (unexpected tag)");
        }
        int enumerated = 0;
        for (size_t i = 0; i < length; i++) {
            enumerated = enumerated << 8;
            enumerated += this->get_next_byte();
        }
        return enumerated;
    }

    unsigned char read_tag() {
        unsigned char tag = this->get_next_byte();
        get_length();
        return tag;
    }

    filter read_filters() {
        unsigned char tag = this->get_next_byte();
        this->get_length();
        filter filter;

        // print tag as hex
        std::cout << "tag: ";
        std::cout << std::hex << std::setfill('0') << std::setw(2) << static_cast<int>(tag) << std::endl;

        if (tag == FILTER_EQUALITY_MATCH) {
            filter.tag = FILTER_EQUALITY_MATCH;
            filter.attribute = this->read_octet_string();
            filter.value = this->read_octet_string();

        } else if (tag == FILTER_SUBSTRINGS) {
            filter.tag = FILTER_SUBSTRINGS;
            filter.attribute = this->read_octet_string();
            this->read_sequence();
            // substrings
            unsigned char substrings_tag;
            size_t substrings_length;

            // loop through all
            while (true) {
                substrings_tag = this->get_next_byte();
                if (substrings_tag != 0x80 && substrings_tag != 0x81 && substrings_tag != 0x82) {
                    this->go_back_byte();
                    break;
                }
                substrings_length = this->get_length();
                std::string substrings_value = "";
                for (size_t i = 0; i < substrings_length; i++) {
                    substrings_value += this->get_next_byte();
                }
                if (substrings_tag == 0x80) {
                    filter.initial.push_back(substrings_value);
                } else if (substrings_tag == 0x81) {
                    filter.any.push_back(substrings_value);
                } else if (substrings_tag == 0x82) {
                    filter.final.push_back(substrings_value);
                }
            }
        } else if (tag == FILTER_AND) {
            filter.tag = FILTER_AND;

            while (true) {
                std::cout << "reading filter" << std::endl;
                struct filter f = this->read_filters();
                if (f.tag == 0x00) {
                    break;
                }
                filter.filters.push_back(f);
            }

        } else if (tag == FILTER_OR) {
            filter.tag = FILTER_OR;

            while (true) {
                std::cout << "reading filter" << std::endl;
                struct filter f = this->read_filters();
                if (f.tag == 0x00) {
                    break;
                }
                filter.filters.push_back(f);
            }
        } else if (tag == FILTER_NOT) {
            filter.tag = FILTER_NOT;
            struct filter f = this->read_filters();
            filter.filters.push_back(f);
        } else {
            filter.tag = 0x00;
        }

        return filter;
    }

    // Decode length (short form or long form)
    size_t get_length() {
        size_t length_byte = static_cast<size_t>(this->get_next_byte());
        if (length_byte < 128) {
            return length_byte;
        } else {
            size_t length = 0;
            for (size_t i = 0; i < length_byte - 128; i++) {
                length = length << 8;
                length += this->get_next_byte();
            }
            return length;
        }
    }

   private:
    // Read next byte (unsigned char) from the bytes, increment iterator
    unsigned char get_next_byte() {
        if (this->iterator >= this->bytes.size()) {
            throw std::runtime_error("Error reading byte (out of bounds)");
        } else {
            return this->bytes[this->iterator++];
        }
    }

    void go_back_byte() {
        if (this->iterator > 0) {
            this->iterator--;
        }
    }
};

class BERwriter {
    ber_bytes bytes = ber_bytes();

   public:
    BERwriter() {}

    ber_bytes build_ldapmessage(int message_id, ber_bytes protocolop_bytes) {
        // MessageID
        ber_bytes message_id_bytes = this->create_integer(message_id);

        // ProtocolOp
        ber_bytes protocol_op_bytes = protocolop_bytes;

        // Controls
        // TODO implement controls

        // Put together the LDAPMessage
        ber_bytes ldapmessage_bytes;

        ldapmessage_bytes.push_back(BER_SEQUENCE);  // LDAPMessage
        ber_bytes length_bytes = this->create_length(message_id_bytes.size() + protocol_op_bytes.size());
        for (unsigned char c : length_bytes) {
            ldapmessage_bytes.push_back(c);
        }
        ldapmessage_bytes.insert(ldapmessage_bytes.end(), message_id_bytes.begin(), message_id_bytes.end());
        ldapmessage_bytes.insert(ldapmessage_bytes.end(), protocol_op_bytes.begin(), protocol_op_bytes.end());

        return ldapmessage_bytes;
    }

    ber_bytes create_length(size_t input) {
        ber_bytes output;

        if (input < 128) {
            // short form
            output.push_back(input);
        } else {
            // long form
            int bytes_needed = 0;
            size_t temp = input;

            while (temp > 0) {
                temp >>= 8;
                bytes_needed++;
            }

            // length of the length bytes
            output.push_back(static_cast<unsigned char>(0x80 | bytes_needed));

            // actual length bytes
            for (int i = bytes_needed - 1; i >= 0; i--) {
                output.push_back(static_cast<unsigned char>((input >> (8 * i)) & 0xFF));
            }
        }
        return output;
    }

    ber_bytes create_integer(int input) {
        ber_bytes output;
        output.push_back(BER_INTEGER);
        ber_bytes length = create_length(1);
        for (unsigned char c : length) {
            output.push_back(c);
        }
        output.push_back(input);
        return output;
    }

    ber_bytes create_octet_string(std::string input) {
        ber_bytes output;
        output.push_back(BER_OCTET_STRING);
        ber_bytes length = create_length(input.size());
        for (unsigned char c : length) {
            output.push_back(c);
        }
        for (char i : input) {
            output.push_back(i);
        }
        return output;
    }

    ber_bytes create_sequence(std::vector<ber_bytes> input) {
        ber_bytes output;
        output.push_back(BER_SEQUENCE);
        int total_size = 0;
        for (ber_bytes i : input) {
            total_size += i.size();
        }
        ber_bytes length = create_length(total_size);
        for (unsigned char c : length) {
            output.push_back(c);
        }
        for (ber_bytes i : input) {
            for (unsigned char c : i) {
                output.push_back(c);
            }
        }
        return output;
    };

    ber_bytes create_sequence(std::vector<ber_bytes> input, unsigned char tag) {
        ber_bytes output;
        output.push_back(tag);
        int total_size = 0;
        for (ber_bytes i : input) {
            total_size += i.size();
        }
        ber_bytes length = create_length(total_size);
        for (unsigned char c : length) {
            output.push_back(c);
        }
        for (ber_bytes i : input) {
            for (unsigned char c : i) {
                output.push_back(c);
            }
        }
        return output;
    };

    ber_bytes create_set(std::vector<ber_bytes> input) {
        ber_bytes output;
        output.push_back(BER_SET);
        int total_size = 0;
        for (ber_bytes i : input) {
            total_size += i.size();
        }
        ber_bytes length = create_length(total_size);
        for (unsigned char c : length) {
            output.push_back(c);
        }
        for (ber_bytes i : input) {
            for (unsigned char c : i) {
                output.push_back(c);
            }
        }
        return output;
    };

    ber_bytes create_enumerated(int input) {
        ber_bytes output;
        output.push_back(BER_ENUMERATED);
        ber_bytes length = create_length(1);
        for (unsigned char c : length) {
            output.push_back(c);
        }
        output.push_back(input);
        return output;
    }

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
};
