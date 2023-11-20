/*
 * ldap.h
 * 2023-11-20
 *
 * ISA project 2023
 * LDAP server
 *
 * Matyas Strelec xstrel03
 */

#pragma once

#include <iostream>
#include <vector>

#include "ber.cpp"
#include "ber.h"


// LDAP message tags
#define LDAP_MESSAGE 0x30
#define BIND_REQUEST 0x60
#define BIND_RESPONSE 0x61
#define SEARCH_REQUEST 0x63
#define SEARCH_RESULT_ENTRY 0x64
#define SEARCH_RESULT_DONE 0x65
#define UNBIND_REQUEST 0x42

// Result
#define RESULT_SIZE_LIMIT_EXCEEDED 0x04

// Scope
#define SCOPE_WHOLE_SUBTREE 0x02

// LDAP result code enum
#define RESULT_SUCCESS 0x00
#define RESULT_OPERATIONS_ERROR 0x01
#define RESULT_PROTOCOL_ERROR 0x02