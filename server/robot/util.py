
def to_enum(enum_type, value):
    if isinstance(value, int):
        return enum_type(value)
    for k, v in enum_type.values.items():
        if value == str(v):
            return v
    raise ValueError(f"Unknown enum value {value}")
