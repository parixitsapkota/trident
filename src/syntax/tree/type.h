#pragma once

#include <stddef.h>

#include "core/arena.h"
#include "syntax/token/token.h"

typedef enum {
    TYPE_PRIMITIVE,
    TYPE_POINTER,
    TYPE_SLICE,
    TYPE_ARRAY,
    TYPE_FUNCTION,
    TYPE_STRUCTURE,
    TYPE_UNION,
    TYPE_ENUMERATION,
    TYPE_ERROR,
    TYPE_TYPE,
} TypeKind;

typedef enum {
    PRIMITIVE_VOID,
    PRIMITIVE_I8, // TYPE ICHAR
    PRIMITIVE_I16,
    PRIMITIVE_I32,
    PRIMITIVE_I64,
    PRIMITIVE_I128,
    PRIMITIVE_ISIZE,
    PRIMITIVE_U8,  // TYPE CHAR
    PRIMITIVE_U16, // TYPE WCHAR
    PRIMITIVE_U32,
    PRIMITIVE_U64,
    PRIMITIVE_U128,
    PRIMITIVE_USIZE,
    PRIMITIVE_BOOL,
    PRIMITIVE_F16,
    PRIMITIVE_F32,
    PRIMITIVE_F64,
    PRIMITIVE_F128,
    PRIMITIVE_STR,
} PrimitiveKind;

typedef struct Type Type;
typedef struct Member Member;

struct Member {
    Type *type;       // member typr
    const char *name; // member name
    Member *next;     // next member;
};

struct Type {
    TypeKind kind;
    union {
        /* TYPE_PRIMITIVE */
        PrimitiveKind primitive;

        /* TYPE_POINTER, (dynamicly sized array)TYPE_SLICE */
        Type *element;

        /* (staticly sized array)TYPE_ARRAY */
        struct {
            Type *type;
            size_t size;
        } array;

        /* TYPE_FUNCTION */
        struct {
            Member *params;      // parameters
            size_t params_count; // parameters count
            Type *ret_type;      // return type
        } func;

        /* TYPE_STRUCTURE, TYPE_UNION, TYPE_ENUMERATION, TYPE_ERROR */
        struct {
            Member *members;      // members
            size_t members_count; // members count
            const char *name;     // object name
        } object;

        /* TYPE_TYPE */
        struct {
            Type *type;       // type type
            const char *name; // type name
        } type;
    } as;
};

Type *type_primitive(Arena *arena, const Token *kind);
Type *type_pointer(Arena *arena, Type *base);
Type *type_slice(Arena *arena, Type *base);
Type *type_array(Arena *arena, Type *base, size_t size);
Type *type_function(Arena *arena, Member *params, size_t params_count, Type *ret_type);
Type *type_object(Arena *arena, TypeKind kind, Member *members, size_t members_count,
                  const char *name);
Type *type_type(Arena *arena, Type *base, const char *name);
