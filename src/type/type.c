#include <stdbool.h>

#include "type.h"

#include "core/mem.h"
#include "syntax/token/token.h"

bool is_primitive_kind(const TokenKind kind) {
    switch (kind) {
    case KW_VOID:
    case KW_I8:
    case KW_I16:
    case KW_I32:
    case KW_I64:
    case KW_I128:
    case KW_ISIZE:
    case KW_U8:
    case KW_U16:
    case KW_U32:
    case KW_U64:
    case KW_U128:
    case KW_USIZE:
    case KW_BOOL:
    case KW_F16:
    case KW_F32:
    case KW_F64:
    case KW_F128:
    case KW_STR: return true;
    default: return false;
    }
}

PrimitiveKind primitive_kind(const TokenKind kind) {
    switch (kind) {
    case KW_VOID: return PRIMITIVE_VOID;
    case KW_I8: return PRIMITIVE_I8;
    case KW_I16: return PRIMITIVE_I16;
    case KW_I32: return PRIMITIVE_I32;
    case KW_I64: return PRIMITIVE_I64;
    case KW_I128: return PRIMITIVE_I128;
    case KW_ISIZE: return PRIMITIVE_ISIZE;
    case KW_U8: return PRIMITIVE_U8;
    case KW_U16: return PRIMITIVE_U16;
    case KW_U32: return PRIMITIVE_U32;
    case KW_U64: return PRIMITIVE_U64;
    case KW_U128: return PRIMITIVE_U128;
    case KW_USIZE: return PRIMITIVE_USIZE;
    case KW_BOOL: return PRIMITIVE_BOOL;
    case KW_F16: return PRIMITIVE_F16;
    case KW_F32: return PRIMITIVE_F32;
    case KW_F64: return PRIMITIVE_F64;
    case KW_F128: return PRIMITIVE_F128;
    case KW_STR: return PRIMITIVE_STR;
    default: return PRIMITIVE_NONE;
    }
}

Type *type_primitive(Arena *arena, PrimitiveKind kind) {
    Type *t = arena_alloc(arena, sizeof(Type));
    *t = (Type){TYPE_PRIMITIVE, .as.primitive = kind};
    return t;
}

Type *type_pointer(Arena *arena, Type *base) {
    Type *t = arena_alloc(arena, sizeof(Type));
    *t = (Type){TYPE_POINTER, .as.element = base};
    return t;
}

Type *type_slice(Arena *arena, Type *base) {
    Type *t = arena_alloc(arena, sizeof(Type));
    *t = (Type){TYPE_SLICE, .as.element = base};
    return t;
}

Type *type_array(Arena *arena, Type *base, size_t size) {
    Type *t = arena_alloc(arena, sizeof(Type));
    *t = (Type){TYPE_SLICE, .as.array.type = base, .as.array.size = size};
    return t;
}

Type *type_function(Arena *arena, Member *params, size_t params_count, Type *ret_type) {
    Type *t = arena_alloc(arena, sizeof(Type));
    *t = (Type){TYPE_SLICE, .as.func.params = params,
                .as.func.params_count = params_count, .as.func.ret_type = ret_type};
    return t;
}

Type *type_object(Arena *arena, TypeKind kind, Member *members, size_t members_count,
                  const char *name) {
    Type *t = arena_alloc(arena, sizeof(Type));
    *t = (Type){kind, .as.object.members = members,
                .as.object.members_count = members_count, .as.object.name = name};
    return t;
}

Type *type_type(Arena *arena, Type *base, const char *name) {
    Type *t = arena_alloc(arena, sizeof(Type));
    *t = (Type){TYPE_TYPE, .as.type.type = base, .as.type.name = name};
    return t;
}

Info *info(Arena *arena, InfoKind kind, Type *type) {
    Info *info = arena_alloc(arena, sizeof(Info));
    *info = (Info){kind, type};
    return info;
}
